#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TacticsCoreTypes.h"
#include "TacticsGameStateSubsystem.h"
#include "UnitActor.generated.h"

UCLASS()
class P3_FLAGSHIP_API AUnitActor : public AActor
{
	GENERATED_BODY()

public:
	AUnitActor();

	UPROPERTY(EditAnywhere, Category = "Unit|Movement")
	float MoveSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Identity")
	int32 UnitId = -1;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Identity")
	int32 SquadId = -1;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Identity")
	ETacticsTeam Team = ETacticsTeam::Team0;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Stats")
	int32 MovePointsMax = 6;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Stats")
	int32 MovePointsRemaining = 6;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Stats")
	FTacticsWeaponDef Weapon;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Stats")
	FName UtilityId = "None";

	void InitializeFromGameState(
		int32 InUnitId,
		int32 InSquadId,
		ETacticsTeam InTeam,
		const FTacticsUnitDef& Def,
		const TacticsCore::TilePos& InSpawnTile
	);

	void RefreshForNewTurn();

	TacticsCore::TilePos GetTile() const { return Tile; }
	bool TryStartMovePath(const TArray<TacticsCore::TilePos>& InPath);

	void SetSelected(bool bSelected);
	bool IsSelected() const { return bIsSelected; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh = nullptr;

	TacticsCore::TilePos Tile;
	TacticsCore::TilePos SpawnTile;

	// Movement
	bool bMoving = false;
	TArray<FVector> Waypoints;
	int32 WaypointIndex = 0;

	bool bIsSelected = false;

	void RefreshWorldLocationFromTile();
	void ApplySelectionVisuals();
};
