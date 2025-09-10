// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/S1WidgetComponent.h"
#include "S1UserWidget.h"

void US1WidgetComponent::InitWidget()
{
	Super::InitWidget();
	
	US1UserWidget* ABUserWidget = Cast<US1UserWidget>(GetWidget());
	if (ABUserWidget)
	{
		ABUserWidget->SetOwningActor(GetOwner());
	}
}