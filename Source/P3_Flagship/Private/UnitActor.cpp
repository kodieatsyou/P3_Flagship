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

void AUnitActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bMoving)
		return;

	if (Path.Num() < 2 || !Path.IsValidIndex(PathIndex) || !Path.IsValidIndex(PathIndex + 1))
	{
		bMoving = false;
		Path.Reset();
		return;
	}

	const float Denom = FMath::Max(0.01f, SecondsPerTile);
	SegmentT += DeltaTime / Denom;

	const float T = FMath::Clamp(SegmentT, 0.0f, 1.0f);
	SetActorLocation(FMath::Lerp(SegmentFrom, SegmentTo, T));

	if (SegmentT >= 1.0f)
	{
		tile = Path[PathIndex + 1];
		SetActorLocation(SegmentTo);

		OnStepped.Broadcast(this, Path[PathIndex + 1]);

		PathIndex++;

		if (!Path.IsValidIndex(PathIndex + 1))
		{
			// Done
			bMoving = false;
			Path.Reset();
			SegmentT = 0.0f;
			OnMoveFinished.Broadcast(this);
			return;
		}

		StartNextSegment();
	}
}

void AUnitActor::SetTile(const TacticsCore::TilePos& newTile)
{
	bMoving = false;
	Path.Reset();
	PathIndex = 0;
	SegmentT = 0.0f;

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

void AUnitActor::MoveAlongPath(const TArray<TacticsCore::TilePos>& InPath, float InSecondsPerTile)
{
	SecondsPerTile = FMath::Max(0.01f, InSecondsPerTile);

	Path = InPath;
	if (Path.Num() == 0)
		return;

	if (Path[0] != tile)
	{
		Path.Insert(tile, 0);
	}

	if (Path.Num() < 2)
	{
		return;
	}

	bMoving = true;
	PathIndex = 0;
	SegmentT = 0.0f;

	SnapToTile();
	StartNextSegment();
}

void AUnitActor::StartNextSegment()
{
	SegmentT = 0.0f;
	SegmentFrom = TileToWorldCenter(Path[PathIndex]);
	SegmentTo = TileToWorldCenter(Path[PathIndex + 1]);
}

FVector AUnitActor::TileToWorldCenter(const TacticsCore::TilePos& T) const
{
	if (const UWorld* W = GetWorld())
	{
		if (const UGridWorldSubsystem* Grid = W->GetSubsystem<UGridWorldSubsystem>())
		{
			return Grid->TileToWorldCenter(T);
		}
	}
	return GetActorLocation();
}
