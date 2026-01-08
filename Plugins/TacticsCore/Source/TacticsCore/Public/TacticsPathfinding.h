#pragma once

#include "CoreMinimal.h"
#include "TacticsCoreTypes.h"
#include "TacticsGrid.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cstdint>

namespace TacticsCore
{
    struct TACTICSCORE_API PathResult
    {
        bool success = false;
        std::vector<TilePos> path;
    };

    using IsBlockedFn = bool(*)(const TilePos& tile, void* user);

    TACTICSCORE_API PathResult FindPathAStar(
        const GridDesc& grid,
        const TilePos& start,
        const TilePos& goal,
        IsBlockedFn isBlocked,
        void* userData
    ) noexcept;
}
