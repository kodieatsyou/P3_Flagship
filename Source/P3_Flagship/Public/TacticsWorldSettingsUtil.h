#pragma once

#include "CoreMinimal.h"

class UWorld;
class ATacticsWorldSettings;

namespace TacticsWorldSettingsUtil
{
	const ATacticsWorldSettings* Get(const UWorld* World);
}
