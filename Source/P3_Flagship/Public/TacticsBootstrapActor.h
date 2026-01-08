// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TacticsBootstrapActor.generated.h"

UCLASS()
class P3_FLAGSHIP_API ATacticsBootstrapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATacticsBootstrapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Tactics")
	int32 TeamForAllUnits = 0;

	UPROPERTY(EditAnywhere, Category = "Tactics")
	bool bAutoStartCombat = true;

	UPROPERTY(EditAnywhere, Category = "Tactics|Debug")
	bool bDrawDebug = true;

};
