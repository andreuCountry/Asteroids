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
    REGISTER_MENU,
    GAMEPLAY
};

struct Collider{
    mlib::Vec2 P1 = {0.0f,0.0f};
    mlib::Vec2 P2 = {16.0f ,16.0f};
};

struct Ship {
    int lifes = 3;
    bool isReloading = false;
    bool isAlive = true;
    Collider collider;
};

struct Asteroid {
    Collider collider;
    AsteroidType type;
};

struct Game {
    Scenes actualScene;
};

#endif