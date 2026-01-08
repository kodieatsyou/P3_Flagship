#include "TacticsGameStateSubsystem.h"

#include "UnitActor.h"
#include "GridWorldSubsystem.h"
#include "TacticsWorldSettingsUtil.h"
#include "TacticsWorldSettings.h"
#include "Engine/World.h"

void UTacticsGameStateSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (const ATacticsWorldSettings* WS = TacticsWorldSettingsUtil::Get(&InWorld))
	{
		UnitActorClass = WS->UnitActorClass;
		Team0SpawnTiles = WS->Team0SpawnTiles;
		Team1SpawnTiles = WS->Team1SpawnTiles;
		DefaultUnitDef = WS->DefaultUnitDef;
	}

	BuildPrototypeTeamsSquadsUnits();
	SpawnAllUnits();

	// Start-of-game: team 0 active refresh MPs for active team
	ActiveTeam = ETacticsTeam::Team0;
	RefreshMovePointsForActiveTeam();
}

void UTacticsGameStateSubsystem::BuildPrototypeTeamsSquadsUnits()
{
	Teams.Reset();
	Squads.Reset();
	Units.Reset();
	NextSquadId = 0;
	NextUnitId = 0;

	FTacticsTeamRecord T0;
	T0.Team = ETacticsTeam::Team0;
	T0.TeamName = "Team 0";
	Teams.Add(T0);

	FTacticsTeamRecord T1;
	T1.Team = ETacticsTeam::Team1;
	T1.TeamName = "Team 1";
	Teams.Add(T1);

	const int32 Squad0Id = CreateSquad(ETacticsTeam::Team0, "Alpha");
	const int32 Squad1Id = CreateSquad(ETacticsTeam::Team1, "Bravo");

	Teams[0].SquadIds.Add(Squad0Id);
	Teams[1].SquadIds.Add(Squad1Id);

	for (const FIntPoint& P : Team0SpawnTiles)
	{
		CreateUnit(Squad0Id, ETacticsTeam::Team0, DefaultUnitDef, TacticsCore::TilePos(P.X, P.Y));
	}
	for (const FIntPoint& P : Team1SpawnTiles)
	{
		CreateUnit(Squad1Id, ETacticsTeam::Team1, DefaultUnitDef, TacticsCore::TilePos(P.X, P.Y));
	}

	if (Team0SpawnTiles.Num() == 0 && Team1SpawnTiles.Num() == 0)
	{
		CreateUnit(Squad0Id, ETacticsTeam::Team0, DefaultUnitDef, TacticsCore::TilePos(2, 2));
		CreateUnit(Squad0Id, ETacticsTeam::Team0, DefaultUnitDef, TacticsCore::TilePos(2, 4));

		CreateUnit(Squad1Id, ETacticsTeam::Team1, DefaultUnitDef, TacticsCore::TilePos(9, 2));
		CreateUnit(Squad1Id, ETacticsTeam::Team1, DefaultUnitDef, TacticsCore::TilePos(9, 4));
	}
}

int32 UTacticsGameStateSubsystem::CreateSquad(ETacticsTeam Team, FName SquadName)
{
	FTacticsSquadRecord S;
	S.SquadId = NextSquadId++;
	S.Team = Team;
	S.SquadName = SquadName;
	Squads.Add(S);
	return S.SquadId;
}

int32 UTacticsGameStateSubsystem::CreateUnit(int32 SquadId, ETacticsTeam Team, const FTacticsUnitDef& Def, const TacticsCore::TilePos& SpawnTile)
{
	FTacticsUnitRecord U;
	U.UnitId = NextUnitId++;
	U.SquadId = SquadId;
	U.Team = Team;
	U.Def = Def;
	U.SpawnTile = SpawnTile;
	Units.Add(U);

	// Attach to squad record
	for (FTacticsSquadRecord& S : Squads)
	{
		if (S.SquadId == SquadId)
		{
			S.UnitIds.Add(U.UnitId);
			break;
		}
	}

	return U.UnitId;
}

void UTacticsGameStateSubsystem::SpawnAllUnits()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!UnitActorClass)
	{
		UnitActorClass = AUnitActor::StaticClass();
	}

	for (FTacticsUnitRecord& U : Units)
	{
		AUnitActor* Actor = SpawnUnitActor(U);
		U.Actor = Actor;
	}
}

AUnitActor* UTacticsGameStateSubsystem::SpawnUnitActor(const FTacticsUnitRecord& UnitRec)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	UGridWorldSubsystem* Grid = World->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) return nullptr;

	const FVector SpawnWorld = Grid->TileToWorldCenter(UnitRec.SpawnTile);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AUnitActor* UnitActor = World->SpawnActor<AUnitActor>(UnitActorClass, SpawnWorld, FRotator::ZeroRotator, Params);
	if (!UnitActor) return nullptr;

	UnitActor->InitializeFromGameState(
		UnitRec.UnitId,
		UnitRec.SquadId,
		UnitRec.Team,
		UnitRec.Def,
		UnitRec.SpawnTile
	);

	return UnitActor;
}

void UTacticsGameStateSubsystem::EndTurn()
{
	ActiveTeam = (ActiveTeam == ETacticsTeam::Team0) ? ETacticsTeam::Team1 : ETacticsTeam::Team0;
	RefreshMovePointsForActiveTeam();
}

void UTacticsGameStateSubsystem::RefreshMovePointsForActiveTeam()
{
	for (FTacticsUnitRecord& U : Units)
	{
		if (U.Team != ActiveTeam) continue;
		if (!U.Actor) continue;
		U.Actor->RefreshForNewTurn();
	}
}

bool UTacticsGameStateSubsystem::IsUnitControllable(const AUnitActor* UnitActor) const
{
	const FTacticsUnitRecord* Rec = GetUnitRecordByActor(UnitActor);
	if (!Rec) {
		return false;
	}
	return Rec->Team == ActiveTeam;
}

const FTacticsUnitRecord* UTacticsGameStateSubsystem::GetUnitRecordByActor(const AUnitActor* UnitActor) const
{
	if (!UnitActor) return nullptr;

	for (const FTacticsUnitRecord& U : Units)
	{
		if (U.Actor == UnitActor) {
			return &U;
		}
	}
	return nullptr;
}

FTacticsUnitRecord* UTacticsGameStateSubsystem::GetUnitRecordByActor(AUnitActor* UnitActor)
{
	if (!UnitActor) return nullptr;

	for (FTacticsUnitRecord& U : Units)
	{
		if (U.Actor == UnitActor) {
			return &U;
		}
	}
	return nullptr;
}

void UTacticsGameStateSubsystem::HandleUnitDeath(AUnitActor* UnitActor)
{
	if (!UnitActor) {
		return;
	}

	for (int32 i = Units.Num() - 1; i >= 0; --i)
	{
		if (Units[i].Actor == UnitActor)
		{
			Units.RemoveAt(i);
			break;
		}
	}

	UnitActor->Destroy();
}