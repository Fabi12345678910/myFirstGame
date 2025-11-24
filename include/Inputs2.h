#pragma once
#include "Types.h"

//OBJECT_ID_TYPE playerInput;

struct playerInput{
    bool moveLeft;
    bool moveRight;
    bool jump;
};

struct playerInputWithId{
    OBJECT_ID_TYPE playerId;
    struct playerInput playerInput;
};

struct allPlayerInputs{
    int ammountInputs;
    struct playerInputWithId *playerInputs;
};