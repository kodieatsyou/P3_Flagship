#include "TacticsDebugDrawSubsystem.h"

#include "GridWorldSubsystem.h"
#include "DrawDebugHelpers.h"
#include "TacticsWorldSettingsUtil.h"
#include "TacticsWorldSettings.h"
#include "Engine/World.h"

void UTacticsDebugDrawSubsystem::OnWorldBeginPlay(UWorld& InWorld) {
	Super::OnWorldBeginPlay(InWorld);

	if (const ATacticsWorldSettings* WS = TacticsWorldSettingsUtil::Get(&InWorld))
	{
		bEnableDebugDrawing = WS->bEnableDebugDraw;
		bDrawGrid = WS->bDrawGrid;
		bDrawReach = WS->bDrawReach;
	}

	if (!bEnableDebugDrawing) {
		return;
	}

	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::RedrawGrid() {
	if (!bEnableDebugDrawing) {
		return;
	}
	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::ClearReachOverlay()
{
	CachedReachTiles.Reset();
	if (!bEnableDebugDrawing) {
		return;
	}
	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::SetReachOverlay(const TArray<TacticsCore::TilePos>& Tiles)
{
	CachedReachTiles = Tiles;
	if (!bEnableDebugDrawing) {
		return;
	}
	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::ClearSelectedTile()
{
	bHasSelectedTile = false;
	if (!bEnableDebugDrawing) {
		return;
	}
	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::SetSelectedTile(const TacticsCore::TilePos& Tile)
{
	CachedSelectedTile = Tile;
	bHasSelectedTile = true;
	if (!bEnableDebugDrawing) {
		return;
	}
	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::FlushAndRedrawAll()
{
	UWorld* World = GetWorld();
	if (!World) {
		return;
	}

	FlushPersistentDebugLines(World);

	const UGridWorldSubsystem* Grid = World->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) {
		return;
	}

	if (bDrawGrid) {
		DrawGridInternal(Grid, World);
	}

	if (bDrawReach && CachedReachTiles.Num() > 0) {
		DrawReachInternal(Grid, World);
	}

	if (bHasSelectedTile) {
		DrawSelectedInternal(Grid, World);
	}

	if (CachedAttackRangeTiles.Num() > 0) {
		DrawAttackRangeInternal(Grid, World);
	}
}

void UTacticsDebugDrawSubsystem::DrawGridInternal(const UGridWorldSubsystem* Grid, UWorld* World) const
{
	const FVector Origin = Grid->GridOrigin + FVector(0, 0, GridZOffset);
	const float TileSize = Grid->TileSize;
	const int32 W = Grid->GridWidth;
	const int32 H = Grid->GridHeight;

	for (int32 x = 0; x <= W; ++x)
	{
		const FVector A = Origin + FVector(x * TileSize, 0.0f, 0.0f);
		const FVector B = Origin + FVector(x * TileSize, H * TileSize, 0.0f);

		DrawDebugLine(World, A, B, FColor::Cyan, true, 0.0f, 0, GridLineThickness);
	}

	for (int32 y = 0; y <= H; ++y)
	{
		const FVector A = Origin + FVector(0.0f, y * TileSize, 0.0f);
		const FVector B = Origin + FVector(W * TileSize, y * TileSize, 0.0f);

		DrawDebugLine(World, A, B, FColor::Cyan, true, 0.0f, 0, GridLineThickness);
	}
}

void UTacticsDebugDrawSubsystem::DrawReachInternal(const UGridWorldSubsystem* Grid, UWorld* World) const
{
	const float Half = Grid->TileSize * 0.45f;

	for (const TacticsCore::TilePos& T : CachedReachTiles)
	{
		const FVector C = Grid->TileToWorldCenter(T) + FVector(0, 0, ReachZOffset);
		DrawDebugBox(World, C, FVector(Half, Half, 10.0f), FColor::Green, true, 0.0f, 0, ReachBoxThickness);
	}
}

void UTacticsDebugDrawSubsystem::DrawSelectedInternal(const UGridWorldSubsystem* Grid, UWorld* World) const
{
	const float Half = Grid->TileSize * 0.48f;
	const FVector C = Grid->TileToWorldCenter(CachedSelectedTile) + FVector(0, 0, SelectedZOffset);

	DrawDebugBox(World, C, FVector(Half, Half, 20.0f), FColor::Yellow, true, 0.0f, 0, SelectedBoxThickness);
}

void UTacticsDebugDrawSubsystem::ClearAttackRangeOverlay()
{
	CachedAttackRangeTiles.Reset();
	if (!bEnableDebugDrawing) {
		return;
	}
	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::SetAttackRangeOverlay(const TArray<TacticsCore::TilePos>& Tiles)
{
	CachedAttackRangeTiles = Tiles;
	if (!bEnableDebugDrawing) {
		return;
	}
	FlushAndRedrawAll();
}

void UTacticsDebugDrawSubsystem::DrawAttackRangeInternal(const UGridWorldSubsystem* Grid, UWorld* World) const
{
	const float Half = Grid->TileSize * 0.45f;

	for (const TacticsCore::TilePos& T : CachedAttackRangeTiles)
	{
		const FVector C = Grid->TileToWorldCenter(T) + FVector(0, 0, ReachZOffset + 15.0f);
		DrawDebugBox(World, C, FVector(Half, Half, 8.0f), FColor::Red, true, 0.0f, 0, 2.0f);
	}
}