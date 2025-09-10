// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/S1WeaponComponent.h"

#include "S1NetworkManager.h"
#include "Character/S1Player.h"

void US1WeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AActor* actor = GetOwner();
	if (actor == nullptr)
	{
		return;
	}
	
	OwnerPlayer = Cast<AS1Player>(actor);
	check(OwnerPlayer != nullptr);
}

void US1WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

void US1WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->GetWorld()->GetTimeSeconds() - LastFireTime > WeaponData->GetFireRate())
	{
		// �߻� �غ�
	}
}

void US1WeaponComponent::FireWeapon(FVector Start, FVector Target, AS1Player* Owner, const Protocol::ObjectInfo& bullet_info)
{
	if (GetOwner()->GetWorld()->GetTimeSeconds() - LastFireTime >= WeaponData->GetFireRate())
	{
		SpawnProjectile(Start, Target, Owner, bullet_info);
		
		LastFireTime = GetOwner()->GetWorld()->GetTimeSeconds();
	}
}

void US1WeaponComponent::SpawnProjectile(FVector Start, FVector Target, AS1Player* Owner, const Protocol::ObjectInfo& bullet_info)
{
	if (GEngine)
	{
		if (UWorld* World = GetWorld())
		{
			US1NetworkManager* NetworkManager = World->GetGameInstance()->GetSubsystem<US1NetworkManager>();
			check(NetworkManager != nullptr);
			AS1BaseProjectile* Projectile = NetworkManager->Create<AS1BaseProjectile>(World, WeaponData->Projectile, bullet_info.object_id());
			check(Projectile != nullptr);
			Projectile->Initialize(Start, Target, Owner, bullet_info);
			Projectile->Shoot();
		}
	}
}
