#include "TacticsPathfinding.h"

#include <queue>

namespace TacticsCore
{
	struct Node
	{
		TilePos T;
		float F = 0.0f;
		float G = 0.0f;
	};

	static FORCEINLINE float HeuristicManhattan(const TilePos& A, const TilePos& B)
	{
		return float(FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y));
	}

	struct NodeGreater
	{
		bool operator()(const Node& L, const Node& R) const { 
			return L.F > R.F; 
		}
	};

	static void ReconstructPath(
		const GridDesc& Grid,
		const TilePos& Start,
		const TilePos& Goal,
		const TArray<int32>& CameFrom,
		PathResult& Out
	)
	{
		Out.Path.Reset();

		TilePos Cur = Goal;
		while (!(Cur == Start))
		{
			Out.Path.Add(Cur);
			const int32 CurIdx = Grid.ToIndex(Cur);
			const int32 PrevIdx = CameFrom[CurIdx];
			if (PrevIdx < 0)
			{
				Out.bSuccess = false;
				Out.Path.Reset();
				return;
			}
			Cur = TilePos(PrevIdx % Grid.Width, PrevIdx / Grid.Width);
		}
		Out.Path.Add(Start);
		Algo::Reverse(Out.Path);
	}

	PathResult FindPathAStar(
		const GridDesc& Grid,
		const TilePos& Start,
		const TilePos& Goal,
		IsBlockedFn IsBlocked,
		void* UserData,
		MoveCostFn MoveCost
	) noexcept
	{
		PathResult Result;

		if (!Grid.InBounds(Start) || !Grid.InBounds(Goal)) {
			return Result;
		}

		if (IsBlocked && (IsBlocked(Start, UserData) || IsBlocked(Goal, UserData))) {
			return Result;
		}

		const int32 N = Grid.Width * Grid.Height;

		TArray<float> GScore;
		GScore.Init(TNumericLimits<float>::Max(), N);

		TArray<int32> CameFrom;
		CameFrom.Init(-1, N);

		TArray<uint8> Closed;
		Closed.Init(0, N);

		std::priority_queue<Node, std::vector<Node>, NodeGreater> Open;

		const int32 StartIdx = Grid.ToIndex(Start);
		GScore[StartIdx] = 0.0f;

		Open.push(Node{ Start, HeuristicManhattan(Start, Goal), 0.0f });

		const TilePos Dirs[4] = { TilePos(1,0), TilePos(-1,0), TilePos(0,1), TilePos(0,-1) };

		while (!Open.empty())
		{
			Node Cur = Open.top();
			Open.pop();

			const int32 CurIdx = Grid.ToIndex(Cur.T);
			if (Closed[CurIdx]) {
				continue;
			}

			Closed[CurIdx] = 1;

			if (Cur.T == Goal)
			{
				Result.bSuccess = true;
				ReconstructPath(Grid, Start, Goal, CameFrom, Result);
				return Result;
			}

			for (const TilePos& D : Dirs)
			{
				const TilePos Next(Cur.T.X + D.X, Cur.T.Y + D.Y);
				if (!Grid.InBounds(Next)) {
					continue;
				}

				if (IsBlocked && IsBlocked(Next, UserData)) {
					continue;
				}

				const int32 NextIdx = Grid.ToIndex(Next);
				if (Closed[NextIdx]) {
					continue;
				}

				const float StepCost = MoveCost ? MoveCost(Cur.T, Next, UserData) : 1.0f;
				const float TentativeG = GScore[CurIdx] + StepCost;

				if (TentativeG < GScore[NextIdx])
				{
					GScore[NextIdx] = TentativeG;
					CameFrom[NextIdx] = CurIdx;

					const float H = HeuristicManhattan(Next, Goal);
					const float F = TentativeG + H;
					Open.push(Node{ Next, F, TentativeG });
				}
			}
		}

		return Result;
	}
}
