#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include "structs.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
// debug
#include <direct.h>

double current_time, last_time;
int fps = 60;

float windowX = 800.0f, windowY = 608.0f, tempTime = 0, tempAskRegister = 0;

esat::Vec3* points = (esat::Vec3*) malloc (5 * sizeof(esat::Vec3));

Game currentGame;

// Inicializar
void InitShip() {

}

void InitConfig() {

    esat::DrawSetTextFont("./Recursos/Fuentes/horrendo.ttf");
    
    currentGame.actualScene = Scenes::MAIN_MENU;
}

void ControlsDetect() {
    switch (currentGame.actualScene) {
        case Scenes::MAIN_MENU:
            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)) {
                currentGame.actualScene = Scenes::ASK_REGISTER;
                tempTime = 0;
            }
        break;
        case Scenes::ASK_REGISTER:
            if (esat::IsKeyDown('n')) {
                currentGame.actualScene = Scenes::REGISTER_MENU;
            }

            if (esat::IsKeyDown('y')) {
                currentGame.actualScene = Scenes::LOAD_REGISTER;
            }
        break;
        case Scenes::HIGHSCORES:
            
        break;
        case Scenes::LOAD_REGISTER:

        break;
        case Scenes::REGISTER_MENU:

        break;
        case Scenes::GAMEPLAY:

        break;
    }
}

void DrawMainMenu() {
	int c = ((esat::Time()/100.0f) - tempTime);

    esat::DrawSetTextSize(64);
    esat::DrawText(windowX / 3.5f, windowY / 3, "ASTEROIDS");

    esat::DrawSetTextSize(24);
    if (c % 10 != 0) {
        esat::DrawText(windowX / 3, windowY / 1.5f, "PRESS ENTER TO PLAY");
    }
}

void DrawHighscores() {

}

void DrawAskRegisterMenu() {
    int c = ((esat::Time()/100.0f) - tempAskRegister);

    if (c % 10 != 0) {
        esat::DrawSetTextSize(36);
        esat::DrawText(windowX / 3, windowY / 4, "HAVE ACCOUNT?");
    }

    esat::DrawSetTextSize(24);
    esat::DrawText(windowX / 4, windowY / 1.5f, "YES (y)");
    esat::DrawText(windowX / 1.5f, windowY / 1.5f, "NO (n)");
}

void DrawLoadRegister() {
    esat::DrawText(windowX / 2, windowY / 2, "LOADED");
}

void DrawRegisterMenu() {
    esat::DrawText(windowX / 2, windowY / 2, "TACTICS");
}

void DrawGameplay() {

}

int esat::main(int argc, char **argv) {

    esat::WindowInit(windowX, windowY);
    esat::WindowSetMouseVisibility(true);

    InitConfig();

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
            case Scenes::ASK_REGISTER:
                DrawAskRegisterMenu();
            break;
            case Scenes::LOAD_REGISTER:
                DrawLoadRegister();
            break;
            case Scenes::REGISTER_MENU:
                DrawRegisterMenu();
            break;
            case Scenes::GAMEPLAY:
                DrawGameplay();
            break;
        }

        ControlsDetect();

        esat::DrawEnd();

        do { current_time = esat::Time(); }
        while ((current_time - last_time) <= 1000.0 / fps);

        esat::WindowFrame();
    }

    free(points);
    esat::WindowDestroy();
    return 0;
}