#pragma once
#include "Login/PersistentServerDataProvider.h"
#include "Login/LoginTypes.h"
#include <string>

class PersistentServerDataProviderMock: public PersistentServerDataProvider{
public:
    virtual USER_ID_TYPE getUserId(USER_LOGIN_KEY_TYPE loginKey) override {return 2;};
    virtual std::string getUserName(USER_ID_TYPE userId) override {return "alfons";};
    virtual USER_LOGIN_KEY_TYPE createUser() override {return 1;};
    virtual USER_LOGIN_KEY_TYPE createUser(const std::string& userName)override {return 1;};
    virtual void renameUser(USER_ID_TYPE userId, const std::string& newUserName)override {return;};
    PersistentServerDataProviderMock(){};
};