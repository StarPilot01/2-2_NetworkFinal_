// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "S1Widget.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class US1Widget : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class S1_API IS1Widget
{
	GENERATED_BODY()

public:
	virtual void SetupCharacterWidget(class US1UserWidget* InUserWidget) = 0;
};
