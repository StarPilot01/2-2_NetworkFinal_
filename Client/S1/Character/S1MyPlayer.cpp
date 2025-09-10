// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/S1MyPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "S1.h"
#include "S1NetworkManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "NavigationSystem.h"

AS1MyPlayer::AS1MyPlayer()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	//AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AS1MyPlayer::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AS1MyPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &AS1MyPlayer::OnMoveMouseClick);

		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AS1MyPlayer::OnAttackMouseClick);
	}

}


void AS1MyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AS1MyPlayer::ReqEnterRoom(int32 id)
{
	Protocol::C_REQ_ENTER_ROOM enterRoomPkt;
	enterRoomPkt.set_room_id(id);




	GetNetworkManager()->SendPacket(enterRoomPkt);

}

void AS1MyPlayer::OnMoveMouseClick(const FInputActionValue& Value)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	check(PlayerController != nullptr);
	
	FHitResult HitResult;
	if (!PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return;
	}

	/** Create Packet And Send It */
	Protocol::C_REQ_MOVE MovePkt;
	{
		Protocol::PosInfo* Info = MovePkt.mutable_info();
		Info->CopyFrom(*PlayerInfo);
		Info->set_state(GetMoveState());
		Info->set_x(HitResult.ImpactPoint.X);
		Info->set_y(HitResult.ImpactPoint.Y);
		Info->set_z(HitResult.ImpactPoint.Z);
	}

	GetNetworkManager()->SendPacket(MovePkt);
}

void AS1MyPlayer::OnAttackMouseClick(const FInputActionValue& Value)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	check(PlayerController != nullptr);

	FHitResult HitResult;
	if (!PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return;
	}



	/** Create Packet And Send It */
	Protocol::C_REQ_SHOOT AttackPkt;
	{
		const FVector ForwardVector = GetActorForwardVector(); // Forward 방향
		const float OffsetDistance = 500.0f; // 원하는 offset 거리
		const FVector OffsetPosition = GetActorLocation() + (ForwardVector * OffsetDistance);


		Protocol::PosInfo* AttackPointInfo = AttackPkt.mutable_attack_point_info();
		AttackPointInfo->CopyFrom(*PlayerInfo);
		AttackPointInfo->set_state(GetMoveState());
		AttackPointInfo->set_x(HitResult.ImpactPoint.X);
		AttackPointInfo->set_y(HitResult.ImpactPoint.Y);
		AttackPointInfo->set_z(HitResult.ImpactPoint.Z);
		//AttackPointInfo->set_x(OffsetPosition.X);
		//AttackPointInfo->set_y(OffsetPosition.Y);
		//AttackPointInfo->set_z(OffsetPosition.Z);


		Protocol::PosInfo* CharacterPointInfo = AttackPkt.mutable_character_pos_info();
		CharacterPointInfo->CopyFrom(*PlayerInfo);
		CharacterPointInfo->set_state(GetMoveState());
		FVector CharacterPos = GetActorLocation();
		CharacterPointInfo->set_x(CharacterPos.X);
		CharacterPointInfo->set_y(CharacterPos.Y);
		CharacterPointInfo->set_z(CharacterPos.Z);
	}

	GetNetworkManager()->SendPacket(AttackPkt);
}