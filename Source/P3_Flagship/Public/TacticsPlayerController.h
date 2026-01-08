// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TacticsPlayerController.generated.h"

UCLASS()
class P3_FLAGSHIP_API ATacticsPlayerController : public APlayerController
{
	GENERATED_BODY()

	protected:
		virtual void SetupInputComponent() override;

	private:
		void ClickMove();
		void EndTurn();

	
};
