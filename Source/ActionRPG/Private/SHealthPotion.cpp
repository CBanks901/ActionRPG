// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAttributeComponent.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>("PotionMesh");


}


// Called when the game starts or when spawned
void ASHealthPotion::BeginPlay()
{
	Super::BeginPlay();
	TimeLock = false;
}

// Called every frame
void ASHealthPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	//UE_LOG()
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Health potion check..");

	if (!TimeLock)
	{
		AActor* Actor_Ref = Cast<AActor>(InstigatorPawn);
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Actor_Ref);

		if (AttributeComp)
		{
			if (AttributeComp->ApplyHealthChange(Actor_Ref, 20.0f))
			{
				TimeLock = true;

				FTimerDelegate delo;
				delo.BindUFunction(this, "LockEffect", InstigatorPawn);
				GetWorld()->GetTimerManager().SetTimer(LockHandle, delo, 10.0f, false);

				PotionMesh->SetVisibility(false);

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Restoring health");
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Health already at maximum");
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Time still in progress");
	}
}

void ASHealthPotion::LockEffect(AActor* InstigatorObj)
{
	TimeLock = false;
	PotionMesh->SetVisibility(true);

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, "Health potion ready to be used again");
}