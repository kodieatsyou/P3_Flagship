#include "TacticsMovementSystem.h"
#include "Math/UnrealMathUtility.h"

bool FMovementSystem::StartMove(uint32 EntityId, const TArray<TacticsCore::TilePos>& Path)
{
    if (Path.Num() < 2)
        return false;

    if (ActiveMoves.Contains(EntityId))
        return false;

    FMoveTask T;
    T.Path = Path;
    T.PathIndex = 0;
    T.Progress = 0.0f;

    ActiveMoves.Add(EntityId, MoveTemp(T));
    return true;
}

void FMovementSystem::CancelMove(uint32 EntityId)
{
    ActiveMoves.Remove(EntityId);
}

void FMovementSystem::Tick(float DeltaSeconds, const FCallbacks& Cb)
{
    if (!Cb.GetUnitTile || !Cb.SetUnitTile || !Cb.OnStepCommitted || !Cb.OnVisualUpdate || !Cb.OnMoveFinished)
        return;

    for (auto It = ActiveMoves.CreateIterator(); It; ++It)
    {
        const uint32 EntityId = It.Key();
        FMoveTask& Task = It.Value();

        if (Task.PathIndex >= Task.Path.Num() - 1)
        {
            Cb.OnMoveFinished(EntityId, Cb.User);
            It.RemoveCurrent();
            continue;
        }

        Task.Progress += DeltaSeconds / TileMoveDuration;

        while (Task.Progress >= 1.0f && Task.PathIndex < Task.Path.Num() - 1)
        {
            const TacticsCore::TilePos From = Task.Path[Task.PathIndex];
            const TacticsCore::TilePos To = Task.Path[Task.PathIndex + 1];

            Cb.OnStepCommitted(EntityId, From, To, Cb.User);
            Cb.SetUnitTile(EntityId, To, Cb.User);

            Task.PathIndex++;
            Task.Progress -= 1.0f;
        }

        if (Task.PathIndex >= Task.Path.Num() - 1)
        {
            Cb.OnMoveFinished(EntityId, Cb.User);
            It.RemoveCurrent();
            continue;
        }

        const TacticsCore::TilePos From = Task.Path[Task.PathIndex];
        const TacticsCore::TilePos To = Task.Path[Task.PathIndex + 1];
        const float Alpha = FMath::Clamp(Task.Progress, 0.0f, 1.0f);

        Cb.OnVisualUpdate(EntityId, From, To, Alpha, Cb.User);
    }
}
