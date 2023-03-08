// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyBTTask_RangedAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SAttributeComponent.h"

UMyBTTask_RangedAttack::UMyBTTask_RangedAttack()
{
	MaxBulletSpread = 2.0f;
}

EBTNodeResult::Type UMyBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());

		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation("Muzzle_01");

		/// grab the target from the blackboard itself
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor") ); 

		// will error out until we find a pawn (for now it is set in the begin play of the controller itself)
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		if (!USAttributeComponent::isActorAlive(TargetActor) )
		{
			return EBTNodeResult::Failed;
		}

		FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = Direction.Rotation();

		MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
		MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

		FActorSpawnParameters FParams;
		FParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FParams.Instigator = MyPawn;

		AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, FParams);

		return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}

