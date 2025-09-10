// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameData/S1EntityStat.h"
#include "S1EntityStatData.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1EntityStatData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FS1EntityStat Stat;
};
