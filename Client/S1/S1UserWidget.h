// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "S1UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1UserWidget : public UUserWidget
{
	GENERATED_BODY()
	public:
    	FORCEINLINE void SetOwningActor(AActor* NewOwner) { OwningActor = NewOwner; }
    
    protected:
    	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor")
    	TObjectPtr<AActor> OwningActor;
};
