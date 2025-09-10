// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct.pb.h"
#include "Components/ActorComponent.h"
#include "../Weapon/DataAssets/S1WeaponData.h"
#include "S1WeaponComponent.generated.h"

class AS1Player;

UCLASS()
class S1_API US1WeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	US1WeaponComponent()
	{
		PrimaryComponentTick.bCanEverTick = true;
		LastFireTime = 0.f;
	}

	virtual void InitializeComponent() override;
protected:
	virtual void BeginPlay() override;

public:
	// Override TickComponent to use DeltaTime for logic
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void FireWeapon(FVector Start, FVector Target, AS1Player* Owner, const Protocol::ObjectInfo& bullet_info);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<US1WeaponData> WeaponData;

	UPROPERTY()
	TObjectPtr<AS1Player> OwnerPlayer;
protected:
	void SpawnProjectile(FVector Start, FVector Target, AS1Player* Owner, const Protocol::ObjectInfo& bullet_info);

	float LastFireTime;
};
