#include "S1NetworkManager.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketSubsystem.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "ClientPacketHandler.h"
#include "S1GameInstance.h"
#include "Character/S1MyPlayer.h"
#include "Kismet/GameplayStatics.h"
#include <Weapon/S1BaseProjectile.h>

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

class UEnhancedInputLocalPlayerSubsystem;

void US1NetworkManager::ConnectToGameServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address Ip;
	FIPv4Address::Parse(IpAddress, Ip);

	TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(Ip.Value);
	InternetAddr->SetPort(Port);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

	bool Connected = Socket->Connect(*InternetAddr);

	if (Connected)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

		// Session
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();

		// TEMP : Lobby���� ĳ���� ����â ��
		{
			Protocol::C_LOGIN Pkt;
			SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(Pkt);
			SendPacket(SendBuffer);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
	}
}

void US1NetworkManager::DisconnectFromGameServer()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	Protocol::C_LEAVE_GAME LeavePkt;
	SendPacket(LeavePkt);

	/*if (Socket)
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}*/
}

void US1NetworkManager::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void US1NetworkManager::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void US1NetworkManager::HandleSpawn(const Protocol::ObjectInfo& ObjectInfo, bool IsMine)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	// �ߺ� ó�� üũ
	const uint64 ObjectId = ObjectInfo.object_id();
	if (Players.Find(ObjectId) != nullptr)
		return;

	FVector SpawnLocation(ObjectInfo.pos_info().x(), ObjectInfo.pos_info().y(), ObjectInfo.pos_info().z());

	if (IsMine)
	{
		//TArray<UUserWidget*> FoundWidgets;
		//UWidgetBlueprintLibrary::GetAllWidgetsOfClass(World, FoundWidgets, UUserWidget::StaticClass(), true);
		//
		//// 이름으로 검색
		//for (UUserWidget* Widget : FoundWidgets)
		//{
		//	if (Widget && Widget->GetName() == "Log")
		//	{
		//		// 뷰포트에서 제거
		//		Widget->RemoveFromParent();
		//		UE_LOG(LogTemp, Log, TEXT("Removed widget: %s"), *Widget->GetName());
		//		return; // 첫 번째 매치된 위젯만 삭제
		//	}
		//}

		if(LoginWidget != nullptr)
		{
			LoginWidget->RemoveFromViewport();
		}

		auto* PC = UGameplayStatics::GetPlayerController(this, 0);
		//
		//AS1Player* Player = Cast<AS1Player>(PC->GetPawn());
		AS1MyPlayer* Player = Cast<AS1MyPlayer>(World->SpawnActor(Cast<US1GameInstance>(GetGameInstance())->MyPlayerClass, &SpawnLocation));
		if (Player == nullptr)
			return;

		Player->SetPlayerInfo(ObjectInfo.pos_info());
		MyPlayer = Player;
		Players.Add(ObjectInfo.object_id(), Player);
		PC->EnableInput(PC);
		PC->Possess(MyPlayer);
		PC->SetInputMode(FInputModeGameOnly());
		//Player->()
	}
	else
	{
		AS1Player* Player = Cast<AS1Player>(World->SpawnActor(Cast<US1GameInstance>(GetGameInstance())->OtherPlayerClass, &SpawnLocation));
		Player->SetPlayerInfo(ObjectInfo.pos_info());
		Players.Add(ObjectInfo.object_id(), Player);
	}
}

void US1NetworkManager::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player(), true);
}

void US1NetworkManager::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (auto& Player : SpawnPkt.players())
	{
		HandleSpawn(Player, false);
	}
}

void US1NetworkManager::HandleDespawn(uint64 ObjectId)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	AS1Player** FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	World->DestroyActor(*FindActor);
}

void US1NetworkManager::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
	for (auto& ObjectId : DespawnPkt.object_ids())
	{
		HandleDespawn(ObjectId);
	}
}

void US1NetworkManager::HandleMove(const Protocol::S_MOVE& MovePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = MovePkt.info().object_id();
	AS1Player** FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	AS1Player* Player = (*FindActor);
	if (Player->IsMyPlayer())
		return;

	const Protocol::PosInfo& Info = MovePkt.info();
	Player->SetDestInfoDirectly(Info);
}

void US1NetworkManager::HandleResMove(const Protocol::S_RES_MOVE& MovePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = MovePkt.info().object_id();
	AS1Player** FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	AS1Player* Player = (*FindActor);

	const Protocol::PosInfo& Info = MovePkt.info();
	Player->SetDestInfo(Info);
	Player->MoveToTargetLocation();
}

void US1NetworkManager::HandleResShoot(const Protocol::S_RES_SHOOT& ShootPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectId = ShootPkt.character_pos_info().object_id();
	AS1Player** FindActor = Players.Find(ObjectId);
	if (FindActor == nullptr)
		return;

	AS1Player* Player = (*FindActor);
	//if (Player->IsMyPlayer())
	//	return;

	const Protocol::PosInfo& startPoint = ShootPkt.character_pos_info();
	const Protocol::PosInfo& attackPoint = ShootPkt.attack_point_info();

	FVector StartLocation = FVector(startPoint.x(), startPoint.y(), startPoint.z());
	FVector TargetLocation = FVector(attackPoint.x(), attackPoint.y(), attackPoint.z());
	
	Player->Shoot(StartLocation, TargetLocation, ShootPkt.bullet_info());
}

void US1NetworkManager::HandleResAttack(const Protocol::S_RES_ATTACK& ShootPkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	auto* World = GetWorld();
	if (World == nullptr)
		return;

	if (!ShootPkt.success())
	{
		return;
	}
	
	AS1Player* TargetPlayer = FindPlayer(ShootPkt.target_id());
	if (TargetPlayer == nullptr)
	{
		return;
	}

	AS1Player* AttackerPlayer = FindPlayer(ShootPkt.attacker_id());
	if (AttackerPlayer == nullptr)
	{
		return;
	}

	TargetPlayer->TakeDamage(AttackerPlayer, ShootPkt.damage());
	if (Objects.Find(ShootPkt.bullet_id()))
	{
		AS1Actor* BulletActor = Objects[ShootPkt.bullet_id()];

		if (BulletActor != nullptr && BulletActor->IsValidLowLevel())
		{
			BulletActor->Destroy();
		}
	}
	
}

void US1NetworkManager::SendLoginPacket(FString id, FString password)
{
	std::string utf8_id = TCHAR_TO_UTF8(*id);
	std::string utf8_password = TCHAR_TO_UTF8(*password);

	// 패킷에 데이터 추가
	Protocol::C_REQ_LOGIN pkt;
	pkt.set_userid(utf8_id);
	pkt.set_password(utf8_password);


	// 서버로 패킷 전송
	SendPacket(pkt);
}


void US1NetworkManager::DeleteAllPlayer(const Protocol::S_LEAVE_GAME& LeavePkt)
{
	if (Players.Num() == 0) // 맵이 비어있는지 확인
		return;

	// 월드 가져오기
	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	// 모든 플레이어 액터 제거
	for (auto& Pair : Players)
	{
		AS1Player* Player = Pair.Value;
		if (Player && Player->IsValidLowLevel()) // 유효한 액터인지 확인
		{
			World->DestroyActor(Player); // 월드에서 액터 제거
		}
	}

	// TMap 비우기
	Players.Empty();
}

AS1Player* US1NetworkManager::FindPlayer(uint64 findID)
{
	AS1Player** FindActor = Players.Find(findID);
	if (FindActor == nullptr)
		return nullptr;

	AS1Player* TargetPlayer = (*FindActor);
	return TargetPlayer;
}
