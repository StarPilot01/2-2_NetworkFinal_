// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/S1EntityStat.h"
#include "S1EntityStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangeDelegate, float /* CurrentHp */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FS1EntityStat& /* BaseStat */, const FS1EntityStat& /* ModifierStat */);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class S1_API US1EntityStatComponent : public UActorComponent
{
	GENERATED_BODY()

// Init Section
public:
	US1EntityStatComponent();

// Init Section
public:
	UFUNCTION(Blueprintable, BlueprintCallable)
	virtual void InitializeComponent() override;

// Stat Section
public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangeDelegate OnHpChanged;
	FOnStatChangedDelegate OnStatChanged;

	FORCEINLINE void AddBaseStat(const FS1EntityStat& InAddStat)
	{
		BaseStat = BaseStat + InAddStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}
	FORCEINLINE void SetBaseStat(const FS1EntityStat& InBaseStat)
	{
		BaseStat = InBaseStat;
		CurrentHp = InBaseStat.MaxHp;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}
	FORCEINLINE void SetModifierStat(const FS1EntityStat& InModifierStat)
	{
		ModifierStat = InModifierStat;
		OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat());
	}

	FORCEINLINE const FS1EntityStat& GetBaseStat() const { return BaseStat; }
	FORCEINLINE const FS1EntityStat& GetModifierStat() const { return ModifierStat; }
	FORCEINLINE FS1EntityStat GetTotalStat() const { return BaseStat + ModifierStat; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	FORCEINLINE void HealHp(float InHealAmount)
	{
		CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0, GetTotalStat().MaxHp);
		OnHpChanged.Broadcast(CurrentHp);
	}
	float ApplyDamage(float InDamage);

// Data Section
public:
	void SetHp(float NewHp);
	
protected:
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	float CurrentHp;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FS1EntityStat BaseStat;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	FS1EntityStat ModifierStat;
};
