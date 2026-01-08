#pragma once

#include "TacticsCoreTypes.h"
#include "Containers/Map.h"
#include "Containers/Array.h"

struct FMoveTask
{
    TArray<TacticsCore::TilePos> Path;
    int32 PathIndex = 0;
    float Progress = 0.0f;
};

class FMovementSystem
{
public:
    void SetTileMoveDuration(float SecondsPerTile) { TileMoveDuration = FMath::Max(SecondsPerTile, 0.1f); }

    bool IsMoving(uint32 EntityId) const { return ActiveMoves.Contains(EntityId); }

    bool StartMove(uint32 EntityId, const TArray<TacticsCore::TilePos>& Path);
    void CancelMove(uint32 EntityId);

    // Callbacks keep this system independent of your unit storage layout
    struct FCallbacks
    {
        TacticsCore::TilePos(*GetUnitTile)(uint32 EntityId, void* User) = nullptr;
        void (*SetUnitTile)(uint32 EntityId, const TacticsCore::TilePos& Tile, void* User) = nullptr;

        void (*OnStepCommitted)(uint32 EntityId, const TacticsCore::TilePos& From, const TacticsCore::TilePos& To, void* User) = nullptr;
        void (*OnVisualUpdate)(uint32 EntityId, const TacticsCore::TilePos& From, const TacticsCore::TilePos& To, float Alpha, void* User) = nullptr;
        void (*OnMoveFinished)(uint32 EntityId, void* User) = nullptr;

        void* User = nullptr;
    };

    void Tick(float DeltaSeconds, const FCallbacks& Cb);

private:
    TMap<uint32, FMoveTask> ActiveMoves;
    float TileMoveDuration = 0.25f;
};
