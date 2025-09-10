// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.pb.h"
#include "S1EntityStatData.h"
#include "Struct.pb.h"
#include "Components/S1EntityStatComponent.h"
#include "GameFramework/Character.h"
#include "Interface/Damageable.h"
#include "Interface/S1Widget.h"
#include "S1Player.generated.h"

class US1WeaponComponent;

UCLASS()
class S1_API AS1Player : public ACharacter, public IDamageable, public IS1Widget
{
	GENERATED_BODY()

public:
	AS1Player();
	virtual ~AS1Player();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

public:
	bool IsMyPlayer();

	Protocol::MoveState GetMoveState() { return PlayerInfo->state(); }
	void SetMoveState(Protocol::MoveState State);

protected:
	class US1NetworkManager* GetNetworkManager() const;

public:
	void SetPlayerInfo(const Protocol::PosInfo& Info);
	void SetDestInfo(const Protocol::PosInfo& Info);
	void SetDestInfoDirectly(const Protocol::PosInfo& Info);
	Protocol::PosInfo* GetPlayerInfo() { return PlayerInfo; }

public:
	void MoveToTargetLocation();

protected:
	class Protocol::PosInfo* PlayerInfo; // ���� ��ġ
	class Protocol::PosInfo* DestInfo; // ������
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<US1WeaponComponent> WeaponComponent;

public:
	void Shoot(FVector StartLocation, FVector TargetLocation, const Protocol::ObjectInfo& bullet_info);

protected:
	float AcceptanceRadius = 1.0f; // ��ȿ �Ÿ�

	void CheckArrival();
	void OnArrival();

/* Stat */
public:
	virtual void TakeDamage(AActor* from, float damage) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class US1WidgetComponent> HpBar;

	FORCEINLINE US1EntityStatComponent* GetStat() { return StatComponent; }
	void SetupCharacterWidget(US1UserWidget* InUserWidget) override;
	void SetDead();
	void PlayDeadAnimation();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<US1EntityStatComponent> StatComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category =Init, Meta = (AllowPrivateAccess = "true", Tooltip = "게임 시작 시 초기화 될 캐릭터의 스텟"))
	TObjectPtr<US1EntityStatData> CharacterInitalizeStatData;
};
