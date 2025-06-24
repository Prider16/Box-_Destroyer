// Copyright Epic Games, Inc. All Rights Reserved.


#include "Box_DestroyerPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Box_DestroyerCameraManager.h"

ABox_DestroyerPlayerController::ABox_DestroyerPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ABox_DestroyerCameraManager::StaticClass();
}

void ABox_DestroyerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
