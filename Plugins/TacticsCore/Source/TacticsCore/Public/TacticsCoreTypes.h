#pragma once

#include "CoreMinimal.h"

namespace TacticsCore {

    struct TilePos {
        int32_t X = 0;
        int32_t Y = 0;

        constexpr TilePos() = default;
        constexpr TilePos(int32 InX, int32 InY) : X(InX), Y(InY) {}

        FORCEINLINE bool operator==(const TilePos& Other) const { 
            return X == Other.X && Y == Other.Y; 
        }
        FORCEINLINE bool operator!=(const TilePos& Other) const { 
            return !(*this == Other); 
        }
    };

    struct GridDesc
    {
        int32 Width = 0;
        int32 Height = 0;

        constexpr GridDesc() = default;
        constexpr GridDesc(int32 InW, int32 InH) : Width(InW), Height(InH) {}

        FORCEINLINE bool InBounds(const TilePos& T) const
        {
            return T.X >= 0 && T.Y >= 0 && T.X < Width && T.Y < Height;
        }

        FORCEINLINE int32 ToIndex(const TilePos& T) const
        {
            return T.Y * Width + T.X;
        }
    };

    struct PathResult
    {
        bool bSuccess = false;
        TArray<TilePos> Path;
    };

    using IsBlockedFn = bool(*)(const TilePos& Tile, void* UserData);
	using MoveCostFn = float(*)(const TilePos& From, const TilePos& To, void* UserData);
}