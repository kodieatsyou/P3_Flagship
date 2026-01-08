

#pragma once

#include "CoreMinimal.h"
#include "TacticsCoreTypes.h"

namespace TacticsCore {

	struct ReachResult {
		TArray<TilePos> Reachable;

		TArray<int32> Cost;
	};

	TACTICSCORE_API ReachResult ComputeReachableTiles(
		const GridDesc& Grid,
		const TilePos& Start,
		int32 MaxCost,
		IsBlockedFn IsBlocked,
		void* USerData
	) noexcept;

}