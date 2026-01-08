#pragma once

#include "CoreMinimal.h"
#include "TacticsCoreTypes.h"

namespace TacticsCore
{
	// 4-directional A* on a rectangular grid.
	// - isBlocked: returns true if tile is not walkable.
	// - moveCost: optional; if null, uses 1.0 per step.
	TACTICSCORE_API PathResult FindPathAStar(
		const GridDesc& Grid,
		const TilePos& Start,
		const TilePos& Goal,
		IsBlockedFn IsBlocked,
		void* UserData,
		MoveCostFn MoveCost = nullptr
	) noexcept;
}