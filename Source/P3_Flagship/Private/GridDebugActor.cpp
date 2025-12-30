// Fill out your copyright notice in the Description page of Project Settings.


#include "GridDebugActor.h"

#include "GridWorldSubsystem.h"
#include "Engine/World.h"

// Sets default values
AGridDebugActor::AGridDebugActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorHiddenInGame(true);

}

// Called when the game starts or when spawned
void AGridDebugActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridDebugActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* world = GetWorld();
	if(!world) {
		return;
	}

	UGridWorldSubsystem* grid = world->GetSubsystem<UGridWorldSubsystem>();

	if(!grid) {
		return;
	}

	grid->DebugDrawGrid(world);
}

