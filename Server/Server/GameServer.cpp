#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
//#include "ClientPacketHandler.h"
#include <tchar.h>
#include <bcrypt.h>

#include "DBConnection.h"
#include "DBBind.h"
#include "Job.h"
#include "Protocol.pb.h"
#include "Room.h"
#include "RoomManager.h"

#include "DBConnectionPool.h"
#include "LoginManager.h"

#pragma comment(lib, "bcrypt.lib")

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{



	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=localhost\\SQLEXPRESS;Database=S1_DB;Trusted_Connection=Yes;Connection Timeout=5;"));


	// 회원가입 테스트
	//{
	//	int playerID = 1;
	//	std::wstring username = L"TestUser";
	//	std::wstring password = L"TestPassword44";
	//
	//	if (GLoginManager->RegisterUser(playerID, username, password))
	//	{
	//		std::cout << "회원가입 성공!\n";
	//	}
	//	else
	//	{
	//		std::cout << "회원가입 실패.\n";
	//	}
	//}
	//{
	//	int playerID = 2;
	//	std::wstring username = L"TestUser2";
	//	std::wstring password = L"TestPasswor44d";
	//
	//	if (GLoginManager->RegisterUser(playerID, username, password))
	//	{
	//		std::cout << "회원가입 성공!\n";
	//	}
	//	else
	//	{
	//		std::cout << "회원가입 실패.\n";
	//	}
	//}
	//{
	//	int playerID = 3;
	//	std::wstring username = L"TestUser3";
	//	std::wstring password = L"Te232stPassword";
	//
	//	if (GLoginManager->RegisterUser(playerID, username, password))
	//	{
	//		std::cout << "회원가입 성공!\n";
	//	}
	//	else
	//	{
	//		std::cout << "회원가입 실패.\n";
	//	}
	//}

	// 로그인 테스트
	//{
	//	std::wstring username = L"TestUser";
	//	std::wstring password = L"TestPassword";
	//
	//	if (loginManager.AuthenticateUser(username, password))
	//	{
	//		std::cout << "로그인 성공!\n";
	//	}
	//	else
	//	{
	//		std::cout << "로그인 실패.\n";
	//	}
	//}

	// Create Table
	//{
	//	auto query = L"									\
	//		DROP TABLE IF EXISTS [dbo].[Gold];			\
	//		CREATE TABLE [dbo].[Gold]					\
	//		(											\
	//			[id] INT NOT NULL PRIMARY KEY IDENTITY, \
	//			[gold] INT NULL,						\
	//			[name] NVARCHAR(50) NULL,				\
	//			[createDate] DATETIME NULL				\
	//		);";
	//
	//	DBConnection* dbConn = GDBConnectionPool->Pop();
	//	ASSERT_CRASH(dbConn->Execute(query));
	//	GDBConnectionPool->Push(dbConn);
	//}




	ServerPacketHandler::Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[=]() { return make_shared<GameSession>(); }, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
				
			});
	}

	for (int i = 0; i < 10; i++) // 10개의 Room 생성
	{
		GRoomManager->CreateRoom();
	}


	GThreadManager->Launch([]()
		{
			while (true)
			{
				GRoomManager->UpdateAllRooms();
				this_thread::sleep_for(0.1s);
			}
		});
	// Main Thread
	//DoWorkerJob(service);

	//GRoom1->DoAsync(&Room::UpdateTick);

	while (true)
	{
		//Protocol::S_CHAT pkt;
		//pkt.set_msg("HelloWorld");
		//auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		//GSessionManager.Broadcast(sendBuffer);
		this_thread::sleep_for(0.1s);
		
	}

	GThreadManager->Join();
}