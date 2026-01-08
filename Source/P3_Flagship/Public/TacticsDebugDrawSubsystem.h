
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TacticsCoreTypes.h"
#include "TacticsDebugDrawSubsystem.generated.h"

class UGridWorldSubsystem;

UCLASS()
class P3_FLAGSHIP_API UTacticsDebugDrawSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	// -- Settings --
	UPROPERTY(EditAnywhere, Category = "DebugDraw")
	bool bEnableDebugDrawing = true;

	UPROPERTY(EditAnywhere, Category = "DebugDraw|Grid")
	bool bDrawGrid = true;

	UPROPERTY(EditAnywhere, Category = "DebugDraw|Reach")
	bool bDrawReach = true;

	UPROPERTY(EditAnywhere, Category = "DebugDraw|Z")
	float GridZOffset = 2.0f;

	UPROPERTY(EditAnywhere, Category = "DebugDraw|Z")
	float ReachZOffset = 10.0f;

	UPROPERTY(EditAnywhere, Category = "DebugDraw|Z")
	float SelectedZOffset = 20.0f;

	//Line Styles
	UPROPERTY(EditAnywhere, Category = "DebugDraw|Style")
	float GridLineThickness = 2.0f;

	UPROPERTY(EditAnywhere, Category = "DebugDraw|Style")
	float ReachBoxThickness = 2.0f;

	UPROPERTY(EditAnywhere, Category = "DebugDraw|Style")
	float SelectedBoxThickness = 2.0f;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	//Grid
	void RedrawGrid();

	//Reach
	void ClearReachOverlay();
	void SetReachOverlay(const TArray<TacticsCore::TilePos>& Tiles);

	//Selected Tile
	void ClearSelectedTile();
	void SetSelectedTile(const TacticsCore::TilePos& Tile);

	// --- Attack Range ---
	void ClearAttackRangeOverlay();
	void SetAttackRangeOverlay(const TArray<TacticsCore::TilePos>& Tiles);

private:
	TArray<TacticsCore::TilePos> CachedReachTiles;
	bool bHasSelectedTile = false;
	TacticsCore::TilePos CachedSelectedTile;
	TArray<TacticsCore::TilePos> CachedAttackRangeTiles;

	void FlushAndRedrawAll();

	//Internal drawing helpers
	void DrawGridInternal(const UGridWorldSubsystem* Grid, UWorld* World) const;
	void DrawReachInternal(const UGridWorldSubsystem* Grid, UWorld* World) const;
	void DrawSelectedInternal(const UGridWorldSubsystem* Grid, UWorld* World) const;
	void DrawAttackRangeInternal(const UGridWorldSubsystem* Grid, UWorld* World) const;
};
