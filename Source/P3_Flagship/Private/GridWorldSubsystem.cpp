// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldSubsystem.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"

void UGridWorldSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    Super::Initialize(Collection);

    grid.width = 12;
    grid.height = 12;
    grid.tileSize = 100.0f;

    origin = FVector(0.f, 0.f, 0.f);
    bDebugDraw = true;
}

void UGridWorldSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

bool UGridWorldSubsystem::InBounds(const TacticsCore::TilePos& tile) const {
    return TacticsCore::InBounds(grid, tile);
}

TacticsCore::TilePos UGridWorldSubsystem::WorldToTile(const FVector& world) const {
    const FVector local = world - origin;
    const int32 x = static_cast<int32>(FMath::FloorToInt(local.X / grid.tileSize));
    const int32 y = static_cast<int32>(FMath::FloorToInt(local.Y / grid.tileSize));

    return TacticsCore::TilePos{x, y};
}

FVector UGridWorldSubsystem::TileToWorldCenter(const TacticsCore::TilePos& tile) const {
    const float x = origin.X + (static_cast<float>(tile.x) + 0.5f) * grid.tileSize;
    const float y = origin.Y + (static_cast<float>(tile.y) + 0.5f) * grid.tileSize;

    return FVector(x, y, origin.Z);
}

void UGridWorldSubsystem::DebugDrawGrid(UWorld* world) const {
    if(!world || !bDebugDraw || !grid.IsValid()) {
        return;
    }

    const float z = origin.Z;
    const float w = static_cast<float>(grid.width) * grid.tileSize;
    const float h = static_cast<float>(grid.height) * grid.tileSize;

    for (int32 x = 0; x <= grid.width; ++x)
    {
        const float xWorld = origin.X + static_cast<float>(x) * grid.tileSize;
        const FVector A(xWorld, origin.Y, z);
        const FVector B(xWorld, origin.Y + h, z);
        DrawDebugLine(world, A, B, FColor::Green, false, 0.f, 0, 1.f);
    }

    for (int32 y = 0; y <= grid.height; ++y)
    {
        const float yWorld = origin.Y + static_cast<float>(y) * grid.tileSize;
        const FVector A(origin.X, yWorld, z);
        const FVector B(origin.X + w, yWorld, z);
        DrawDebugLine(world, A, B, FColor::Green, false, 0.f, 0, 1.f);
    }
}