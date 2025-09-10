// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/S1EntityStatComponent.h"

US1EntityStatComponent::US1EntityStatComponent()
{
	bWantsInitializeComponent = true;
}

void US1EntityStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	SetHp(BaseStat.MaxHp);
}

float US1EntityStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);

	SetHp(PrevHp - ActualDamage);
	if (CurrentHp <= KINDA_SMALL_NUMBER)
	{
		OnHpZero.Broadcast();
	}
	
	return ActualDamage;
}

void US1EntityStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, BaseStat.MaxHp);
	OnHpChanged.Broadcast(CurrentHp);
}
