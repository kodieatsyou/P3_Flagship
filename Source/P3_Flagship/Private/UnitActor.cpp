// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActor.h"

#include "GridWorldSubsystem.h"
#include "Engine/World.h"

// Sets default values
AUnitActor::AUnitActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUnitActor::BeginPlay()
{
	Super::BeginPlay();
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
	UWorld* world = GetWorld();
	if(!world) {
		return;
	}

	UGridWorldSubsystem* grid = world->GetSubsystem<UGridWorldSubsystem>();
	if(!grid) {
		return;
	}

	const FVector worldPos = grid->TileToWorldCenter(tile);
	SetActorLocation(worldPos);
}

