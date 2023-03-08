// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponents.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"
#include "SWorldUserWidget.h"

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interact Component"), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponents::USInteractionComponents()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	TraceDistance = 500;
	RadiusDistance = 30.0f;
	CollisionChannel = ECC_WorldDynamic;
}




// Called when the game starts
void USInteractionComponents::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USInteractionComponents::FindBestInteractable()
{
	bool DebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	// Hit result required for line tracing
	FHitResult hit;

	// CollisionObject querey setting required by line trace. We make it so it only querys for worlddynamic objects
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
	
	// used for whoever owns this component in general. We use this to get their eyesight later on
	AActor* owner = GetOwner();

	FVector End;				// The end point we want to stop tracing to
	FVector EyeLocation;		
	FRotator EyeRotation;		// Used by the GetActorEyesViewpoint function which bases its value off the control rotation (camera not actor)

	// Update the Eyelocation and rotation based on the actor itself
	owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	// Same as the eyelocation but it's extended out by a unit based on the camera's current location
	End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	/* Draw a debug line based on everythign we've done so far and store it into a boolean called blockingHit
	// bool blockingHit = GetWorld()->LineTraceSingleByObjectType(hit, EyeLocation, End, ObjectQueryParams);
	AActor* hitactor = hit.GetActor();
	*/

	TArray<FHitResult> Hits;
	FCollisionShape shape;


	//float radius = 30.0f; /*already replaced value*/

	shape.SetSphere(RadiusDistance);

	// Draw a debug line based on everythign we've done so far and store it into a boolean called blockingHit
	bool blockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, shape);

	// NOTES: The U prefix is necessary when you want to know if its using that interface, the I is for calling methods
	// within said interface

	// clear the focused actor
	FocusedActor = nullptr;

	FColor LineColor = blockingHit ? FColor::Green : FColor::Red;

	for (FHitResult Hit : Hits)
	{
		if (Hit.GetActor()->Implements<USGameplayInterface>())
		{
			FocusedActor = Hit.GetActor();
			break;
		}

		if (DebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, RadiusDistance, 32, LineColor, false, 0.0f);
		}
	}
	
	// Original Method (SOLO)
	/*
	if (hitactor)
	{
		if (hitactor->Implements<USGameplayInterface>())
		{
			APawn* MyPawn = Cast<APawn>(owner);

			// The same interact from the interface but here we are calling it's direct implementation
			ISGameplayInterface::Execute_Interact(hitactor, MyPawn);
		}
	}
	*/

	// Alternate Method (SOLO)
	/*
	// Alternate method (SOLO)
	if (hitactor)
	{
		if (hitactor->GetClass()->ImplementsInterface(USGameplayInterface::StaticClass()) )
		{
			APawn* MyPawn = Cast<APawn>(owner);
			ISGameplayInterface::Execute_Interact(hitactor, MyPawn);
		}
	}
	*/

	if (FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(WidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), WidgetClass);
		}

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport() )
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		if (DefaultWidgetInstance)
		{
			if (DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->RemoveFromParent();
			}
		}
	}

	if (DebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 0.0f);
	}
}


// Called every frame
void USInteractionComponents::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	APawn* MyPawn = Cast<APawn>(GetOwner());

	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}

	
}

void USInteractionComponents::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void USInteractionComponents::ServerInteract_Implementation(AActor* InFocus)
{
	if (InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No focus actor to Interact with");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());

	// The same interact from the interface but here we are calling it's direct implementation
	ISGameplayInterface::Execute_Interact(InFocus, MyPawn);
}