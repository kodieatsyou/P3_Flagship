// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TacticsCoreTypes.h"
#include "UnitActor.generated.h"

UCLASS()
class P3_FLAGSHIP_API AUnitActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitActor();

	void SetTile(const TacticsCore::TilePos& newTile);
	const TacticsCore::TilePos& GetTile() const { return tile; }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void SnapToTile();

	UPROPERTY(EditAnywhere, Category = "Tactics|Grid", meta = (ClampMin = "0"))
	int32 InitialTileX = 0;

	UPROPERTY(EditAnywhere, Category = "Tactics|Grid", meta = (ClampMin = "0"))
	int32 InitialTileY = 0;

	TacticsCore::TilePos tile { 0, 0 };
};
