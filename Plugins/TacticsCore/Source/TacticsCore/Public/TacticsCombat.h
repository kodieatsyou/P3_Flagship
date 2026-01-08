

#pragma once

#include "CoreMinimal.h"
#include "TacticsCoreTypes.h"

namespace TacticsCore
{

	TACTICSCORE_API inline int32 ManhattanDistance(const TilePos& A, const TilePos& B)
	{
		return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y);
	}

	TACTICSCORE_API inline bool HasLineOfSight(
		const GridDesc& Grid,
		const TilePos& From,
		const TilePos& To,
		IsBlockedFn IsBlocked,
		void* UserData
	)
	{
		return true;
	}
}