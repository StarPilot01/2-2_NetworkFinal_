#pragma once

#include <string>
#include <vector>
#include "DBConnection.h"
#include "DBBind.h"
#include "DBConnectionPool.h"

#define BCRYPT_HASHSIZE 32


class LoginManager
{
public:
	static bool RegisterUser(int playerID, const std::wstring& username, const std::wstring& password);
	static bool AuthenticateUser(const std::wstring& username, const std::wstring& password);

private:
	static std::wstring HashPassword(const std::wstring& password, const std::wstring& salt);
	static bool VerifyPassword(const std::wstring& password, const std::wstring& hashedPassword, const std::wstring& salt);
	static std::wstring BytesToHex(const std::vector<BYTE>& bytes);
	static std::wstring GenerateSalt();
	static bool IsUsernameTaken(const std::wstring& username);
};
