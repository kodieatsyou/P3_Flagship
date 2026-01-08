// GridWorldSubsystem.cpp

#include "GridWorldSubsystem.h"
#include "DrawDebugHelpers.h"


void UGridWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    grid.width = 20;
    grid.height = 20;
    grid.tileSize = 100.0f;

    origin = FVector::ZeroVector;
    bDebugDraw = true;
}

void UGridWorldSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

TacticsCore::TilePos UGridWorldSubsystem::WorldToTile(const FVector& world) const
{
    const FVector local = world - origin;
    const float inv = 1.0f / grid.tileSize;

    const int32 x = FMath::FloorToInt(local.X * inv);
    const int32 y = FMath::FloorToInt(local.Y * inv);

    return TacticsCore::TilePos{ x, y };
}

FVector UGridWorldSubsystem::TileToWorldCenter(const TacticsCore::TilePos& tile) const
{
    const float half = grid.tileSize * 0.5f;
    return origin + FVector(tile.x * grid.tileSize + half, tile.y * grid.tileSize + half, 0.0f);
}

bool UGridWorldSubsystem::InBounds(const TacticsCore::TilePos& tile) const
{
    return TacticsCore::InBounds(grid, tile);
}

void UGridWorldSubsystem::DebugDrawGrid(UWorld* world) const
{
    if (!world || !bDebugDraw || !grid.IsValid())
        return;

    const float z = origin.Z;
    const float w = grid.width * grid.tileSize;
    const float h = grid.height * grid.tileSize;

    // Vertical lines
    for (int32 x = 0; x <= grid.width; ++x)
    {
        const float wx = origin.X + x * grid.tileSize;
        const FVector a(wx, origin.Y, z);
        const FVector b(wx, origin.Y + h, z);
        DrawDebugLine(world, a, b, FColor::Green, false, 0.0f, 0, 2.0f);
    }

    // Horizontal lines
    for (int32 y = 0; y <= grid.height; ++y)
    {
        const float wy = origin.Y + y * grid.tileSize;
        const FVector a(origin.X, wy, z);
        const FVector b(origin.X + w, wy, z);
        DrawDebugLine(world, a, b, FColor::Green, false, 0.0f, 0, 2.0f);
    }
}


void UGridWorldSubsystem::DebugFillTile(UWorld* World, const TacticsCore::TilePos& Tile, const FColor& Color, float ZOffset) const
{
    if (!World || !grid.IsValid())
        return;

    const FVector Center = TileToWorldCenter(Tile) + FVector(0, 0, ZOffset);
    const FVector Extent(grid.tileSize * 0.5f, grid.tileSize * 0.5f, 1.0f);

    DrawDebugBox(World, Center, Extent, Color, false, 0.0f, 0, 0.0f);
}

void UGridWorldSubsystem::DebugMarkTile(UWorld* World, const TacticsCore::TilePos& Tile, const FColor& Color, float ZOffset) const
{
    if (!World || !grid.IsValid())
        return;

    const FVector Center = TileToWorldCenter(Tile) + FVector(0, 0, ZOffset);

    const float Half = grid.tileSize * 0.25f;

    const FVector A = Center + FVector(-Half, 0, 0);
    const FVector B = Center + FVector(Half, 0, 0);
    const FVector C = Center + FVector(0, -Half, 0);
    const FVector D = Center + FVector(0, Half, 0);

    DrawDebugLine(World, A, B, Color, false, 0.0f, 0, 6.0f);
    DrawDebugLine(World, C, D, Color, false, 0.0f, 0, 6.0f);

    DrawDebugBox(World, Center + FVector(0, 0, 10.0f), FVector(6, 6, 10), Color, false, 0.0f, 0, 2.0f);
}
