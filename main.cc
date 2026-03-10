#include <stdio.h>
#include <stdlib.h>
#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include "structs.h"

double current_time, last_time;
int fps = 60;

float windowX = 800.0f, windowY = 608.0f;

esat::Vec3* points = (esat::Vec3*) malloc (5 * sizeof(esat::Vec3));

Game currentGame;

// Inicializar
void InitShip() {

}

void InitConfig() {
    esat::DrawSetTextFont("./resources/fonts/fuenteSI.ttf");
    esat::DrawSetTextSize(16);
    currentGame.actualScene = Scenes::MAIN_MENU;
}

void DrawMainMenu() {

}

void DrawHighscores() {

}

void DrawRegisterMenu() {

}

void DrawGameplay() {

}

int esat::main(int argc, char **argv) {

    InitConfig();
    esat::WindowInit(windowX, windowY);
    esat::WindowSetMouseVisibility(true);

    while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
        last_time = esat::Time();

        esat::DrawBegin();
        esat::DrawClear(0, 10, 0);

        // Draw scenes up
        switch (currentGame.actualScene) {
            case Scenes::MAIN_MENU:
                DrawMainMenu();
            break;
            case Scenes::HIGHSCORES:
                DrawHighscores();
            break;
            case Scenes::REGISTER_MENU:
                DrawRegisterMenu();
            break;
            case Scenes::GAMEPLAY:
                DrawGameplay();
            break;
        }

        esat::DrawEnd();

        do { current_time = esat::Time(); }
        while ((current_time - last_time) <= 1000.0 / fps);

        esat::WindowFrame();
    }


    esat::WindowDestroy();
    return 0;
}