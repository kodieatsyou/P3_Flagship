#pragma once

#include "TacticsCoreTypes.h"
#include <cstdint>

namespace TacticsCore {

    struct GridDesc{
        int32_t width = 10;
        int32_t height = 10;
        float tileSize = 100.0f;

        constexpr bool IsValid() const noexcept {
            return width > 0 && height > 0 && tileSize > 0.0f;
        }
    };

    constexpr int32_t ToIndex(const GridDesc& grid, const TilePos& p) noexcept {
        return p.x + (p.y * grid.width);
    }

    constexpr bool InBounds(const GridDesc& grid, const TilePos& p) noexcept {
        return (p.x >= 0 && p.x >= 0 && p.x < grid.width && p.y < grid.height);
    }

}