#include "UnitActor.h"
#include "GridWorldSubsystem.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

AUnitActor::AUnitActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
}

void AUnitActor::BeginPlay()
{
	Super::BeginPlay();
	ApplySelectionVisuals();
}

void AUnitActor::InitializeFromGameState(
	int32 InUnitId,
	int32 InSquadId,
	ETacticsTeam InTeam,
	const FTacticsUnitDef& Def,
	const TacticsCore::TilePos& InSpawnTile
)
{
	UnitId = InUnitId;
	SquadId = InSquadId;
	Team = InTeam;

	MovePointsMax = Def.MovePointsMax;
	MovePointsRemaining = MovePointsMax;

	Weapon = Def.Weapon;
	UtilityId = Def.UtilityId;

	SpawnTile = InSpawnTile;
	Tile = InSpawnTile;

	RefreshWorldLocationFromTile();
}

void AUnitActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Selected marker
	if (bIsSelected)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(0, 0, 60), 30.0f, 12, FColor::Yellow, false, 0.0f, 0, 2.0f);
	}

	if (!bMoving || Waypoints.Num() == 0) {
		return;
	}

	const FVector Cur = GetActorLocation();
	const FVector Target = Waypoints[WaypointIndex];

	const FVector ToTarget = Target - Cur;
	const float Dist = ToTarget.Size();
	const float Step = MoveSpeed * DeltaSeconds;

	if (Dist <= Step || Dist < 1.0f)
	{
		SetActorLocation(Target);
		WaypointIndex++;

		if (WaypointIndex >= Waypoints.Num())
		{
			bMoving = false;
			Waypoints.Reset();
			WaypointIndex = 0;
		}
		return;
	}

	SetActorLocation(Cur + (ToTarget / Dist) * Step);
}

void AUnitActor::RefreshForNewTurn()
{
	MovePointsRemaining = MovePointsMax;
}

bool AUnitActor::TryStartMovePath(const TArray<TacticsCore::TilePos>& InPath)
{
	const int32 Steps = InPath.Num() - 1;
	if (Steps <= 0) {
		return false;
	}

	if (Steps > MovePointsRemaining) {
		return false;
	}

	UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) {
		return false;
	}

	Waypoints.Reset();
	Waypoints.Reserve(InPath.Num());

	for (const TacticsCore::TilePos& T : InPath)
	{
		Waypoints.Add(Grid->TileToWorldCenter(T));
	}

	MovePointsRemaining -= Steps;
	Tile = InPath.Last();

	bMoving = true;
	WaypointIndex = 1;
	return true;
}

void AUnitActor::RefreshWorldLocationFromTile()
{
	UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) {
		return;
	}

	SetActorLocation(Grid->TileToWorldCenter(Tile));
}

void AUnitActor::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	ApplySelectionVisuals();
}

void AUnitActor::ApplySelectionVisuals()
{
	SetActorScale3D(bIsSelected ? FVector(1.20f) : FVector(1.0f));
}
