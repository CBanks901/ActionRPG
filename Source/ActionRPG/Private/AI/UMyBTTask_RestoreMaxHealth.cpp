// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/UMyBTTask_RestoreMaxHealth.h"
#include "SAttributeComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UUMyBTTask_RestoreMaxHealth::UUMyBTTask_RestoreMaxHealth()
{

}

EBTNodeResult::Type UUMyBTTask_RestoreMaxHealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* OwningController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();
	
	bool result = false;

	if (OwningController)
	{
		AActor* Pawn_Ref = Cast<AActor>(OwningController->GetPawn());
		if (Pawn_Ref)
		{
			USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Pawn_Ref);
			if (AttributeComp)
			{
				float HealthRestore = BBC->GetValueAsFloat(HealthTarget.SelectedKeyName);

				if (HealthRestore > 0.0f)
					result = AttributeComp->ApplyHealthChange(Pawn_Ref, HealthRestore);
			}
			else
			{
				return EBTNodeResult::Failed;
			}
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return result ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}