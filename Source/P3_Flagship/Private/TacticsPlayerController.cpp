#include "TacticsPlayerController.h"
#include "UnitActor.h"
#include "GridWorldSubsystem.h"
#include "TacticsGameStateSubsystem.h"
#include "TacticsDebugDrawSubsystem.h"
#include "TacticsPathfinding.h"
#include "TacticsReachability.h"
#include "TacticsCombat.h"
#include "Engine/World.h"

ATacticsPlayerController::ATacticsPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATacticsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UTacticsDebugDrawSubsystem* Debug = GetWorld()->GetSubsystem<UTacticsDebugDrawSubsystem>())
	{
		Debug->RedrawGrid();
	}
}

void ATacticsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftClick", IE_Pressed, this, &ATacticsPlayerController::OnLeftClick);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &ATacticsPlayerController::OnRightClick);

	InputComponent->BindAction("EndTurn", IE_Pressed, this, &ATacticsPlayerController::OnEndTurnTest);
}

void ATacticsPlayerController::OnLeftClick()
{
	AUnitActor* Hovered = GetHoveredUnit();
	if (!Hovered) {
		return;
	}

	UTacticsGameStateSubsystem* GameState = GetWorld()->GetSubsystem<UTacticsGameStateSubsystem>();
	if (!GameState) {
		return;
	}

	if (!GameState->IsUnitControllable(Hovered)) {
		return;
	}

	SelectUnit(Hovered);
}

void ATacticsPlayerController::SelectUnit(AUnitActor* Unit)
{
	if (SelectedUnit == Unit) {
		return;
	}

	if (SelectedUnit) {
		SelectedUnit->SetSelected(false);
	}

	SelectedUnit = Unit;

	if (SelectedUnit)
	{
		SelectedUnit->SetSelected(true);
		RecomputeAndDrawReachOverlay();
	}
	else
	{
		ReachableSet.Reset();
		if (UTacticsDebugDrawSubsystem* Debug = GetWorld()->GetSubsystem<UTacticsDebugDrawSubsystem>())
		{
			Debug->ClearReachOverlay();
			Debug->ClearSelectedTile();
		}
	}
}

void ATacticsPlayerController::RecomputeAndDrawReachOverlay()
{
	ReachableSet.Reset();

	if (!SelectedUnit) {
		return;
	}

	UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	UTacticsDebugDrawSubsystem* Debug = GetWorld()->GetSubsystem<UTacticsDebugDrawSubsystem>();
	if (!Grid || !Debug) {
		return;
	}

	const TacticsCore::GridDesc Desc = Grid->GetGridDesc();
	const TacticsCore::TilePos Start = SelectedUnit->GetTile();
	const int32 MP = SelectedUnit->MovePointsRemaining;

	const TacticsCore::ReachResult Reach = TacticsCore::ComputeReachableTiles(
		Desc,
		Start,
		MP,
		&UGridWorldSubsystem::IsBlockedFn,
		Grid
	);

	for (const TacticsCore::TilePos& T : Reach.Reachable)
	{
		ReachableSet.Add(Desc.ToIndex(T));
	}

	Debug->SetReachOverlay(Reach.Reachable);
	Debug->SetSelectedTile(Start);

	RecomputeAndDrawAttackRange();
}

bool ATacticsPlayerController::IsTileReachable(const TacticsCore::TilePos& Tile) const
{
	if (!SelectedUnit) {
		return false;
	}

	UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) {
		return false;
	}

	const TacticsCore::GridDesc Desc = Grid->GetGridDesc();
	return ReachableSet.Contains(Desc.ToIndex(Tile));
}

void ATacticsPlayerController::OnRightClick()
{
	if (!SelectedUnit) {
		return;
	}

	UTacticsGameStateSubsystem* GameState = GetWorld()->GetSubsystem<UTacticsGameStateSubsystem>();
	if (!GameState) {
		return;
	}

	if (!GameState->IsUnitControllable(SelectedUnit))
	{
		SelectUnit(nullptr);
		return;
	}

	UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) {
		return;
	}

	// ------------------------------------------------------------
	// 1) ATTACK
	// ------------------------------------------------------------
	if (AUnitActor* TargetUnit = GetHoveredUnit())
	{

		if (TargetUnit == SelectedUnit) {
			return;
		}

		// Must be allowed to attack this turn
		if (!SelectedUnit->CanAttack()) {
			return;
		}

		const FTacticsUnitRecord* AttackerRec = GameState->GetUnitRecordByActor(SelectedUnit);
		const FTacticsUnitRecord* TargetRec = GameState->GetUnitRecordByActor(TargetUnit);
		if (!AttackerRec || !TargetRec) {
			return;
		}

		if (AttackerRec->Team == TargetRec->Team) {
			return;
		}

		const TacticsCore::TilePos AttackerTile = SelectedUnit->GetTile();
		const TacticsCore::TilePos TargetTile = TargetUnit->GetTile();


		if (!IsTileInAttackRange(TargetTile)) {
			return;
		}

		if (SelectedUnit->Weapon.bRequiresLOS)
		{
			const bool bHasLOS = TacticsCore::HasLineOfSight(
				Grid->GetGridDesc(),
				AttackerTile,
				TargetTile,
				&UGridWorldSubsystem::IsBlockedFn,
				Grid
			);

			if (!bHasLOS) {
				return;
			}
		}

		// Apply damage and consume attack
		TargetUnit->ApplyDamage(SelectedUnit->Weapon.Damage);
		SelectedUnit->bHasAttackedThisTurn = true;

		if (TargetUnit->IsDead())
		{
			GameState->HandleUnitDeath(TargetUnit);
		}

		RecomputeAndDrawAttackRange();
		return;
	}

	// ------------------------------------------------------------
	// 2) MOVE 
	// ------------------------------------------------------------
	TacticsCore::TilePos TargetTile;
	FVector HitWorld;
	if (!GetHoveredTile(TargetTile, HitWorld)) {
		return;
	}

	if (!IsTileReachable(TargetTile)) {
		return;
	}

	const TacticsCore::TilePos StartTile = SelectedUnit->GetTile();
	const TacticsCore::GridDesc Desc = Grid->GetGridDesc();

	TacticsCore::PathResult Path = TacticsCore::FindPathAStar(
		Desc,
		StartTile,
		TargetTile,
		&UGridWorldSubsystem::IsBlockedFn,
		Grid,
		nullptr
	);

	if (Path.bSuccess && Path.Path.Num() >= 2)
	{
		if (SelectedUnit->TryStartMovePath(Path.Path))
		{
			// Movement changes position + MP, so update overlays.
			RecomputeAndDrawReachOverlay();
			RecomputeAndDrawAttackRange();
		}
	}
}

void ATacticsPlayerController::OnEndTurnTest()
{
	UTacticsGameStateSubsystem* GameState = GetWorld()->GetSubsystem<UTacticsGameStateSubsystem>();
	if (!GameState) {
		return;
	}

	GameState->EndTurn();

	if (SelectedUnit && !GameState->IsUnitControllable(SelectedUnit))
	{
		SelectUnit(nullptr);
	}
	else
	{
		RecomputeAndDrawReachOverlay();
	}
}

bool ATacticsPlayerController::GetHoveredTile(TacticsCore::TilePos& OutTile, FVector& OutHitWorld) const
{
	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit)) {
		return false;
	}

	OutHitWorld = Hit.ImpactPoint;

	if (UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>())
	{
		return Grid->WorldToTile(OutHitWorld, OutTile);
	}
	return false;
}

AUnitActor* ATacticsPlayerController::GetHoveredUnit() const
{
	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit)) {
		return nullptr;
	}

	return Cast<AUnitActor>(Hit.GetActor());
}

void ATacticsPlayerController::RecomputeAndDrawAttackRange()
{
	AttackRangeSet.Reset();

	if (!SelectedUnit || !SelectedUnit->CanAttack())
	{
		if (UTacticsDebugDrawSubsystem* Debug = GetWorld()->GetSubsystem<UTacticsDebugDrawSubsystem>())
		{
			Debug->ClearAttackRangeOverlay();
		}
		return;
	}

	UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	UTacticsDebugDrawSubsystem* Debug = GetWorld()->GetSubsystem<UTacticsDebugDrawSubsystem>();
	if (!Grid || !Debug) {
		return;
	}

	const TacticsCore::GridDesc Desc = Grid->GetGridDesc();
	const TacticsCore::TilePos Start = SelectedUnit->GetTile();
	const int32 Range = SelectedUnit->Weapon.RangeTiles;

	TArray<TacticsCore::TilePos> Tiles;

	for (int32 y = 0; y < Desc.Height; ++y)
	{
		for (int32 x = 0; x < Desc.Width; ++x)
		{
			TacticsCore::TilePos T(x, y);
			if (TacticsCore::ManhattanDistance(Start, T) <= Range)
			{
				const int32 Idx = Desc.ToIndex(T);
				AttackRangeSet.Add(Idx);
				Tiles.Add(T);
			}
		}
	}

	Debug->SetAttackRangeOverlay(Tiles);
}

bool ATacticsPlayerController::IsTileInAttackRange(const TacticsCore::TilePos& Tile) const
{
	if (!SelectedUnit) {
		return false;
	}

	UGridWorldSubsystem* Grid = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	if (!Grid) {
		return false;
	}

	const TacticsCore::GridDesc Desc = Grid->GetGridDesc();
	const int32 TileIndex = Desc.ToIndex(Tile);

	return AttackRangeSet.Contains(TileIndex);
}