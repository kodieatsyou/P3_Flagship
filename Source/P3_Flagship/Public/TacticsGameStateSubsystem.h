

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TacticsCoreTypes.h"
#include "TacticsGameStateSubsystem.generated.h"

class AUnitActor;

UENUM(BlueprintType)
enum class ETacticsTeam : uint8
{
	Team0 = 0,
	Team1 = 1
};

USTRUCT(BlueprintType)
struct FTacticsWeaponDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WeaponId = "Rifle";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RangeTiles = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Damage = 3;
};

USTRUCT(BlueprintType)
struct FTacticsUnitDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName UnitNAme = "Unit";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MovePointsMax = 6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTacticsWeaponDef Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName UtilityId = "None";
};

USTRUCT(BlueprintType)
struct FTacticsUnitRecord
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 UnitId = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SquadId = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETacticsTeam Team = ETacticsTeam::Team0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTacticsUnitDef Def;

	TacticsCore::TilePos SpawnTile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AUnitActor> Actor = nullptr;
};

USTRUCT(BlueprintType)
struct FTacticsSquadRecord
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SquadId = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETacticsTeam Team = ETacticsTeam::Team0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName SquadName = "Squad";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> UnitIds;
};

USTRUCT(BlueprintType)
struct FTacticsTeamRecord
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETacticsTeam Team = ETacticsTeam::Team0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName TeamName = "Team";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> SquadIds;
};

UCLASS()
class P3_FLAGSHIP_API UTacticsGameStateSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// -- Authoring --
	UPROPERTY(EditAnywhere, Category = "Tactics|Spawn")
	TSubclassOf<AUnitActor> UnitActorClass;

	UPROPERTY(EditAnywhere, Category = "Tactics|Spawn")
	TArray<FIntPoint> Team0SpawnTiles;

	UPROPERTY(EditAnywhere, Category = "Tactics|Spawn")
	TArray<FIntPoint> Team1SpawnTiles;

	UPROPERTY(EditAnywhere, Category = "Tactics|Defs")
	FTacticsUnitDef DefaultUnitDef;

	// -- Lifecycle --
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// -- Turn --
	ETacticsTeam GetActiveTeam() const {
		return ActiveTeam;
	}
	void EndTurn();

	// -- Queries --
	bool IsUnitControllable(const AUnitActor* UnitActor) const;
	const FTacticsUnitRecord* GetUnitRecordByActor(const AUnitActor* UnitActor) const;
	FTacticsUnitRecord* GetUnitRecordByActor(AUnitActor* UnitActor);

	void RefreshMovePointsForActiveTeam();

private:
	// -- State --
	ETacticsTeam ActiveTeam = ETacticsTeam::Team0;

	UPROPERTY()
	TArray<FTacticsTeamRecord> Teams;

	UPROPERTY()
	TArray<FTacticsSquadRecord> Squads;

	UPROPERTY()
	TArray<FTacticsUnitRecord> Units;

	void BuildPrototypeTeamsSquadsUnits();
	void SpawnAllUnits();
	AUnitActor* SpawnUnitActor(const FTacticsUnitRecord& UnitRec);

	int32 NextSquadId = 0;
	int32 NextUnitId = 0;

	int32 CreateSquad(ETacticsTeam Team, FName SquadName);
	int32 CreateUnit(int32 SquadId, ETacticsTeam Team, const FTacticsUnitDef& Def, const TacticsCore::TilePos& SpawnTile);
};