#include "TacticsWorldSettingsUtil.h"
#include "TacticsWorldSettings.h"
#include "Engine/World.h"

namespace TacticsWorldSettingsUtil
{
	const ATacticsWorldSettings* Get(const UWorld* World)
	{
		if (!World) {
			return nullptr;
		}
		return Cast<ATacticsWorldSettings>(World->GetWorldSettings());
	}
}
