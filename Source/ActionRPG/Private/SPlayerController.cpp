// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "Blueprint/UserWidget.h"

void ASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}

void ASPlayerController::TooglePauseMenu()
{
	if (PauseMenu_Instance && PauseMenu_Instance->IsInViewport())
	{
		PauseMenu_Instance->RemoveFromParent();
		PauseMenu_Instance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		return;
	}

	PauseMenu_Instance = CreateWidget<UUserWidget>(this, PauseMenuClass);

	if (PauseMenu_Instance)
	{
		PauseMenu_Instance->AddToViewport(100);

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::TooglePauseMenu);
}
