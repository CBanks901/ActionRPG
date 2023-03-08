// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponents.generated.h"

class USWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API USInteractionComponents : public UActorComponent
{
	GENERATED_BODY()

public:
	void PrimaryInteract();

public:	
	// Sets default values for this component's properties
	USInteractionComponents();

protected:

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InFocus);

	// Called when the game starts
	virtual void BeginPlay() override;

	void FindBestInteractable();

	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USWorldUserWidget> WidgetClass;

	UPROPERTY()
	USWorldUserWidget* DefaultWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float RadiusDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
