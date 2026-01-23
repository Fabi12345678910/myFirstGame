#pragma once
#include "Login/LoginTypes.h"
#include <string>
#include "Login/Sqlite3Cpp.h"

class PersistentServerDataProvider{
public:
    virtual USER_ID_TYPE getUserId(USER_LOGIN_KEY_TYPE loginKey) = 0;
    virtual std::string getUserName(USER_ID_TYPE userId) = 0;
    virtual USER_LOGIN_KEY_TYPE createUser() = 0;
    virtual USER_LOGIN_KEY_TYPE createUser(const std::string& userName) = 0;
    virtual void renameUser(USER_ID_TYPE userId, const std::string& newUserName) = 0;
};