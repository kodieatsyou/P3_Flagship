#include "TacticsGameMode.h"
#include "TacticsPlayerController.h"

ATacticsGameMode::ATacticsGameMode()
{
	PlayerControllerClass = ATacticsPlayerController::StaticClass();
}
