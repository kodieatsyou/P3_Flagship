#pragma once

#include <cstdint>
#include <functional>

namespace TacticsCore {

    struct TilePos {
        int32_t x = 0;
        int32_t y = 0;

        constexpr bool operator ==(const TilePos& other) const noexcept {
            return x == other.x && y == other.y;
        }

        constexpr bool operator !=(const TilePos& other) const noexcept {
            return !(*this == other);
        }
    };

    struct TilePosHash {
        size_t operator()(const TilePos& p) const noexcept{
            const uint64_t a = static_cast<uint32_t>(p.x);
            const uint64_t b = static_cast<uint32_t>(p.y);
            return static_cast<size_t>((a << 32) ^ b);
        }
    };
}