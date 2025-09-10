// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "S1EntityStat.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FS1EntityStat : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	FS1EntityStat operator+(const FS1EntityStat& Other) const
	{
		const float* const ThisPtr = reinterpret_cast<const float* const>(this);
		const float* const OtherPtr = reinterpret_cast<const float* const>(&Other);
		
		FS1EntityStat Result;
		float* ResultPtr = reinterpret_cast<float*>(&Result);
		int32 statNum = sizeof(FS1EntityStat) / sizeof(float);
		for (int32 i = 0; i < statNum; i++)
		{
			ResultPtr[i] = ThisPtr[i] + OtherPtr[i];
		}
		return Result;
	}
};
