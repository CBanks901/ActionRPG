// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"




void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	FVector2D Screenposition;

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid. Removing Health Widget"));
		return;
	}

	if ( UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, Screenposition) )
	{
		float scale = UWidgetLayoutLibrary::GetViewportScale(this);

		Screenposition /= scale;
	}

	if (ParentSizeBox)
	{
		ParentSizeBox->SetRenderTranslation(Screenposition);
	}
}