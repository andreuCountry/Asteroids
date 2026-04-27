#include "mathlib.h"
#include <esat/draw.h>
#include <esat/sprite.h>

#ifndef STRUCTS_H
#define STRUCTS_H

enum Scenes {
    MAIN_MENU,
    HIGHSCORES,
    ADMIN_SECTION,
    EDIT_SECTION,
    ASK_REGISTER,
    LOAD_REGISTER,
    REGISTER_MENU,
    ASK_GAMEPLAY,
    ASK_SECOND_LOGIN,
    GAMEPLAY
};

struct Game {
    Scenes actualScene;
};

#endif
