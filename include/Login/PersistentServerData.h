#pragma  once
#include "Login/LoginTypes.h"
#include <string>
#include <unordered_map>
struct PersistentServerData{
    std::uint64_t s;
    std::unordered_map<USER_LOGIN_KEY_TYPE, USER_ID_TYPE> userLogin;
    std::unordered_map<USER_ID_TYPE, std::string> userNames;
//    optional std::unordered_map<USER_ID_TYPE, CharacterType>
};