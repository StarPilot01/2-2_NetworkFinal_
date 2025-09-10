// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S1UserWidget.h"
#include "GameData/S1EntityStat.h"
#include "S1HpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1HpBarWidget : public US1UserWidget
{
	GENERATED_BODY()
			
public:
	US1HpBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void UpdateStat(const FS1EntityStat& BaseStat, const FS1EntityStat& ModifierStat);
	void UpdateHpBar(float NewCurrentHp);
	FString GetHpStatText();

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	TObjectPtr<class UTextBlock> HpStat;

	UPROPERTY()
	float CurrentHp;

	UPROPERTY()
	float MaxHp;
};
