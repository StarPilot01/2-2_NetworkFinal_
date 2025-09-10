#pragma once

#include "CoreMinimal.h"
#include "ClientPacketHandler.h"
#include "PacketSession.h"
#include "Protocol.pb.h"
#include "S1.h"
#include "Character/S1Actor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "S1NetworkManager.generated.h"

class AS1Player;

UCLASS()
class S1_API US1NetworkManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	template<typename T>
	void SendPacket(T packet) const;
	template <class T>
	T* Create(UWorld* World, TSubclassOf<T> staticClass, uint64 ObjectId);

public:
	void HandleSpawn(const Protocol::ObjectInfo& PlayerInfo, bool IsMine);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectId);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

	void HandleMove(const Protocol::S_MOVE& MovePkt);
	void HandleResMove(const Protocol::S_RES_MOVE& MovePkt);
	void HandleResShoot(const Protocol::S_RES_SHOOT& ShootPkt);
	void HandleResAttack(const Protocol::S_RES_ATTACK& ShootPkt);

public:
	UFUNCTION(BlueprintCallable)
	void SendLoginPacket(FString id, FString password);
	void DeleteAllPlayer(const Protocol::S_LEAVE_GAME& LeavePkt);
public:
	// GameServer
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;

	TSharedPtr<class PacketSession> GameServerSession;
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* LoginWidget;

public:
	UPROPERTY()
	TMap<uint64, AS1Player*> Players;

	// TODO: 오브젝트의 아이디와, 모든 언리얼 오브젝트들을 상속받은 클래스 맵.
	UPROPERTY()
	TMap<uint64, AS1Actor*> Objects; // 서버에서 생성한 오브젝트들

	
	UPROPERTY()
	TObjectPtr<AS1Player> MyPlayer;

protected:
	AS1Player* FindPlayer(uint64 findID);
};


/**
 * <��Ŷ����> ������ ��Ŷ�� �����ϴ� ���� ��û�ϴ� �Լ� �Դϴ�.
 * ��û�� ��� �ݿ������� �ʽ��ϴ� SendPacketQueue�� �������ϰ� SendThread���� ������ �Ǹ� �����ִ� ����Դϴ�.
 */
template <typename T> 
void US1NetworkManager::SendPacket(T packet) const
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	const SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(packet);
	GameServerSession->SendPacket(SendBuffer);
}

template <class T>
T* US1NetworkManager::Create(UWorld* World, TSubclassOf<T> staticClass, uint64 ObjectId)
{
	static_assert(TIsDerivedFrom<T, AS1Actor>::Value, "AS1Actor을 상속받은 클래스만 허용");

	if (!World)
		return nullptr;

	T* NewActor = World->SpawnActor<T>(staticClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (NewActor)
	{
		NewActor->SetObjectId(ObjectId);
	}
	return NewActor;
}
