// Fill out your copyright notice in the Description page of Project Settings.


#include "S1Actor.h"
#include "S1NetworkManager.h"

AS1Actor::AS1Actor()
{
	
}

void AS1Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AS1Actor::BeginDestroy()
{
	Super::BeginDestroy();

	if (!GEngine)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	UGameInstance* GameInstance = World->GetGameInstance();
	if (GameInstance == nullptr)
	{
		return;
	}
	US1NetworkManager* NetworkManager = GameInstance->GetSubsystem<US1NetworkManager>();
	if (NetworkManager == nullptr)
	{
		return;
	}
	// 네트워크 매니저에서 제거
	NetworkManager->Objects.Remove(ObjectId);
}

void AS1Actor::SetObjectId(uint64 objectId)
{
	ObjectId = objectId;
	if (!GEngine)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	UGameInstance* GameInstance = World->GetGameInstance();
	if (GameInstance == nullptr)
	{
		return;
	}
	US1NetworkManager* NetworkManager = GameInstance->GetSubsystem<US1NetworkManager>();
	if (NetworkManager == nullptr)
	{
		return;
	}
	// 네트워크 매니저에서 추가
	NetworkManager->Objects.Add({ObjectId, this});
}
