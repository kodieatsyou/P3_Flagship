#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TacticsCommands.h"
#include "TacticsCoreTypes.h"
#include "Containers/Map.h"
#include "Containers/Queue.h"
#include "Containers/Array.h"

#include "TacticsSimWorldSubsystem.generated.h"

class AUnitActor;
class AGridWorldSubsystem;

USTRUCT()
struct FTacticsUnitState {
	GENERATED_BODY()

	TacticsCore::EntityId Id = 0;
	TacticsCore::TilePos Tile{ 0, 0 };

	UPROPERTY()
	TObjectPtr<AUnitActor> Actor = nullptr;

	int32 Team = 0;
	bool bAlive = true;
};

UCLASS()
class P3_FLAGSHIP_API UTacticsSimWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return true; }

	TacticsCore::EntityId RegisterUnit(AUnitActor* UnitActor, int32 Team);
	bool UnregisterUnit(TacticsCore::EntityId Id);

	bool TryGetUnitState(TacticsCore::EntityId Id, FTacticsUnitState& Out) const;
	TacticsCore::EntityId GetActiveEntity() const { return ActiveEntity; }
	bool IsInCombat() const { return bCombatActive; }

	void StartCombat();
	void EndCombat();

	void EnqueueCommand(const TacticsCore::Command& Cmd);

private:
	void ProcessCommands(int32 MaxPerTick);
	void ApplyCommand(const TacticsCore::Command& Cmd);

	void ApplyMove(const TacticsCore::MoveCommand& Move);
	void ApplyEndTurn();

	bool CanAct(TacticsCore::EntityId Id) const;
	bool IsTileOccupied(const TacticsCore::TilePos& Tile) const;
	TacticsCore::EntityId GetOccupant(const TacticsCore::TilePos& Tile) const;

	static int64 PackTileKey(const TacticsCore::TilePos& Tile);
	void SetOccupant(const TacticsCore::TilePos& Tile, TacticsCore::EntityId Id);
	void ClearOccupant(const TacticsCore::TilePos& Tile);

	void RebuildTurnOrder();
	void AdvanceTurnToNextAlive();

	TacticsCore::EntityId NextId = 1;

	UPROPERTY()
	TMap<uint32, FTacticsUnitState> Units;

	TMap<int64, uint32> Occupancy;

	uint32 ActiveEntity = 0;
	bool bCombatActive = false;
	TArray<uint32> TurnOrder;
	int32 TurnIndex = 0;

	TQueue<TacticsCore::Command> CommandQueue;

	int32 MaxCommandsPerTick = 0;
};
