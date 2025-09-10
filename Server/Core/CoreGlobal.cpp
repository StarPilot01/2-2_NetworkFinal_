#include "pch.h"
#include "CoreGlobal.h"
#include "pch.h"
#include "CoreGlobal.h"

#include "DBConnectionPool.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "LoginManager.h"

ThreadManager* GThreadManager = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;
DBConnectionPool* GDBConnectionPool = nullptr;
LoginManager* GLoginManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GGlobalQueue = new GlobalQueue();
		GJobTimer = new JobTimer();
		GDBConnectionPool = new DBConnectionPool();
		GLoginManager = new LoginManager();

		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GGlobalQueue;
		delete GJobTimer;
		delete GDBConnectionPool;
		delete GLoginManager;

		SocketUtils::Clear();
	}
} GCoreGlobal;