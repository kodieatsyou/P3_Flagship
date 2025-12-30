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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void SnapToTile();
	TacticsCore::TilePos tile { 0, 0 };
};
