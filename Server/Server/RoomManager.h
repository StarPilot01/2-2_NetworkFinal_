#pragma once

#include "Room.h"

class RoomManager;

using RoomManagerRef = std::shared_ptr<RoomManager>;

class RoomManager
{
public:
	RoomManager();
	~RoomManager();

	// Room 추가
	RoomRef CreateRoom();
	void UpdateAllRooms();
	// Room 삭제
	bool DeleteRoom(uint64 roomId);

	// 특정 Room 가져오기
	RoomRef GetRoom(uint64 roomId);

	// 플레이어 이동
	bool RelocatePlayerToRoom(PlayerRef player, RoomRef fromRoom, RoomRef toRoom);

private:
	std::unordered_map<uint64, RoomRef> mRooms; // Room ID -> Room 객체 매핑
	uint64 nextRoomId; // 고유 Room ID 생성기
};

extern RoomManagerRef GRoomManager;
