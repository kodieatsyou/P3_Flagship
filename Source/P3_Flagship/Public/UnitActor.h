// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TacticsCoreTypes.h"
#include "UnitActor.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUnitActorStepped, AUnitActor*, const TacticsCore::TilePos&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitActorMoveFinished, AUnitActor*);

UCLASS()
class P3_FLAGSHIP_API AUnitActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitActor();

	void SetTile(const TacticsCore::TilePos& newTile);
	const TacticsCore::TilePos& GetTile() const { return tile; }

	void MoveAlongPath(const TArray<TacticsCore::TilePos>& InPath, float InSecondsPerTile = 0.12f);

	FOnUnitActorStepped OnStepped;
	FOnUnitActorMoveFinished OnMoveFinished;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void SnapToTile();

	void StartNextSegment();
	FVector TileToWorldCenter(const TacticsCore::TilePos& T) const;

	UPROPERTY(EditAnywhere, Category = "Tactics|Grid", meta = (ClampMin = "0"))
	int32 InitialTileX = 0;

	UPROPERTY(EditAnywhere, Category = "Tactics|Grid", meta = (ClampMin = "0"))
	int32 InitialTileY = 0;

	TacticsCore::TilePos tile { 0, 0 };


	bool bMoving = false;
	float SecondsPerTile = 0.12f;

	TArray<TacticsCore::TilePos> Path;
	int32 PathIndex = 0;
	float SegmentT = 0.0f;

	FVector SegmentFrom = FVector::ZeroVector;
	FVector SegmentTo = FVector::ZeroVector;
};
