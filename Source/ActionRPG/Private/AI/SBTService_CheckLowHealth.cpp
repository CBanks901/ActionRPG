// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckLowHealth.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"

void USBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Comp = OwnerComp.GetBlackboardComponent();

	if (Comp)
	{
		AAIController* AI_Owner = OwnerComp.GetAIOwner();

		if (AI_Owner)
		{
			AActor* Owner_Pawn = Cast<AActor>(AI_Owner->GetPawn() );

			if (ensure(Owner_Pawn))
			{
				USAttributeComponent* Attri_Comp = USAttributeComponent::GetAttributes(Owner_Pawn);
				
				if (ensure(Attri_Comp))
				{
					Comp->SetValueAsBool(LowHealthKey.SelectedKeyName, Attri_Comp->LowHealth() );
					if (Attri_Comp->LowHealth() )
						Comp->SetValueAsFloat(HealthTarget.SelectedKeyName, Attri_Comp->MaxDiff());
				}
			}
		}
		//Cast<AActor>(Comp->GetValueAsObject("") );
	}
}
