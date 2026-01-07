// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActor.h"

#include "GridWorldSubsystem.h"
#include "Engine/World.h"

// Sets default values
AUnitActor::AUnitActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AUnitActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	tile = TacticsCore::TilePos{ InitialTileX, InitialTileY };
	SnapToTile();
}

void AUnitActor::BeginPlay()
{
	Super::BeginPlay();
	tile = TacticsCore::TilePos{ InitialTileX, InitialTileY };
	SnapToTile();
}

// Called every frame
void AUnitActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnitActor::SetTile(const TacticsCore::TilePos& newTile) {
	tile = newTile;
	SnapToTile();
}

void AUnitActor::SnapToTile() {
	if (UWorld* W = GetWorld()) {
		if (UGridWorldSubsystem* Grid = W->GetSubsystem<UGridWorldSubsystem>()) {
			SetActorLocation(Grid->TileToWorldCenter(tile));
		}
	}
}

