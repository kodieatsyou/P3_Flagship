// Fill out your copyright notice in the Description page of Project Settings.


#include "TacticsPlayerController.h"

#include "TacticsSimWorldSubsystem.h"
#include "GridWorldSubsystem.h"
#include "TacticsCommands.h"

void ATacticsPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	InputComponent->BindAction("ClickMove", IE_Pressed, this, &ATacticsPlayerController::ClickMove);
	InputComponent->BindAction("EndTurn", IE_Pressed, this, &ATacticsPlayerController::EndTurn);

}

void ATacticsPlayerController::ClickMove() {
	UWorld* W = GetWorld();

	if (!W) {
		return;
	}

	UTacticsSimWorldSubsystem* Sim = W->GetSubsystem<UTacticsSimWorldSubsystem>();
	UGridWorldSubsystem* Grid = W->GetSubsystem<UGridWorldSubsystem>();

	if (!Sim || !Grid) {
		return;
	}

	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (!bHit) {
		return;
	}

	const TacticsCore::TilePos Dest = Grid->WorldToTile(Hit.ImpactPoint);
	const uint32 Active = Sim->GetActiveEntity();
	if (Active == 0) {
		return;
	}

	Sim->EnqueueCommand(TacticsCore::MakeMove(Active, Dest));

	UE_LOG(LogTemp, Log, TEXT("[Input] Move Active=%u -> Tile=(%d,%d)"), Active, Dest.x, Dest.y);
}

void ATacticsPlayerController::EndTurn() {
	if (UWorld* W = GetWorld()) {
		if (UTacticsSimWorldSubsystem* Sim = W->GetSubsystem<UTacticsSimWorldSubsystem>()) {
			Sim->EnqueueCommand(TacticsCore::MakeEndTurn());
			UE_LOG(LogTemp, Log, TEXT("[Input] EndTurn"));
		}
	}
}