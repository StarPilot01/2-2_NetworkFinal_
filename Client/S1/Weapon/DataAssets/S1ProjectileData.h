// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "S1ProjectileData.generated.h"

USTRUCT(BlueprintType)
struct FS1Projectile
{
	GENERATED_BODY()
public:
	FS1Projectile()
	{
		Damage = 100.f;
		Speed = 10.f;
		MaxDistance = 1000.f;
		bCanPassThrough = false;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanPassThrough;
};

/**
 * 
 */
UCLASS(Blueprintable)
class S1_API US1ProjectileData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FS1Projectile Data;
};
