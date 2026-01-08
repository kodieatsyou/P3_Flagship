

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TacticsCoreTypes.h"
#include "TacticsGameStateSubsystem.generated.h"

class AUnitActor;

UENUM(BlueprintType)
enum class ETacticsTeam : uint8
{
	Team0 = 0,
	Team1 = 1
};

UCLASS()
class P3_FLAGSHIP_API UTacticsGameStateSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
};