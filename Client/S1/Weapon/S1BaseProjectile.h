// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Struct.pb.h"
#include "Character/S1Actor.h"
#include "Character/S1Player.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DataAssets/S1ProjectileData.h"
#include "S1BaseProjectile.generated.h"

UCLASS()
class S1_API AS1BaseProjectile : public AS1Actor
{
	GENERATED_BODY()
	
public:	
	AS1BaseProjectile() : Super()
	{
		/* CDO */
		SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
		SphereCollision->InitSphereRadius(5.0f);
		SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		RootComponent = SphereCollision;

		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AS1BaseProjectile::OnSphereOverlap);
    
		MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
		MovementComponent->bAutoActivate = false;
		MovementComponent->ProjectileGravityScale = 0.f;
    
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
		Mesh->SetupAttachment(SphereCollision);

		/* Data */
		bIsInitialized = false;
	}

/* Lock */
protected:
	bool bIsInitialized;
	
/* Init Data */
public:
	void Initialize(const FVector& Start, const FVector& Target, AS1Player* InOwner, const Protocol::ObjectInfo& bullet_info);

/* Data Section */
protected:
	UPROPERTY()
	FVector StartLocation;
	UPROPERTY()
	FVector ShootDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<US1ProjectileData> ProjectileData;
	UPROPERTY()
	TObjectPtr<AS1Player> OwnerPlayer;
	
public:
	FORCEINLINE US1ProjectileData* GetProjectileData() { return ProjectileData; }

/* Shoot */
public:
	void Shoot();

protected:
	void CheckDistance();
	virtual void BeginDestroy() override;
	FTimerHandle DestroyDistanceHandler;
	FTimerHandle DestroyTimerHandle;

private:
	void Destroy();
	
/* Physics */
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereCollision;
public:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OnComponentBeginOverlap, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

/* Mesh */
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;
};
