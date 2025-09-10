// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../S1BaseProjectile.h"
#include "S1ProjectileData.h"
#include "S1WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1WeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	US1WeaponData()
	{
		MaxAmmo = 30;
		FireRate = 0.05f;
	}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AS1BaseProjectile> Projectile;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "200"))
	int MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMin = "0.1", ClampMax = "10"))
	float FireRate;
public:
	FORCEINLINE int GetMaxAmmo() { return MaxAmmo; }
	FORCEINLINE float GetFireRate() { return FireRate; }

};
