#pragma once

#include "Login/LoginTypes.h"
#include "Networking/EventTypeList.h"
#include "Networking/Templates/SingleVariableEvent.h"

using EventUserCreationDenied = SingleVariableEvent<EVENT_TYPE_USER_CREATION_DENIED, USER_LOGIN_KEY_TYPE>;
