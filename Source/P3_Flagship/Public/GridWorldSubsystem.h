// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TacticsGrid.h"
#include "GridWorldSubsystem.generated.h"



UCLASS()
class P3_FLAGSHIP_API UGridWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	public:
		virtual void Initialize(FSubsystemCollectionBase& Collection) override;
		virtual void Deinitialize() override;

		const TacticsCore::GridDesc& GetGridDesc() const { return grid; }

		TacticsCore::TilePos WorldToTile(const FVector& world) const;
		FVector TileToWorldCenter(const TacticsCore::TilePos& tile) const;

		bool InBounds(const TacticsCore::TilePos& tile) const;

		void SetDebugDrawEnabled(bool bEnabled) { bDebugDraw = bEnabled; }
		bool IsDebugDrawEnabled() const { return bDebugDraw; }

		void DebugDrawGrid(UWorld *world) const;

	private:
		FVector origin = FVector::ZeroVector;

		TacticsCore::GridDesc grid;

		bool bDebugDraw = true;
};
