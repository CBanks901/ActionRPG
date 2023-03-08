// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffectThorns.h"
#include "SAttributeComponent.h"


USActionEffectThorns::USActionEffectThorns()
{
	bAutoStart = true;
	Duration = 0.0f;
	Period = 0.0f;
}



void USActionEffectThorns::StartAction_Implementation(AActor* Instigator)
{
	static FGameplayTag newTag = FGameplayTag::RequestGameplayTag("Status.Thorns");

	if (newTag.IsValid())
		GrantsTags.AddTag(newTag);

	Super::StartAction_Implementation(Instigator);
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::White, "Calling from within the stop action of Thorns - Start");

}

void USActionEffectThorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	AActor* Ref = Cast<AActor>(GetWorld()->GetOuter() );
	USAttributeComponent *AttriComp = USAttributeComponent::GetAttributes(Ref);
	
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::White, "Calling from within the stop action of Thorns - Stop");
	//if (AttriComp)
	//{
		//AttriComp->OnHealthChanged.AddDynamic(this, );
	//}
}

/*void USActionEffectThorns::ExecutePeriodicEffect_Implementation(AActor* Instigator)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::White, "Calling from within the stop action of Thorns - Execute");
}*/



