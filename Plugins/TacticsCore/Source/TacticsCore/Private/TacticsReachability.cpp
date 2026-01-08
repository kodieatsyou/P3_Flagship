


#include "TacticsReachability.h"
#include <queue>


namespace TacticsCore {

	ReachResult ComputeReachableTiles(
		const GridDesc& Grid,
		const TilePos& Start,
		int32 MaxCost,
		IsBlockedFn IsBlocked,
		void* UserData
	) noexcept
	{
		ReachResult Out;

		if (MaxCost < 0) {
			return Out;
		}

		if (!Grid.InBounds(Start)) {
			return Out;
		}

		if (IsBlocked && IsBlocked(Start, UserData)) {
			return Out;
		}

		const int32 N = Grid.Width * Grid.Height;
		TArray<int32> Dist;
		Dist.Init(INT32_MAX, N);

		auto PushReach = [&](const TilePos& T, int32 C) {
			Out.Reachable.Add(T);
			Out.Cost.Add(C);
		};

		std::queue<TilePos> Q;

		Dist[Grid.ToIndex(Start)] = 0;
		Q.push(Start);
		PushReach(Start, 0);

		const TilePos Dirs[4] = { 
			TilePos(1,0), 
			TilePos(-1,0), 
			TilePos(0,1), 
			TilePos(0,-1) 
		};

		while (!Q.empty()) {
			const TilePos Cur = Q.front();
			Q.pop();

			const int32 CurIdx = Grid.ToIndex(Cur);
			const int32 CurDist = Dist[CurIdx];

			if (CurDist >= MaxCost) {
				continue;
			}

			for (const TilePos& D : Dirs) {
				const TilePos Next(Cur.X + D.X, Cur.Y + D.Y);

				if (!Grid.InBounds(Next)) {
					continue;
				}

				if (IsBlocked && IsBlocked(Next, UserData)) {
					continue;
				}

				const int32 NextIdx = Grid.ToIndex(Next);
				const int32 NextDist = CurDist + 1;

				if (NextDist < Dist[NextIdx] && NextDist <= MaxCost) {
					Dist[NextIdx] = NextDist;
					Q.push(Next);
					PushReach(Next, NextDist);
				}
			}
		}
		return Out;
	}

}