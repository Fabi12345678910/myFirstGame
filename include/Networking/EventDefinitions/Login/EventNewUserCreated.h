#pragma once

#include "Login/LoginTypes.h"
#include "Networking/EventTypeList.h"
#include "Networking/Templates/SingleVariableEvent.h"

using EventNewUserCreated = SingleVariableEvent<EVENT_TYPE_NEW_USER_CREATED, USER_LOGIN_KEY_TYPE>;
