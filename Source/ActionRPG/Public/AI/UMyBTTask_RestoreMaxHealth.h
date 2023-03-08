// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UMyBTTask_RestoreMaxHealth.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UUMyBTTask_RestoreMaxHealth : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UUMyBTTask_RestoreMaxHealth();

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector HealthTarget;
};
