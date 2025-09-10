#include "pch.h"
#include "ServerPacketHandler.h"

#include <codecvt>
#include <locale>

#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"
#include "Room.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "GameSession.h"
#include "LoginManager.h"
#include "RoomManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	// TODO : DB에서 Account 정보 긁어온다
	// TODO : DB에서 유저 정보 긁어온다
	Protocol::S_LOGIN loginPkt;

	for (int32 i = 0; i < 3; i++)
	{
		Protocol::ObjectInfo* player = loginPkt.add_players();
		Protocol::PosInfo* posInfo = player->mutable_pos_info();
		posInfo->set_x(Utils::GetRandom(0.f, 100.f));
		posInfo->set_y(Utils::GetRandom(0.f, 100.f));
		posInfo->set_z(Utils::GetRandom(0.f, 100.f));
	}

	loginPkt.set_success(true);

	SEND_PACKET(loginPkt);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	// 플레이어 생성
	//PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session));
	//GRoomManager->GetRoom(1)->DoAsync(&Room::HandleEnterPlayer, player);


	// 방에 입장하면 무조건 룸1
	//GRoom1->DoAsync(&Room::HandleEnterPlayer, player);
	

	//GRoom1->HandleEnterPlayerLocked(player);

	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->HandleLeavePlayer(player);

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleMove, pkt);
	//room->HandleMove(pkt);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	return true;
}

bool Handle_C_REQ_MOVE(PacketSessionRef& session, Protocol::C_REQ_MOVE& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleREQMove, pkt);

	return true;
}

bool Handle_C_REQ_SHOOT(PacketSessionRef& session, Protocol::C_REQ_SHOOT& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);
	
	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock(); // 플레이어의 룸 정보를 가져온다.
	if (room == nullptr)
		return false;

	ObjectRef object = ObjectUtils::CreateObject(static_pointer_cast<GameSession>(session), room); // 해당 룸에서 생성.
	assert(object != nullptr);

	room = object->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleREQShoot, object, pkt);

	return true;
}

bool Handle_C_REQ_ATTACK(PacketSessionRef& session, Protocol::C_REQ_ATTACK& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->DoAsync(&Room::HandleREQAttack, pkt);


	return false;
}

bool Handle_C_REQ_ENTER_ROOM(PacketSessionRef& session, Protocol::C_REQ_ENTER_ROOM& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	return GRoomManager->RelocatePlayerToRoom(player, player->room.load().lock(), GRoomManager->GetRoom(pkt.room_id()));

	//RoomRef room = player->room.load().lock();
	//if (room == nullptr)
	//	return false;


	return true;
}

bool Handle_C_REQ_LOGIN(PacketSessionRef& session, Protocol::C_REQ_LOGIN& pkt)
{

	auto stringToWstring = [](const std::string& str) -> std::wstring {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
		return wstr;
		};
	 
	// userid와 password를 wstring으로 변환
	std::wstring userid = stringToWstring(pkt.userid());
	std::wstring password = stringToWstring(pkt.password());

	// AuthenticateUser 호출
	if(GLoginManager->AuthenticateUser(userid, password))
	{
		PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session));
		GRoomManager->GetRoom(1)->DoAsync(&Room::HandleEnterPlayer, player);
	}
	else
	{
		Protocol::S_DENY_LOGIN denyPkt;

		SEND_PACKET(denyPkt);
	}

	return true;
}
