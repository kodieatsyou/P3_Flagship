#include "GridWorldSubsystem.h"
#include "DrawDebugHelpers.h"

TacticsCore::GridDesc UGridWorldSubsystem::GetGridDesc() const
{
	return TacticsCore::GridDesc(GridWidth, GridHeight);
}

bool UGridWorldSubsystem::IsInBounds(const TacticsCore::TilePos& Tile) const
{
	return GetGridDesc().InBounds(Tile);
}

bool UGridWorldSubsystem::IsBlockedTile(const TacticsCore::TilePos& Tile) const
{
	if (!IsInBounds(Tile)) {
		return true;
	}

	const int32 Idx = GetGridDesc().ToIndex(Tile);
	return BlockedIndices.Contains(Idx);
}

void UGridWorldSubsystem::SetBlockedTile(const TacticsCore::TilePos& Tile, bool bBlocked)
{
	if (!IsInBounds(Tile)) {
		return;
	}

	const int32 Idx = GetGridDesc().ToIndex(Tile);
	if (bBlocked) {
		BlockedIndices.Add(Idx);
	}
	else {
		BlockedIndices.Remove(Idx);
	}
}

FVector UGridWorldSubsystem::TileToWorldCenter(const TacticsCore::TilePos& Tile) const
{
	// X -> world X, Y -> world Y
	return GridOrigin + FVector((Tile.X + 0.5f) * TileSize, (Tile.Y + 0.5f) * TileSize, 0.0f);
}

bool UGridWorldSubsystem::WorldToTile(const FVector& World, TacticsCore::TilePos& OutTile) const
{
	const FVector Local = World - GridOrigin;

	const int32 X = FMath::FloorToInt(Local.X / TileSize);
	const int32 Y = FMath::FloorToInt(Local.Y / TileSize);

	OutTile = TacticsCore::TilePos(X, Y);
	return IsInBounds(OutTile);
}

bool UGridWorldSubsystem::IsBlockedFn(const TacticsCore::TilePos& Tile, void* UserData)
{
	const UGridWorldSubsystem* Grid = static_cast<const UGridWorldSubsystem*>(UserData);
	return Grid ? Grid->IsBlockedTile(Tile) : true;
}
