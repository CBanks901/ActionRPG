// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;
class UUserWidget;
class USWorldUserWidget;
class USActionComponent;

UCLASS()
class ACTIONRPG_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASAICharacter();

protected:

	USWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeParameter;

	void SetTargetActor(AActor* NewTarget);

	virtual void PostInitializeComponents() override;
	AActor* GetTargetActor() const;
	
	FName TargetActorKey;

	UPROPERTY(VisibleAnywhere,Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* attriComp;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* actionComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpottedWidgetClass;

	//UPROPERTY(EditDefaults)
	//FBlackboardKeySelector Target;
};
