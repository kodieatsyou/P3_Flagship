#include "TacticsBootstrapActor.h"

#include "TacticsSimWorldSubsystem.h"
#include "UnitActor.h"
#include "EngineUtils.h"

// Sets default values
ATacticsBootstrapActor::ATacticsBootstrapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATacticsBootstrapActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* W = GetWorld();

	if (!W) {
		return;
	}

	UTacticsSimWorldSubsystem* Sim = W->GetSubsystem<UTacticsSimWorldSubsystem>();

	if (!Sim) {
		return;
	}

	for (TActorIterator<AUnitActor> It(W); It; ++It) {
		AUnitActor* Unit = *It;
		Sim->RegisterUnit(Unit, TeamForAllUnits);
	}

	if (bAutoStartCombat) {
		Sim->StartCombat();
	}

	if (bDrawDebug) {
		Sim->bDrawDebugGrid = true;
	}
	
}

