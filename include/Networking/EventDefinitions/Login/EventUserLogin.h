#pragma once

#include "Login/LoginTypes.h"
#include "Networking/EventTypeList.h"
#include "Networking/Templates/SingleVariableEvent.h"

using EventUserLogin = SingleVariableEvent<EVENT_TYPE_USER_LOGIN, USER_LOGIN_KEY_TYPE>;
