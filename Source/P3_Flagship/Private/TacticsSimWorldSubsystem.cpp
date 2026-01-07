// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticsSimWorldSubsystem.h"

#include "GridWorldSubsystem.h"
#include "UnitActor.h"

#include "Engine/World.h"

void UTacticsSimWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	NextId = 1;
	bCombatActive = false;
	TurnOrder.Reset();
	TurnIndex = 0;
	ActiveEntity = 0;

	Units.Reset();
	Occupancy.Reset();

	while (!CommandQueue.IsEmpty()) {
		TacticsCore::Command Temp;
		CommandQueue.Dequeue(Temp);
	}
}

void UTacticsSimWorldSubsystem::Deinitialize() {
	Super::Deinitialize();
}

bool UTacticsSimWorldSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const {
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId UTacticsSimWorldSubsystem::GetStatId() const {
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTacticsSimWorldSubsystem, STATGROUP_Tickables);
}

void UTacticsSimWorldSubsystem::Tick(float DeltaTime) {
	ProcessCommands(MaxCommandsPerTick);
}

TacticsCore::EntityId UTacticsSimWorldSubsystem::RegisterUnit(AUnitActor* UnitActor, int32 Team) {
	if (!UnitActor) {
		return 0;
	}

	//If unit is already registered just reject it.
	for (const auto& Kvp : Units) {
		if (Kvp.Value.Actor == UnitActor) {
			return Kvp.Key;
		}
	}

	UGridWorldSubsystem* Grid = GetWorld() ? GetWorld()->GetSubsystem<UGridWorldSubsystem>() : nullptr;
	if (!Grid) {
		return 0;
	}

	const TacticsCore::TilePos Tile = UnitActor->GetTile();
	if (!Grid->InBounds(Tile)) {
		return 0;
	}

	if (IsTileOccupied(Tile)) {
		return 0;
	}

	const uint32 Id = NextId++;
	FTacticsUnitState S;
	S.Id = Id;
	S.Team = Team;
	S.Actor = UnitActor;
	S.Tile = Tile;
	S.bAlive = true;

	Units.Add(Id, S);
	SetOccupant(Tile, Id);

	UE_LOG(LogTemp, Log, TEXT("[Sim] Registered Unit Id=%u Team=%d Tile=(%d,%d) Actor=%s"), Id, Team, Tile.x, Tile.y, *GetNameSafe(UnitActor));

	return Id;
}

bool UTacticsSimWorldSubsystem::UnregisterUnit(TacticsCore::EntityId Id) {
	FTacticsUnitState* S = Units.Find(Id);
	if (!S) {
		return false;
	}

	ClearOccupant(S->Tile);
	Units.Remove(Id);

	//If combat is active we need to rebuild the turn order
	if (bCombatActive) {
		RebuildTurnOrder();
		if (TurnOrder.Num() > 0) {
			TurnIndex = FMath::Clamp(TurnIndex, 0, TurnOrder.Num() - 1);
			ActiveEntity = TurnOrder[TurnIndex];
			AdvanceTurnToNextAlive();
		}
		else {
			ActiveEntity = 0;
		}
	}

	return true;
}

bool UTacticsSimWorldSubsystem::TryGetUnitState(TacticsCore::EntityId Id, FTacticsUnitState& Out) const {
	if (const FTacticsUnitState* Found = Units.Find(Id)) {
		Out = *Found;
		return true;
	}

	return false;
}

void UTacticsSimWorldSubsystem::StartCombat() {
	bCombatActive = true;
	RebuildTurnOrder();

	TurnIndex = 0;
	ActiveEntity = (TurnOrder.Num() > 0) ? TurnOrder[0] : 0;

	AdvanceTurnToNextAlive();

	UE_LOG(LogTemp, Log, TEXT("[Sim] StartCombat: Units=%d TurnOrder=%d Active=%u"), Units.Num(), TurnOrder.Num(), ActiveEntity);

	if (ActiveEntity != 0)
	{
		const FTacticsUnitState* S = Units.Find(ActiveEntity);
		if (S)
		{
			UE_LOG(LogTemp, Log, TEXT("[Sim] Active Unit Tile=(%d,%d) Actor=%s"), S->Tile.x, S->Tile.y, *GetNameSafe(S->Actor));
		}
	}
}

void UTacticsSimWorldSubsystem::EndCombat() {
	bCombatActive = false;
	TurnOrder.Reset();
	TurnIndex = 0;
	ActiveEntity = 0;

	while (!CommandQueue.IsEmpty()) {
		TacticsCore::Command Tmp;
		CommandQueue.Dequeue(Tmp);
	}
}

void UTacticsSimWorldSubsystem::EnqueueCommand(const TacticsCore::Command& Cmd) {
	CommandQueue.Enqueue(Cmd);
}

void UTacticsSimWorldSubsystem::ProcessCommands(int32 MaxPerTick) {
	for (int32 i = 0; i < MaxPerTick; ++i) {
		TacticsCore::Command Cmd;
		if (!CommandQueue.Dequeue(Cmd)) {
			break;
		}

		ApplyCommand(Cmd);
	}
}

void UTacticsSimWorldSubsystem::ApplyCommand(const TacticsCore::Command& Cmd) {
	switch (Cmd.type) {
		case TacticsCore::CommandType::Move:
			ApplyMove(Cmd.move);
			break;
		case TacticsCore::CommandType::EndTurn:
			ApplyEndTurn();
			break;

		default:
			break;
	}
}

bool UTacticsSimWorldSubsystem::CanAct(TacticsCore::EntityId Id) const {
	if (!bCombatActive) {
		return true;
	}

	if (Id == 0 || Id != ActiveEntity) {
		return false;
	}

	const FTacticsUnitState* S = Units.Find(Id);
	return S && S->bAlive;
}

void UTacticsSimWorldSubsystem::ApplyMove(const TacticsCore::MoveCommand& Move) {
	UWorld* W = GetWorld();
	if (!W) {
		return;
	}

	UGridWorldSubsystem* Grid = W->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) {
		return;
	}

	if (!CanAct(Move.entity)) {
		return;
	}

	FTacticsUnitState* S = Units.Find(Move.entity);
	if (!S || !S->bAlive || !S->Actor) {
		return;
	}

	if (!Grid->InBounds(Move.dest)) {
		return;
	}

	if (Move.dest != S->Tile && IsTileOccupied(Move.dest)) {
		return;
	}

	const TacticsCore::TilePos From = S->Tile;
	const TacticsCore::TilePos To = Move.dest;

	if (From != To) {
		ClearOccupant(From);
		SetOccupant(To, S->Id);
		S->Tile = To;

		S->Actor->SetTile(To);
	}
}

void UTacticsSimWorldSubsystem::ApplyEndTurn() {
	if (!bCombatActive || TurnOrder.Num() == 0) {
		return;
	}


	// If no active entity attempt to recover.
	if (ActiveEntity == 0)
	{
		AdvanceTurnToNextAlive();
		return;
	}

	// Advance index and then skip dead or missing
	TurnIndex = (TurnIndex + 1) % TurnOrder.Num();
	ActiveEntity = TurnOrder[TurnIndex];

	AdvanceTurnToNextAlive();
}

void UTacticsSimWorldSubsystem::RebuildTurnOrder() {
	TurnOrder.Reset();

	TArray<uint32> Keys;
	Units.GetKeys(Keys);
	Keys.Sort();

	for (uint32 Id : Keys) {
		const FTacticsUnitState* S = Units.Find(Id);
		if (S && S->bAlive) {
			TurnOrder.Add(Id);
		}
	}
}

void UTacticsSimWorldSubsystem::AdvanceTurnToNextAlive() {
	if (TurnOrder.Num() == 0) {
		ActiveEntity = 0;
		return;
	}

	for (int32 Attempts = 0; Attempts < TurnOrder.Num(); ++Attempts) {
		const uint32 Candidate = TurnOrder[TurnIndex];
		const FTacticsUnitState* S = Units.Find(Candidate);

		if (S && S->bAlive) {
			ActiveEntity = Candidate;
			return;
		}

		TurnIndex = (TurnIndex + 1) % TurnOrder.Num();
	}

	//Cant find valid unit
	ActiveEntity = 0;
}

int64 UTacticsSimWorldSubsystem::PackTileKey(const TacticsCore::TilePos& Tile) {
	const uint64 X = static_cast<uint32>(Tile.x);
	const uint64 Y = static_cast<uint32>(Tile.y);
	return static_cast<int64>((X << 32) | Y);
}

bool UTacticsSimWorldSubsystem::IsTileOccupied(const TacticsCore::TilePos& Tile) const {
	return Occupancy.Contains(PackTileKey(Tile));
}

TacticsCore::EntityId UTacticsSimWorldSubsystem::GetOccupant(const TacticsCore::TilePos& Tile) const {
	if (const uint32* Found = Occupancy.Find(PackTileKey(Tile))) {
		return *Found;
	}
	return 0;
}

void UTacticsSimWorldSubsystem::SetOccupant(const TacticsCore::TilePos& Tile, TacticsCore::EntityId Id) {
	Occupancy.Add(PackTileKey(Tile), Id);
}

void UTacticsSimWorldSubsystem::ClearOccupant(const TacticsCore::TilePos& Tile) {
	Occupancy.Remove(PackTileKey(Tile));
}
