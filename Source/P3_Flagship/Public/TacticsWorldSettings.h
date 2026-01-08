#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "TacticsGameStateSubsystem.h" // FTacticsUnitDef
#include "TacticsWorldSettings.generated.h"

class AUnitActor;

UCLASS()
class P3_FLAGSHIP_API ATacticsWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	// ---------------- Grid ----------------
	UPROPERTY(EditAnywhere, Category = "Tactics|Grid")
	int32 GridWidth = 12;

	UPROPERTY(EditAnywhere, Category = "Tactics|Grid")
	int32 GridHeight = 12;

	UPROPERTY(EditAnywhere, Category = "Tactics|Grid")
	float TileSize = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Tactics|Grid")
	FVector GridOrigin = FVector(0, 0, 50);

	// ---------------- Debug Draw ----------------
	UPROPERTY(EditAnywhere, Category = "Tactics|Debug")
	bool bEnableDebugDraw = true;

	UPROPERTY(EditAnywhere, Category = "Tactics|Debug")
	bool bDrawGrid = true;

	UPROPERTY(EditAnywhere, Category = "Tactics|Debug")
	bool bDrawReach = true;

	// ---------------- Spawning / Teams ----------------
	UPROPERTY(EditAnywhere, Category = "Tactics|Spawning")
	TSubclassOf<AUnitActor> UnitActorClass;

	UPROPERTY(EditAnywhere, Category = "Tactics|Spawning")
	TArray<FIntPoint> Team0SpawnTiles;

	UPROPERTY(EditAnywhere, Category = "Tactics|Spawning")
	TArray<FIntPoint> Team1SpawnTiles;

	UPROPERTY(EditAnywhere, Category = "Tactics|Defs")
	FTacticsUnitDef DefaultUnitDef;
};
