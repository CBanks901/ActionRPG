// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"
#include "BrainComponent.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "SActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SMiscellanousAIComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	attriComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	actionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	MiscallenousComp = CreateDefaultSubobject<USMiscellanousAIComponent>("MiscallaneousComp");

	TimeParameter = "Time To Hit";

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	TargetActorKey = "TargetActor";
	
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	/*if (!GetTargetActor())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Target Actor for the AI is not set") ) );
	}*/

	AAIController* AIC = Cast<AAIController>(GetController());

	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	attriComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);

	if (!MiscallenousComp->Spawned)
	{
		MiscallenousComp->KillReward = 20.0f;
	}
}

AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIC = Cast<AAIController>(GetController());

	if (AIC)
	{
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}

	return nullptr;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
		//SetTargetActor(Pawn);
		//DrawDebugString(GetWorld(), GetActorLocation(), "Player Spotted!!", nullptr, FColor::White, 4.0f, true);

		if (GetTargetActor() != Pawn)
		{
			SetTargetActor(Pawn);

			USWorldUserWidget* NewWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);

			if (NewWidget)
			{
				NewWidget->AttachedActor = this;
				
				NewWidget->AddToViewport(10);
			}
		}
	
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{

		if (InstigatorActor != this)
		{
			// This line won't check if other ai hit each other. So basically if an AI hits another AI that responding AI will become the target
			SetTargetActor(InstigatorActor);
		}

		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);

			ActiveHealthBar->AttachedActor = this;
			if (ActiveHealthBar)
					ActiveHealthBar->AddToViewport();
		}

		GetMesh()->SetScalarParameterValueOnMaterials(TimeParameter, GetWorld()->TimeSeconds);

		// Died
		if (NewHealth <= 0.0f)
		{
			// Stop BT
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			SetLifeSpan(10.0f);
		}
	}

}
