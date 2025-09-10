// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1Player.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AIController.h"
#include "S1NetworkManager.h"
#include "S1MyPlayer.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "UI/S1HpBarWidget.h"
#include "UI/S1WidgetComponent.h"
#include "Weapon/S1WeaponComponent.h"

AS1Player::AS1Player()
{
	WeaponComponent = CreateDefaultSubobject<US1WeaponComponent>(TEXT("WeaponComponent"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetCharacterMovement()->bRunPhysicsWithNoController = true;

	PlayerInfo = new Protocol::PosInfo();
	DestInfo = new Protocol::PosInfo();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	StatComponent = CreateDefaultSubobject<US1EntityStatComponent>(TEXT("StatComponent"));
	StatComponent->OnHpZero.AddUObject(this, &AS1Player::SetDead);

	HpBar = CreateDefaultSubobject<US1WidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/S1/UI/WBP_HpBar.WBP_HpBar_C"));

	if (HpBarWidgetRef.Class != nullptr)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

AS1Player::~AS1Player()
{
	delete PlayerInfo;
	delete DestInfo;
	PlayerInfo = nullptr;
	DestInfo = nullptr;
}

void AS1Player::BeginPlay()
{
	Super::BeginPlay();
	
	check(StatComponent != nullptr);
	check(CharacterInitalizeStatData != nullptr);	
	StatComponent->SetBaseStat(CharacterInitalizeStatData->Stat);
	{
		FVector Location = GetActorLocation();
		DestInfo->set_x(Location.X);
		DestInfo->set_y(Location.Y);
		DestInfo->set_z(Location.Z);

		SetMoveState(Protocol::MOVE_STATE_IDLE);
	}
}

void AS1Player::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetMoveState() == Protocol::MOVE_STATE_RUN)
	{
		FVector Location = GetActorLocation();
		PlayerInfo->set_x(Location.X);
		PlayerInfo->set_y(Location.Y);
		PlayerInfo->set_z(Location.Z);

		CheckArrival();
	}
}


US1NetworkManager* AS1Player::GetNetworkManager() const
{
	return GetGameInstance()->GetSubsystem<US1NetworkManager>();
}


bool AS1Player::IsMyPlayer()
{
	return Cast<AS1MyPlayer>(this) != nullptr;
}

void AS1Player::SetMoveState(Protocol::MoveState State)
{
	if (PlayerInfo->state() == State)
		return;

	PlayerInfo->set_state(State);

	// TODO
}

void AS1Player::SetPlayerInfo(const Protocol::PosInfo& Info)
{
	if (PlayerInfo->object_id() != 0)
	{
		assert(PlayerInfo->object_id() == Info.object_id());
	}

	PlayerInfo->CopyFrom(Info);

	FVector Location(Info.x(), Info.y(), Info.z());
	SetActorLocation(Location);
}

void AS1Player::SetDestInfo(const Protocol::PosInfo& Info)
{
	if (PlayerInfo->object_id() != 0)
	{
		assert(PlayerInfo->object_id() == Info.object_id());
	}

	// Dest�� ���� ���� ����.
	DestInfo->CopyFrom(Info);

	// ���¸� �ٷ� ��������.
	SetMoveState(Info.state());
}

void AS1Player::SetDestInfoDirectly(const Protocol::PosInfo& Info)
{
	SetDestInfo(Info);
	SetActorLocation(FVector(DestInfo->x(), DestInfo->y(), DestInfo->z()));
}

void AS1Player::MoveToTargetLocation()
{
	AController* MyController = GetController();
	if (!MyController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is null for %s"), *GetName());
		return;
	}

	FVector TargetLocation(DestInfo->x(), DestInfo->y(), DestInfo->z());

	// ��Ʈ�ѷ� Ÿ�Կ� ���� �̵� ó��
	if (Cast<APlayerController>(MyController))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(MyController, TargetLocation);
	}
	else if (Cast<AAIController>(MyController))
	{
		AAIController* AICont = Cast<AAIController>(MyController);
		if (AICont != nullptr)
		{
			AICont->MoveToLocation(TargetLocation);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unsupported Controller type for %s"), *GetName());
	}

	//FString str = FString::Printf(TEXT("Actor Location: %s, Target Location: %s"), *GetActorLocation().ToString(), *TargetLocation.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, *str);

	// �̵� ���� ����
	SetMoveState(Protocol::MOVE_STATE_RUN);
}

void AS1Player::Shoot(FVector StartLocation, FVector TargetLocation, const Protocol::ObjectInfo& bullet_info)
{
	FVector FireDirection = (TargetLocation - StartLocation).GetSafeNormal();
	FRotator FireRotation = FireDirection.Rotation();
	SetActorRotation(FireRotation);
	
	check(WeaponComponent != nullptr);
	WeaponComponent->FireWeapon(StartLocation, TargetLocation, this, bullet_info);
}

void AS1Player::CheckArrival()
{
	float DistanceToTarget = FVector::Dist(FVector(PlayerInfo->x(), PlayerInfo->y(), PlayerInfo->z()), FVector(DestInfo->x(), DestInfo->y(), DestInfo->z()));

	if (DistanceToTarget <= AcceptanceRadius)
	{
		OnArrival();
	}
}

void AS1Player::OnArrival()
{
	SetMoveState(Protocol::MOVE_STATE_IDLE);
	SetActorLocation(FVector(DestInfo->x(), DestInfo->y(), DestInfo->z()));

	FVector Location = GetActorLocation();
	PlayerInfo->set_x(Location.X);
	PlayerInfo->set_y(Location.Y);
	PlayerInfo->set_z(Location.Z);

	/** Create Packet And Send It */
	Protocol::C_MOVE MovePkt;
	{
		Protocol::PosInfo* Info = MovePkt.mutable_info();
		Info->CopyFrom(*PlayerInfo);
		Info->set_state(GetMoveState());
		Info->set_x(Location.X);
		Info->set_y(Location.Y);
		Info->set_z(Location.Z);
	}

	GetNetworkManager()->SendPacket(MovePkt);
}

void AS1Player::TakeDamage(AActor* from, float damage)
{
	StatComponent->ApplyDamage(damage);
}

void AS1Player::SetupCharacterWidget(US1UserWidget* InUserWidget)
{
	US1HpBarWidget* HpBarWidget = Cast<US1HpBarWidget>(InUserWidget);
	if (HpBarWidget != nullptr)
	{
		HpBarWidget->UpdateStat(StatComponent->GetBaseStat(), StatComponent->GetModifierStat());
		HpBarWidget->UpdateHpBar(StatComponent->GetCurrentHp());
		StatComponent->OnHpChanged.AddUObject(HpBarWidget, &US1HpBarWidget::UpdateHpBar);
		StatComponent->OnStatChanged.AddUObject(HpBarWidget, &US1HpBarWidget::UpdateStat);
	}
}

void AS1Player::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	
	PlayDeadAnimation();
	
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void AS1Player::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	// AnimInstance->Montage_Play(DeadMontage, 1.0f);
}