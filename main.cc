#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include "structs.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

double current_time, last_time;
int fps = 60;

float windowX = 800.0f, windowY = 608.0f, tempTime = 0, tempAskRegister = 0, tempStickBar = 0;

esat::Vec3* points = (esat::Vec3*) malloc (5 * sizeof(esat::Vec3));

Game currentGame;

const int numPoints = 5;

// We dont need this
float pi = 3.141592f;

esat::Vec3 g_figurita[numPoints];

esat::Vec2 stickPosition, stickLoginPosition;

char* nickname = (char*) malloc (1);
int nicknameLength = 0;

char* userPlayer = (char*) malloc (1);
int userPlayerLength = 0, userPlayerMaxLength = 15;

char* password = (char*) malloc (1);
int passwordLength = 0, passwordMaxLength = 15;;

char* userLogin = (char*) malloc (1);
int userLoginLength = 0, userLoginMaxLength = 15;

char* passwordLogin = (char*) malloc (1);
int passwordLoginLength = 0, passwordLoginMaxLength = 15;

int currentField = 0, currentLoginField = 0;

FILE *file;

struct User {
    char* nickname;
    char* userPlayer;
    char* password;
    bool isAdmin = false;
};

User user;

float DegreeToRadians(float degree) {
    return degree * pi / 180.0f;
}

// Inicializar
void InitShip() {
    g_figurita[0] = {cosf(DegreeToRadians(0.0f)) * 40, sinf(DegreeToRadians(0.0f)) * 50, 1.0f};
    g_figurita[1] = {cosf(DegreeToRadians(160.0f)) * 15, sinf(DegreeToRadians(160.0f)) * 15, 1.0f};
    g_figurita[2] = {cosf(DegreeToRadians(170.0f)) * 10, sinf(DegreeToRadians(170.0f)) * 10, 1.0f};
    g_figurita[3] = {cosf(DegreeToRadians(-170.0f)) * 10, sinf(DegreeToRadians(-170.0f)) * 10, 1.0f};
    g_figurita[4] = {cosf(DegreeToRadians(-160.0f)) * 15, sinf(DegreeToRadians(-160.0f)) * 15, 1.0f};
}

void InitConfig() {

    esat::DrawSetTextFont("./Recursos/Fuentes/horrendo.ttf");
    
    currentGame.actualScene = Scenes::MAIN_MENU;

    // Variables que se modifican con el paso del tiempo y de la lectura
    stickPosition.x = windowX / 7;
    stickPosition.y = windowY / 4;

    // Variables que se modifican con el login
    stickLoginPosition.x = windowX / 7;
    stickLoginPosition.y = windowY / 2.5f;

    *(userPlayer+0) = '\0';
    *(nickname+0) = '\0';
    *(password+0) = '\0'; 
}

void DrawStickBar() {

    int c = ((esat::Time()/100.0f) - tempStickBar);

    esat::DrawSetTextSize(24);

    if (c % 10 != 0) {
        esat::DrawText(stickPosition.x, stickPosition.y, "--o");
    }
}

void DrawStickLoginBar() {
    int c = ((esat::Time()/100.0f) - tempStickBar);

    esat::DrawSetTextSize(24);

    if (c % 10 != 0) {
        esat::DrawText(stickLoginPosition.x, stickLoginPosition.y, "--o");
    }
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
            if (esat::IsKeyDown('N')) {
                currentGame.actualScene = Scenes::REGISTER_MENU;
            }

            if (esat::IsKeyDown('Y')) {
                currentGame.actualScene = Scenes::LOAD_REGISTER;
            }
        break;
        case Scenes::HIGHSCORES:
            
        break;
        case Scenes::LOAD_REGISTER:
            DrawStickLoginBar();

            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab)) {
                if (stickLoginPosition.y == windowY / 2.5f) {
                    stickLoginPosition.y = windowY / 2;
                } else if ( stickLoginPosition.y == windowY / 2) {
                    stickLoginPosition.y = windowY / 1.5f;
                } else if (stickLoginPosition.y == windowY / 1.5f){
                    stickLoginPosition.y = windowY / 2.5f;
                }

                currentLoginField = (currentLoginField + 1) % 3;
            }
        break;
        case Scenes::REGISTER_MENU:
            DrawStickBar();

            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab)) {
                if (stickPosition.y == windowY / 2) {
                    stickPosition.y = windowY / 1.3f;
                } else if ( stickPosition.y == windowY / 1.3f) {
                    stickPosition.y = windowY / 4;
                } else {
                    stickPosition.y += windowY / 8;
                }

                currentField = (currentField + 1) % 4;
            }

        break;
        case Scenes::GAMEPLAY:

        break;
    }
}

bool CheckUserName(char* userName) {
    bool isValid = true;

    if ((*userName+0) != 'A') {
        isValid = false;
    }

    if ((*userName+1) != 'D') {
        isValid = false;
    }

    if ((*userName+2) != 'M') {
        isValid = false;
    }

    if ((*userName+3) != 'I') {
        isValid = false;
    }

    if ((*userName+4) != 'N') {
        isValid = false;
    }

    return isValid;
}

bool CheckPassword(char* password) {
    bool isValid = true;

    if ((*password+i) != '1') {
        isValid = false;
    }

    if ((*password+i) != '2') {
        isValid = false;
    }

    if ((*password+i) != '3') {
        isValid = false;
    }

    if ((*password+i) != '4') {
        isValid = false;
    }

    if ((*password+i) != '5') {
        isValid = true;
    }

    return isValid;
}

void SaveUser() {
    user.nickname = nickname;
    user.userPlayer = userPlayer;
    user.password = password;


    file = fopen("users.dat", "ab");

    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    if (CheckUserName(userPlayer) && CheckPassword(password)) {
        user.isAdmin = true;
    }

    fwrite(&user, sizeof(User), 1, file);

    fclose(file);
}

bool CheckUser() {
    return true;
}

void HandleTextInputDynamic() {
    char character;

    if (currentField == 0) {
        for (character = 'A'; character <= 'Z'; character++) {

            // checkeo de la longitud y de la pulsación de tecla
            if (esat::IsKeyDown(character) && nicknameLength < 3) {
                nicknameLength++;
                nickname = (char*) realloc(nickname, nicknameLength + 1);
                *(nickname+nicknameLength - 1) = character;
                *(nickname+nicknameLength) = '\0';
            }
        }

        for (character = 'a'; character <= 'z'; character++) {

            if (esat::IsKeyDown(character) && nicknameLength < 3) {
                nicknameLength++;
                nickname = (char*) realloc(nickname, nicknameLength + 1);
                *(nickname+nicknameLength - 1) = character;
                *(nickname+nicknameLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) &&
            nicknameLength > 0) {
            nicknameLength--;
            *(nickname + nicknameLength) = '\0';
        }
    }

    if (currentField == 1) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i) && userPlayerLength < userPlayerMaxLength) {
                userPlayerLength++;
                userPlayer = (char*) realloc(userPlayer, userPlayerLength + 1);
                *(userPlayer+userPlayerLength - 1) = i;
                *(userPlayer+userPlayerLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) &&
            userPlayerLength > 0) {
            userPlayerLength--;
            *(userPlayer+userPlayerLength) = '\0';
        }
    }

    if (currentField == 2) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && passwordLength < passwordMaxLength) {
                passwordLength++;
                password = (char*) realloc(password, passwordLength + 1);
                *(password+passwordLength - 1) = (char)i;
                *(password+passwordLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && passwordLength > 0) {
            passwordLength--;
            *(password+passwordLength) = '\0';
        }
    }

    if (currentField == 3) {
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)) {
            SaveUser();
            currentGame.actualScene = ASK_REGISTER;
        }
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        currentGame.actualScene = ASK_REGISTER;
    }
}

void HandleLogin() {
    char character;

    if (currentLoginField == 0) {
        for (character = 'A'; character <= 'Z'; character++) {

            if (esat::IsKeyDown(character) && userLoginLength < userLoginMaxLength) {
                userLoginLength++;
                userLogin = (char*) realloc(userLogin, userLoginLength + 1);
                *(userLogin+userLoginLength - 1) = character;
                *(userLogin+userLoginLength) = '\0';
            }
        }

        for (character = 'a'; character <= 'z'; character++) {

            if (esat::IsKeyDown(character) && userLoginLength < userLoginMaxLength) {
                userLoginLength++;
                userLogin = (char*) realloc(userLogin, userLoginLength + 1);
                *(userLogin+userLoginLength - 1) = character;
                *(userLogin+userLoginLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) &&
            userLoginLength > 0) {
            userLoginLength--;
            *(userLogin + userLoginLength) = '\0';
        }
    }

    if (currentLoginField == 1) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && passwordLoginLength < passwordLoginMaxLength) {
                passwordLoginLength++;
                passwordLogin = (char*) realloc(passwordLogin, passwordLoginLength + 1);
                *(password+passwordLength - 1) = (char)i;
                *(password+passwordLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && passwordLoginLength > 0) {
            passwordLoginLength--;
            *(passwordLogin+passwordLoginLength) = '\0';
        }
    }

    if (currentLoginField == 2) {
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)) {
            bool validUser = CheckUser();

            if (validUser) {
                currentGame.actualScene = GAMEPLAY;
            }
        }
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        currentGame.actualScene = ASK_REGISTER;
    }
}

void DrawMainMenu() {
    esat::DrawSetFillColor(255, 255, 255, 255);

	int c = ((esat::Time()/100.0f) - tempTime);

    esat::DrawSetTextSize(64);
    esat::DrawText(windowX / 3.5f, windowY / 3, "ASTEROIDS");

    esat::DrawSetTextSize(24);
    if (c % 10 != 0) {
        esat::DrawText(windowX / 3, windowY / 1.5f, "PRESS ENTER TO PLAY");
    }
}

void DrawHighscores() {
    esat::DrawSetFillColor(255, 255, 255, 255);

}

void DrawAskRegisterMenu() {

    esat::DrawSetFillColor(255, 255, 255, 255);

    int c = ((esat::Time()/100.0f) - tempAskRegister);

    if (c % 10 != 0) {
        esat::DrawSetTextSize(36);
        esat::DrawText(windowX / 3, windowY / 4, "HAVE ACCOUNT?");
    }

    esat::DrawSetTextSize(24);
    esat::DrawText(windowX / 4, windowY / 1.5f, "YES (y)");
    esat::DrawText(windowX / 1.5f, windowY / 1.5f, "NO (n)");
}

void DrawBack() {

    esat::DrawSetTextSize(30);
    esat::DrawText(20, windowY - 50, "BACK (o---)");
}

void DrawLoadRegister() {

    esat::DrawSetFillColor(255, 255, 255, 255);

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 2.5f, windowY / 4, "LOGIN");

    esat::DrawSetTextSize(24);
    esat::DrawText(windowX / 3.5f, windowY / 2.5f, "USER: ");
    esat::DrawText(windowX / 3.5f, windowY / 2, "PASSWORD: ");
    esat::DrawText(windowX / 2.5f, windowY / 1.5f, "PLAY.....");

    // nickname
    esat::DrawText(windowX / 2, windowY / 2.5f, userLogin);

    // tema de password
    char hiddenPass[50] = "";
    for(int i=0; i<passwordLoginLength; i++) *(hiddenPass+i) = '*';
    hiddenPass[passwordLoginLength] = '\0';

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 2, windowY / 1.9f, hiddenPass);

    DrawBack();
}

void DrawRegisterMenu() {

    esat::DrawSetFillColor(255, 255, 255, 255);

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 3.5f, windowY / 7, "REGISTER INFO");
    esat::DrawSetTextSize(24);

    esat::DrawText(windowX / 5, windowY / 4, "NICKNAME: ");

    esat::DrawText(windowX / 5, windowY / 2.65f, "USER: ");
    
    esat::DrawText(windowX / 5, windowY / 2, "PASSWORD: ");

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 2.5f, windowY / 1.3f, "SAVE");

    // nickname
    esat::DrawText(windowX / 2, windowY / 4, nickname);

    esat::DrawSetTextSize(26);
    esat::DrawText(windowX / 2, windowY / 2.65f, userPlayer);

    // tema de password
    char hiddenPass[50] = "";
    for(int i=0; i<passwordLength; i++) *(hiddenPass+i) = '*';
    hiddenPass[passwordLength] = '\0';

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 2, windowY / 1.9f, hiddenPass);

    DrawBack();
}

void DrawGameplay() {

    esat::DrawSetFillColor(255, 255, 255, 255);

}

esat::Mat3 UpdateFigurita(esat::Vec2 scale, float angle, esat::Vec2 whereMove) {
    
    esat::Mat3 m = esat::Mat3Identity();
    m = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 0.0f), m);
    m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
    m = esat::Mat3Multiply(esat::Mat3Translate(whereMove.x, whereMove.y), m);
    m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);

    return m;
}

void DrawFigurita(esat::Mat3 m, int numberOfFigures) {

    float points[numPoints * 2];

    m = esat::Mat3Multiply(esat::Mat3Translate(esat::MousePositionX(), esat::MousePositionY()), m);

    for (int i = 0; i < numPoints; i++) {
        // Necesitamos esto para transformar los Mat3 en Vec3, para dibujar
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_figurita[i]);
        points[i*2] = tmp.x;
        points[i*2+1] = tmp.y;
    }
    esat::DrawSolidPath(points, numPoints);
}

int esat::main(int argc, char **argv) {

    esat::WindowInit(windowX, windowY);
    esat::WindowSetMouseVisibility(true);

    InitConfig();
    InitShip();

    esat::Mat3 matriz = UpdateFigurita({1.0f, 1.0f}, 0.0f, {0.0f, 0.0f});

    while (esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
        last_time = esat::Time();

        esat::DrawBegin();
        esat::DrawClear(0, 0, 0);


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
                HandleLogin();
            break;
            case Scenes::REGISTER_MENU:

                DrawRegisterMenu();
                HandleTextInputDynamic();
            break;
            case Scenes::GAMEPLAY:

                DrawGameplay();

                esat::DrawSetFillColor(0, 0, 0, 255);
                DrawFigurita(matriz, numPoints);
            break;
        }

        ControlsDetect();

        esat::DrawEnd();

        do { current_time = esat::Time(); }
        while ((current_time - last_time) <= 1000.0 / fps);

        esat::WindowFrame();
    }

    free(points);
    free(nickname);
    free(userPlayer);
    free(password);

    esat::WindowDestroy();
    return 0;
}