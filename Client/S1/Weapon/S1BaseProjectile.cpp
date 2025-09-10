// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/S1BaseProjectile.h"

#include "S1NetworkManager.h"
#include "Weapon/DataAssets/S1ProjectileData.h"
#include "Character/S1Player.h"

void AS1BaseProjectile::Initialize(const FVector& Start, const FVector& Target, AS1Player* InOwner, const Protocol::ObjectInfo& bullet_info)
{
	SetObjectId(bullet_info.object_id());
	
	StartLocation = Start;
	FVector ShootTarget = Target;
	ShootTarget.Z = Start.Z;
	ShootDirection = (ShootTarget - Start).GetSafeNormal();
	check(!ShootDirection.IsNearlyZero());
	
	check(MovementComponent != nullptr);
	MovementComponent->InitialSpeed = ProjectileData->Data.Speed;
	MovementComponent->MaxSpeed = ProjectileData->Data.Speed;
	MovementComponent->Velocity = ShootDirection * MovementComponent->InitialSpeed;
	check(!MovementComponent->Velocity.IsNearlyZero());

	SetActorLocation(Start);
	
	OwnerPlayer = InOwner;

	bIsInitialized = true;
}

void AS1BaseProjectile::Shoot()
{
	check(bIsInitialized);
	check(MovementComponent != nullptr);
	MovementComponent->Activate();

	GetWorld()->GetTimerManager().SetTimer(
		DestroyDistanceHandler,
		this,
		&AS1BaseProjectile::CheckDistance,
		0.02f,
		true
	);
	
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AS1BaseProjectile::Destroy,
		5.0f,
		false
	);

	if (OwnerPlayer->IsMyPlayer()) // 디버그에서 3번 그려지는거 방지, 별로 상관없긴 해.
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Projectile Fired!"));
	}
}

void AS1BaseProjectile::CheckDistance()
{
	const float Distance = FVector::Dist(StartLocation, GetActorLocation());
	if (Distance > ProjectileData->Data.MaxDistance)
	{
		Destroy();
	}
}

void AS1BaseProjectile::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (GetWorld() == nullptr)
	{
		return;
	}
	if (GetWorld()->GetTimerManager().TimerExists(DestroyDistanceHandler))
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroyDistanceHandler);
	}
	if (GetWorld()->GetTimerManager().TimerExists(DestroyTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroyTimerHandle);
	}
}

void AS1BaseProjectile::Destroy()
{
	Super::Destroy();
}

void AS1BaseProjectile::OnSphereOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OwnerPlayer->IsMyPlayer())
	{
		return;
	}
	AS1Player* other = Cast<AS1Player>(OtherActor);
	if (other == nullptr)
	{
		return;
	}
    if (other == OwnerPlayer)
    {
	    return;
    }
	/** Create Packet And Send It */
	Protocol::C_REQ_ATTACK AttackPkt;
	{
		AttackPkt.set_target_id(other->GetPlayerInfo()->object_id());
		AttackPkt.set_damage(ProjectileData->Data.Damage);
		AttackPkt.set_bullet_id(ObjectId);
		Protocol::PosInfo* attackerPos = AttackPkt.mutable_attacker_pos_info();
		attackerPos->CopyFrom(*OwnerPlayer->GetPlayerInfo());
	}
	
	GetGameInstance()->GetSubsystem<US1NetworkManager>()->SendPacket(AttackPkt);
}
