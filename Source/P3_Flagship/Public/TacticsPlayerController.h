#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TacticsCoreTypes.h"
#include "TacticsPlayerController.generated.h"

class AUnitActor;

UCLASS()
class P3_FLAGSHIP_API ATacticsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATacticsPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY()
	AUnitActor* SelectedUnit = nullptr;

	TSet<int32> ReachableSet;

	void OnLeftClick();
	void OnRightClick();
	void OnEndTurnTest();

	bool GetHoveredTile(TacticsCore::TilePos& OutTile, FVector& OutHitWorld) const;
	AUnitActor* GetHoveredUnit() const;

	void SelectUnit(AUnitActor* Unit);
	void RecomputeAndDrawReachOverlay();
	bool IsTileReachable(const TacticsCore::TilePos& Tile) const;

	void RecomputeAndDrawAttackRange();
	bool IsTileInAttackRange(const TacticsCore::TilePos& Tile) const;
	TSet<int32> AttackRangeSet;
};
