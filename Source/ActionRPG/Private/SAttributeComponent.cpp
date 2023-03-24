// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "Math/UnrealMathUtility.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage multiplier for Attribute Component"), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100.f;
	MaxHealth = 100.f;

	Rage = 0.0f;
	MaxRage = 50.0f;

	SetIsReplicatedByDefault(true);
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if (Delta > 0.0f && Health == MaxHealth)
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	/*TODO: if the health somehow fails or goes overboard, add the fix here*/

	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);
	float ActualDelta = NewHealth - OldHealth;


	// Is Server?
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;


		// Send out a signal alert that the health has been updated
		//OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

		if (ActualDelta != 0)
		{
			MulitcastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		// Died
		if (ActualDelta < 0.0f && Health == 0.0f)
		{
			ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if (GM)
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}

		}

	}

	return ActualDelta != 0;
}

bool USAttributeComponent::LowHealth() const
{
	static float percentage = Health / MaxHealth;

	return percentage <= .2 ? true : false;
}

float USAttributeComponent::MaxDiff() const
{
	return MaxHealth - Health;
}





bool USAttributeComponent::Kill(AActor* Instigator)
{
	return ApplyHealthChange(Instigator, -MaxHealth);
}

bool USAttributeComponent::isAlive() const
{
	return Health > 0.0f;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::isActorAlive(AActor* Actor)
{
	USAttributeComponent* Attributes = GetAttributes(Actor);

	if (Attributes)
	{
		return Attributes->isAlive();
	}

	return false;
}

bool USAttributeComponent::IsFullHealth() const
{
	return (Health == MaxHealth);
}

float USAttributeComponent::GetHealthMax() const
{
	return MaxHealth;
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);
}

void USAttributeComponent::MulitcastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}

void USAttributeComponent::AddRage(float Delta)
{
	if (Delta == 0)
		Rage = 0.0f;

	if (Rage < MaxRage)
	{
		Rage += Delta;
		Rage = FMath::Clamp(Rage, 0.0f, MaxRage);
	}
}