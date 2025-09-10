#include "pch.h"
#include "LoginManager.h"
#include <bcrypt.h>
#include <random>
#include <iostream>

#pragma comment(lib, "bcrypt.lib") // bcrypt 라이브러리 링크
bool LoginManager::RegisterUser(int playerID, const std::wstring& username, const std::wstring& password)
{
	try
	{
		if (IsUsernameTaken(username))
		{
			std::wcout << "Username is already taken: " << username << std::endl;
			return false;
		}



		std::wstring salt = GenerateSalt();
		std::wstring passwordHash = HashPassword(password, salt);

		DBConnection* dbConn = GDBConnectionPool->Pop();
		if (!dbConn)
		{
			std::wcerr << L"Failed to acquire DB connection." << std::endl;
			return false;
		}

		// 쿼리에서 playerID 열 제거 (자동 생성)
		const WCHAR* query = L"INSERT INTO PlayerAccounts (Username, Passwordhash, Salt) VALUES (?, ?, ?)";
		DBBind<3, 0> binder(*dbConn, query);
		binder.BindParam(0, username.c_str());
		binder.BindParam(1, passwordHash.c_str());
		binder.BindParam(2, salt.c_str());

		if (!binder.Execute())
		{
			GDBConnectionPool->Push(dbConn);
			std::wcerr << L"Failed to execute insert query." << std::endl;
			return false;
		}

		GDBConnectionPool->Push(dbConn);
		return true;
	}
	catch (const std::exception& e)
	{
		std::wcerr << L"Error: " << e.what() << std::endl;
		return false;
	}
}

bool LoginManager::AuthenticateUser(const std::wstring& username, const std::wstring& password)
{

	return IsUsernameTaken(username);

	try
	{
		// DB 연결 풀에서 DBConnection 가져오기
		DBConnection* dbConn = GDBConnectionPool->Pop();
		if (!dbConn)
		{
			std::wcerr << L"Failed to acquire a valid DB connection." << std::endl;
			return false;
		}

		// 쿼리 작성
		const WCHAR* query = L"SELECT Passwordhash, Salt FROM PlayerAccounts WHERE Username = ?";
		DBBind<1, 2> binder(*dbConn, query);

		// username 파라미터 바인딩
		binder.BindParam(0, username.c_str());


		
		// 쿼리 실행
		if (!binder.Execute() || !binder.Fetch())
		{
			std::wcerr << L"Failed to fetch user data for authentication." << std::endl;
			GDBConnectionPool->Push(dbConn);
			return false;
		}

		// 결과 컬럼을 저장할 변수
		std::wstring storedHash;
		std::wstring storedSalt;

		// 컬럼 바인딩
		binder.BindCol(0, storedHash); // 첫 번째 컬럼: Passwordhash
		binder.BindCol(1, storedSalt); // 두 번째 컬럼: Salt


		GDBConnectionPool->Push(dbConn); // DB 연결 반환

		// 입력받은 비밀번호와 DB의 비밀번호 비교
		if (VerifyPassword(password, storedHash, storedSalt))
		{
			std::wcout << L"Authentication successful for user: " << username << std::endl;
			return true;
		}
		else
		{
			std::wcout << L"Authentication failed for user: " << username << std::endl;
			return false;
		}
	}
	catch (const std::exception& e)
	{
		std::wcerr << L"Exception during authentication: " << e.what() << std::endl;
		return false;
	}
}


std::wstring LoginManager::HashPassword(const std::wstring& password, const std::wstring& salt)
{
	BCRYPT_ALG_HANDLE hAlgorithm = nullptr;
	NTSTATUS status;
	std::vector<BYTE> hashBuffer(BCRYPT_HASHSIZE);
	DWORD resultLength = 0;

	// SHA256 알고리즘 핸들 열기
	status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
	if (status != 0)
	{
		throw std::runtime_error("Failed to open algorithm provider.");
	}

	// 비밀번호와 솔트를 결합
	std::wstring saltedPassword = password + salt;

	// 해싱
	status = BCryptHash(hAlgorithm, nullptr, 0, (PUCHAR)saltedPassword.c_str(), saltedPassword.size() * sizeof(wchar_t), hashBuffer.data(), hashBuffer.size());
	if (status != 0)
	{
		BCryptCloseAlgorithmProvider(hAlgorithm, 0);
		throw std::runtime_error("Failed to hash the password.");
	}

	BCryptCloseAlgorithmProvider(hAlgorithm, 0);

	// 해시 값을 문자열로 변환
	return BytesToHex(hashBuffer);
}

bool LoginManager::VerifyPassword(const std::wstring& password, const std::wstring& hashedPassword, const std::wstring& salt)
{
	std::wstring newHash = HashPassword(password, salt);
	return newHash == hashedPassword;
}

std::wstring LoginManager::BytesToHex(const std::vector<BYTE>& bytes)
{
	static const wchar_t hexChars[] = L"0123456789ABCDEF";
	std::wstring hexString;

	for (BYTE b : bytes)
	{
		hexString.push_back(hexChars[b >> 4]);
		hexString.push_back(hexChars[b & 0x0F]);
	}

	return hexString;
}

std::wstring LoginManager::GenerateSalt()
{
	const int saltLength = 16; // 솔트 길이
	static const wchar_t characters[] =
		L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		L"abcdefghijklmnopqrstuvwxyz"
		L"0123456789";
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, sizeof(characters) / sizeof(wchar_t) - 2);

	std::wstring salt;
	for (int i = 0; i < saltLength; ++i)
	{
		salt += characters[dis(gen)];
	}

	return salt;
}

bool LoginManager::IsUsernameTaken(const std::wstring& username)
{
	try
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		if (!dbConn)
		{
			std::wcerr << L"Failed to acquire DB connection." << std::endl;
			return true; // 연결 실패 시 중복으로 간주
		}

		const WCHAR* query = L"SELECT COUNT(*) FROM PlayerAccounts WHERE username = ?";
		DBBind<1, 1> binder(*dbConn, query);
		binder.BindParam(0, username.c_str());

		int count = 0;
		binder.BindCol(0, count);

		if (!binder.Execute() || !binder.Fetch())
		{
			GDBConnectionPool->Push(dbConn);
			std::wcerr << L"Failed to check username existence." << std::endl;
			return true; // 중복으로 간주
		}

		GDBConnectionPool->Push(dbConn);
		return count > 0; // 중복 여부 반환
	}
	catch (const std::exception& e)
	{
		std::wcerr << L"Error: " << e.what() << std::endl;
		return true; // 에러 발생 시 중복으로 간주
	}
}
