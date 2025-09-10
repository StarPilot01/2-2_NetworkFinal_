#include "pch.h"
#include "RoomManager.h"

RoomManagerRef GRoomManager = std::make_shared<RoomManager>();

RoomManager::RoomManager()
	: nextRoomId(1) // Room ID �ʱ�ȭ
{

}

RoomManager::~RoomManager()
{
	mRooms.clear();
}

RoomRef RoomManager::CreateRoom()
{
	// ���ο� Room ����
	RoomRef newRoom = std::make_shared<Room>();
	mRooms[nextRoomId] = newRoom;
	nextRoomId++; // ���� Room ID ����
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
	// ��� Room�� ���� Room Ȯ��
	//RoomRef fromRoom = GetRoom(fromRoomId);
	//RoomRef toRoom = GetRoom(toRoomId);

	if (!fromRoom || !toRoom)
		return false;

	// ���� Room���� ������ ���ο� Room���� ����
	if (fromRoom->HandleLeavePlayer(player))
	{
		return toRoom->HandleEnterPlayer(player);
	}

	return false;
}
