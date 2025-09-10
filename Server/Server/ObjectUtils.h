#pragma once

class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session);

	static ObjectRef CreateObject(GameSessionRef session, const RoomRef& room);

private:
	static atomic<int64> s_idGenerator;
};

