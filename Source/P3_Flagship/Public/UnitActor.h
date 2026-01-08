#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TacticsCoreTypes.h"
#include "UnitActor.generated.h"

UCLASS()
class P3_FLAGSHIP_API AUnitActor : public AActor
{
	GENERATED_BODY()

public:
	AUnitActor();

	UPROPERTY(EditAnywhere, Category = "Unit")
	int32 InitialTileX = 0;

	UPROPERTY(EditAnywhere, Category = "Unit")
	int32 InitialTileY = 0;

	UPROPERTY(EditAnywhere, Category = "Unit|Movement")
	float MoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Unit|Stats")
	int32 MovePointsMax = 6;

	UPROPERTY(VisibleAnywhere, Category = "Unit|Stats")
	int32 MovePointsRemaining = 6;

	void RefreshForNewTurn();

	TacticsCore::TilePos GetTile() const { return Tile; }
	void SetTileImmediate(const TacticsCore::TilePos& NewTile);

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

	bool bMoving = false;
	TArray<FVector> Waypoints;
	int32 WaypointIndex = 0;

	bool bIsSelected = false;

	void RefreshWorldLocationFromTile();
	void ApplySelectionVisuals();
};
