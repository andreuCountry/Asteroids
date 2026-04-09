#include "mathlib.h"
#include <esat/draw.h>
#include <esat/sprite.h>

#ifndef STRUCTS_H
#define STRUCTS_H

enum AsteroidType {

};

enum Scenes {
    MAIN_MENU,
    HIGHSCORES,
    ADMIN_SECTION,
    EDIT_SECTION,
    ASK_REGISTER,
    LOAD_REGISTER,
    REGISTER_MENU,
    GAMEPLAY
};

struct Collider{
    mlib::Vec2 P1 = {0.0f,0.0f};
    mlib::Vec2 P2 = {16.0f ,16.0f};
};

struct Asteroid {
    Collider collider;
    AsteroidType type;
};

struct Game {
    Scenes actualScene;
};

#endif
