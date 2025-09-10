// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S1Actor.generated.h"

UCLASS()
class S1_API AS1Actor : public AActor
{
	GENERATED_BODY()

public:
	AS1Actor();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
public:
	uint64 GetObjectId() const { return ObjectId; }

	void SetObjectId(uint64 objectId);


protected:
	uint64 ObjectId;
};
