#include "pch.h"
#include "RoomManager.h"

RoomManagerRef GRoomManager = std::make_shared<RoomManager>();

RoomManager::RoomManager()
	: nextRoomId(1) // Room ID 초기화
{

}

RoomManager::~RoomManager()
{
	mRooms.clear();
}

RoomRef RoomManager::CreateRoom()
{
	// 새로운 Room 생성
	RoomRef newRoom = std::make_shared<Room>();
	mRooms[nextRoomId] = newRoom;
	nextRoomId++; // 다음 Room ID 증가
	return newRoom;
}

void RoomManager::UpdateAllRooms()
{
	for (auto& pair : mRooms)
	{
		RoomRef room = pair.second;
		if (room)
			room->DoAsync(&Room::UpdateTick);
	}
}

bool RoomManager::DeleteRoom(uint64 roomId)
{
	auto it = mRooms.find(roomId);
	if (it == mRooms.end())
		return false;

	mRooms.erase(it);
	return true;
}

RoomRef RoomManager::GetRoom(uint64 roomId)
{
	auto it = mRooms.find(roomId);
	if (it != mRooms.end())
		return it->second;

	return nullptr;
}

bool RoomManager::RelocatePlayerToRoom(PlayerRef player, RoomRef fromRoom, RoomRef toRoom)
{
	// 출발 Room과 도착 Room 확인
	//RoomRef fromRoom = GetRoom(fromRoomId);
	//RoomRef toRoom = GetRoom(toRoomId);

	if (!fromRoom || !toRoom)
		return false;

	// 기존 Room에서 나가고 새로운 Room으로 들어가기
	if (fromRoom->HandleLeavePlayer(player))
	{
		return toRoom->HandleEnterPlayer(player);
	}

	return false;
}
