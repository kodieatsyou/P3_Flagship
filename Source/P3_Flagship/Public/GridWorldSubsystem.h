#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TacticsCoreTypes.h"
#include "GridWorldSubsystem.generated.h"

UCLASS()
class P3_FLAGSHIP_API UGridWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// --- Authoring parameters ---
	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 GridWidth = 12;

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 GridHeight = 12;

	UPROPERTY(EditAnywhere, Category = "Grid")
	float TileSize = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Grid")
	FVector GridOrigin = FVector(0, 0, 0);

	// --- API ---
	TacticsCore::GridDesc GetGridDesc() const;

	bool IsInBounds(const TacticsCore::TilePos& Tile) const;
	bool IsBlockedTile(const TacticsCore::TilePos& Tile) const;

	void SetBlockedTile(const TacticsCore::TilePos& Tile, bool bBlocked);

	FVector TileToWorldCenter(const TacticsCore::TilePos& Tile) const;
	bool WorldToTile(const FVector& World, TacticsCore::TilePos& OutTile) const;

	static bool IsBlockedFn(const TacticsCore::TilePos& Tile, void* UserData);

private:
	TSet<int32> BlockedIndices;
};
