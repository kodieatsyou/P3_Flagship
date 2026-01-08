


#include "TacticsPathfinding.h"
#include <algorithm>
#include <cmath>

namespace TacticsCore
{

	static inline int32_t HeuristicManhattan(const TilePos& a, const TilePos& b) {
		const int32_t dx = (a.x > b.x) ? (a.x - b.x) : (b.x - a.x);
		const int32_t dy = (a.y > b.y) ? (a.y - b.y) : (b.y - a.y);
		return dx + dy;
	}

    static inline void GetNeighbors4(const TilePos& p, TilePos out[4]) noexcept
    {
        out[0] = TilePos{ p.x + 1, p.y };
        out[1] = TilePos{ p.x - 1, p.y };
        out[2] = TilePos{ p.x, p.y + 1 };
        out[3] = TilePos{ p.x, p.y - 1 };
    }

    struct Node
    {
        TilePos pos{};
        int32_t f = 0;
        int32_t g = 0;
    };

    struct NodeGreater
    {
        bool operator()(const Node& a, const Node& b) const noexcept
        {
            if (a.f != b.f) return a.f > b.f;
            return a.g > b.g;
        }
    };

    PathResult FindPathAStar(
        const GridDesc& grid,
        const TilePos& start,
        const TilePos& goal,
        IsBlockedFn isBlocked,
        void* userData
    ) noexcept
    {
        PathResult result;

        if (!grid.IsValid()) {
            return result;
        }

        if (!InBounds(grid, start) || !InBounds(grid, goal)) {
            return result;
        }

        if (start == goal) {
            result.success = true;
            result.path.push_back(start);
            return result;
        }

        std::priority_queue<Node, std::vector<Node>, NodeGreater> open;
        std::unordered_map<TilePos, int32_t, TilePosHash> gScore;
        std::unordered_map<TilePos, TilePos, TilePosHash> cameFrom;
        std::unordered_set<TilePos, TilePosHash> closed;

        gScore[start] = 0;
        open.push(Node{ start, HeuristicManhattan(start, goal), 0 });

        while (!open.empty()) {
            Node current = open.top();
            open.pop();

            if (closed.find(current.pos) != closed.end()) {
                continue;
            }

            if (current.pos == goal) {
                std::vector<TilePos> rev;
                TilePos p = goal;
                rev.push_back(p);

                while (p != start) {
                    auto it = cameFrom.find(p);
                    if (it == cameFrom.end()) {
                        break;
                    }

                    p = it->second;
                    rev.push_back(p);
                }

                std::reverse(rev.begin(), rev.end());
                result.success = true;
                result.path = std::move(rev);
                return result;
            }

            closed.insert(current.pos);

            TilePos neigh[4];
            GetNeighbors4(current.pos, neigh);

            for (int i = 0; i < 4; i++) {
                const TilePos n = neigh[i];

                if (!InBounds(grid, n)) {
                    continue;
                }

                if (closed.find(n) != closed.end()) {
                    continue;
                }

                if (isBlocked && isBlocked(n, userData)) {
                    continue;
                }

                const int32_t tentativeG = current.g + 1;

                auto itG = gScore.find(n);
                if (itG == gScore.end() || tentativeG < itG->second) {
                    cameFrom[n] = current.pos;
                    gScore[n] = tentativeG;

                    const int32_t f = tentativeG + HeuristicManhattan(n, goal);
                    open.push(Node{ n, f, tentativeG });
                }
            }
        }

        return result;
    }

}