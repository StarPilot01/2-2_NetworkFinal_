#pragma once

#include "Room.h"

class RoomManager;

using RoomManagerRef = std::shared_ptr<RoomManager>;

class RoomManager
{
public:
	RoomManager();
	~RoomManager();

	// Room �߰�
	RoomRef CreateRoom();
	void UpdateAllRooms();
	// Room ����
	bool DeleteRoom(uint64 roomId);

	// Ư�� Room ��������
	RoomRef GetRoom(uint64 roomId);

	// �÷��̾� �̵�
	bool RelocatePlayerToRoom(PlayerRef player, RoomRef fromRoom, RoomRef toRoom);

private:
	std::unordered_map<uint64, RoomRef> mRooms; // Room ID -> Room ��ü ����
	uint64 nextRoomId; // ���� Room ID ������
};

extern RoomManagerRef GRoomManager;
