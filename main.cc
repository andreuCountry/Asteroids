#include <esat/window.h>
#include <esat/draw.h>
#include <esat/sprite.h>
#include <esat/input.h>
#include <esat/time.h>
#include "structs.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

double current_time, last_time;
int fps = 60;

float windowX = 800.0f, windowY = 608.0f, tempTime = 0, tempAskRegister = 0, tempStickBar = 0;

const int numPoints = 5, pi = 3.141592f;

esat::Vec3* points = (esat::Vec3*) malloc (numPoints * sizeof(esat::Vec3));

Game currentGame;

esat::Vec2 stickPosition, stickLoginPosition, adminSectionStickPosition;

char* nickname = (char*) malloc (1);
int nicknameLength = 0;

char* userPlayer = (char*) malloc (1);
int userPlayerLength = 0, userPlayerMaxLength = 15;

char* password = (char*) malloc (1);
int passwordLength = 0, passwordMaxLength = 15;

char* userLogin = (char*) malloc (1);
int userLoginLength = 0, userLoginMaxLength = 15;

char* passwordLogin = (char*) malloc (1);
int passwordLoginLength = 0, passwordLoginMaxLength = 15;

char* nicknameEdit = (char*) malloc (1);
int nicknameEditLength = 0;

char* userPlayerEdit = (char*) malloc (1);
int userPlayerEditLength = 0, userPlayerEditMaxLength = 15;

char* passwordEdit = (char*) malloc (1);
int passwordEditLength = 0, passwordEditMaxLength = 15;

int currentField = 0, currentLoginField = 0, currentEditField = 0;
int userId = 0;
int positionInPage = 1;

FILE *file;

struct User {
    int id;
    char* nickname;
    char* userPlayer;
    char* password;
    bool isAdmin = false;
    int credits;
    bool isDeleted = false;
    int puntuation;
};

// Globales para ir byte por byte en los bloques de memoria para copiar su info y estructurarla
#define OFFSET_ID        0
#define OFFSET_NICK      4
#define OFFSET_USER      7
#define OFFSET_PASS      21
#define OFFSET_ADMIN     35
#define OFFSET_CREDITS   36
#define OFFSET_DELETED   40
#define OFFSET_PUNTUA    41

struct Ship {
    esat::Vec3* points;
    int lifes;
    bool isAlive;
    esat::Vec3 centralPoint;
    esat::Vec2 speed = {0.0f, 0.0f};
    esat::Vec2 acceleration;
    float angle = 0.0f;
};

User user, userLooked;
User* usersToShow = nullptr;
User* usersOrdered = nullptr;

Ship shipPlayer;

const float acceleration = 1.005f;
const float deceleration = 0.995;
const float maxSpeed = 15.0f;
const float minimumSpeed = 0.0f;

int lastIdInserted = 0, countUsersNotDeleted = 0, currentPage = 0, usersOrderedCount = 0;

// si esto es true, podemos pasar de pagina en la sección de admin
bool canPassPage = false;

float DegreeToRadians(float degree) {
    return degree * pi / 180.0f;
}

// Inicializar
void InitShip() {
    *(points+0) = {cosf(DegreeToRadians(0.0f)) * 40, sinf(DegreeToRadians(0.0f)) * 50, 1.0f};
    *(points+1) = {cosf(DegreeToRadians(160.0f)) * 15, sinf(DegreeToRadians(160.0f)) * 15, 1.0f};
    *(points+2) = {cosf(DegreeToRadians(170.0f)) * 10, sinf(DegreeToRadians(170.0f)) * 10, 1.0f};
    *(points+3) = {cosf(DegreeToRadians(-170.0f)) * 10, sinf(DegreeToRadians(-170.0f)) * 10, 1.0f};
    *(points+4) = {cosf(DegreeToRadians(-160.0f)) * 15, sinf(DegreeToRadians(-160.0f)) * 15, 1.0f};

    shipPlayer.centralPoint = {windowX / 2, windowY / 2, 1.0f};
    shipPlayer.points = points;
}

void LoadUsers() {
    file = fopen("users.dat", "r+b");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    int id, puntuation;
    bool isDeleted;

    // reset para que no se dupliquen los usuarios no borrados, por mala gestión de esta función
    countUsersNotDeleted = 0;

    while (fread(&id, sizeof(id), 1, file) == 1) {

        if (id > lastIdInserted) {
            lastIdInserted = id;
        }

        // saltar la parte restante del user que no me interesa
        fseek(file, 3 + 14 + 14 + sizeof(bool) + sizeof(int), SEEK_CUR);

        fread(&isDeleted, sizeof(isDeleted), 1, file);
        if (!isDeleted) {
            countUsersNotDeleted++;
        }

        fread(&puntuation, sizeof(puntuation), 1, file);
    }

    fclose(file);
    //printf("id: [%d] \n", lastIdInserted);
    //printf("counted players not deleted: [%d]", countUsersNotDeleted);

}

void LoadUsersLogin() {
    file = fopen("users.dat", "r+b");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    usersToShow = (User*) malloc(countUsersNotDeleted * 45);
    if (usersToShow == NULL) {
        printf("No hay memoria\n");
        fclose(file);
        return;
    }

    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);
    char* tmpPass = (char*) malloc(15);

    bool admin;
    int credits;
    int id;
    bool isDeleted;
    int puntuation;

    int index = 0;

    while (fread(&id, sizeof(id), 1, file) == 1) {
        fread(tmpNick, 3, 1, file); tmpNick[3] = '\0';
        fread(tmpUser, 14, 1, file); tmpUser[14] = '\0';
        fread(tmpPass, 14, 1, file); tmpPass[14] = '\0';
        fread(&admin, sizeof(admin), 1, file);
        fread(&credits, sizeof(credits), 1, file);
        fread(&isDeleted, sizeof(isDeleted), 1, file);
        fread(&puntuation, sizeof(puntuation), 1, file);

        if (!isDeleted) {

            // fumada histórica, para copiar en bloques de memoria, memcpy, parecido al strcpy
            unsigned char* ptr = ((unsigned char*)usersToShow) + index * 45;

            memcpy(ptr + OFFSET_ID, &id, 4);
            memcpy(ptr + OFFSET_NICK, tmpNick, 3);
            memcpy(ptr + OFFSET_USER, tmpUser, 14);
            memcpy(ptr + OFFSET_PASS, tmpPass, 14);
            memcpy(ptr + OFFSET_ADMIN, &admin, 1);
            memcpy(ptr + OFFSET_CREDITS, &credits, 4);
            memcpy(ptr + OFFSET_DELETED, &isDeleted, 1);
            memcpy(ptr + OFFSET_PUNTUA, &puntuation, 4);

            index++;
        }
        //printf("puntuation=%d \n", puntuation);
    }

    fclose(file);
}

void LoadUsersOrdered() {
    file = fopen("users.dat", "r+b");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    usersOrdered = (User*) malloc(10 * 45);
    if (usersOrdered == NULL) {
        printf("No hay memoria ni jugadores para asociar puntuacion \n");
        fclose(file);
        return;
    }

    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);
    char* tmpPass = (char*) malloc(15);

    bool admin;
    int credits;
    int id;
    bool isDeleted;
    int puntuation;
    int auxPuntuation = 0;

    int count = 0;
    while (fread(&id, sizeof(id), 1, file) == 1) {
        fread(tmpNick, 3, 1, file); tmpNick[3] = '\0';
        fread(tmpUser, 14, 1, file); tmpUser[14] = '\0';
        fread(tmpPass, 14, 1, file); tmpPass[14] = '\0';
        fread(&admin, sizeof(admin), 1, file);
        fread(&credits, sizeof(credits), 1, file);
        fread(&isDeleted, sizeof(isDeleted), 1, file);
        fread(&puntuation, sizeof(puntuation), 1, file);

        if (isDeleted) continue;

        int pos = 0;
        for (; pos < count; pos++) {
            unsigned char* current = ((unsigned char*)usersOrdered) + pos * 45;

            int currentScore;
            memcpy(&currentScore, current + OFFSET_PUNTUA, 4);

            if (puntuation > currentScore) {
                break;
            }
        }

        if (pos < 10) {

            for (int j = (count < 10 ? count : 9); j > pos; j--) {
                unsigned char* destiny = ((unsigned char*)usersOrdered) + j * 45;
                unsigned char* source = ((unsigned char*)usersOrdered) + (j - 1) * 45;

                memcpy(destiny, source, 45);
            }

            unsigned char* ptr = ((unsigned char*)usersOrdered) + pos * 45;

            memcpy(ptr + OFFSET_ID, &id, 4);
            memcpy(ptr + OFFSET_NICK, tmpNick, 3);
            memcpy(ptr + OFFSET_USER, tmpUser, 14);
            memcpy(ptr + OFFSET_PASS, tmpPass, 14);
            memcpy(ptr + OFFSET_ADMIN, &admin, 1);
            memcpy(ptr + OFFSET_CREDITS, &credits, 4);
            memcpy(ptr + OFFSET_DELETED, &isDeleted, 1);
            memcpy(ptr + OFFSET_PUNTUA, &puntuation, 4);

            if (count < 10) count++;
        }
    }

    fclose(file);

    usersOrderedCount = count;
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

    adminSectionStickPosition.x = windowX / 12;
    adminSectionStickPosition.y = windowY / 2.75f;

    *(userPlayer+0) = '\0';
    *(nickname+0) = '\0';
    *(password+0) = '\0'; 

    LoadUsers();
    // Resetear el buffer del teclado
    esat::ResetBufferdKeyInput();
}

void ShowPlayersAdminSection() {
    esat::DrawSetTextSize(20);

    const int usersPerPage = 4;

    // Calculo de paginación para la sección de usuarios
    int startIndex = currentPage * usersPerPage;
    int endIndex = startIndex + usersPerPage;

    if (endIndex > countUsersNotDeleted) endIndex = countUsersNotDeleted;

    float y = windowY / 2.75f;

    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);
    char* tmpPass = (char*) malloc(15);
    
    // currentPage + 1, esto hace referencia al indice de la proxima linea
    // usuarios de pagina son los actuales, multiplicado saco los de la proxima pagina
    // si son menos que countUsersNotDeleted sacamos que se puede pasar
    if ((currentPage + 1) * usersPerPage < countUsersNotDeleted) {
        canPassPage = true;
    } else {
        canPassPage = false;
    }

    for (int i = startIndex; i < endIndex; i++) {
        char* u = ((char*)usersToShow) + i * 45;

        memcpy(tmpNick, u + OFFSET_NICK, 3);
        tmpNick[3] = '\0';

        memcpy(tmpUser, u + OFFSET_USER, 14);
        tmpUser[14] = '\0';

        memcpy(tmpPass, u + OFFSET_PASS, 14);
        tmpPass[14] = '\0';

        esat::DrawText(120, y, tmpNick);
        esat::DrawText(120 + 200, y, tmpUser);
        esat::DrawText(120 + 200 + 200, y, tmpPass);

        if (y == windowY / 2.75f) {
            y = windowY / 2;
        } else if (y == windowY / 2) {
            y = windowY / 1.5f;
        } else if (y == windowY / 1.5f) {
            y = windowY / 1.2f;
        }
    }
}

void ShowOrderedPlayersScore() {
    esat::DrawSetTextSize(20);

    float y = 80.f;

    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);
    char* tmpPass = (char*) malloc(15);

    bool admin;
    int credits;
    int id;
    bool isDeleted;
    int puntuation;

    char* puntuationBuffer = (char*) malloc(6);

    // recordamos usar el conteo de los usuarios que hay ordenados, por si hay menos de 10
    // validación pocha pero nos aseguramos de que no se inserte basura en memoria
    // además solo trabajamos con los usuarios que tenemos, con máximo de 10
    for (int i = 0; i < usersOrderedCount; i++) {
        char* u = ((char*)usersOrdered) + i * 45;

        memcpy(tmpNick, u + OFFSET_NICK, 3);
        tmpNick[3] = '\0';

        memcpy(tmpUser, u + OFFSET_USER, 14);
        tmpUser[14] = '\0';

        memcpy(tmpPass, u + OFFSET_PASS, 14);
        tmpPass[14] = '\0';

        memcpy(&admin, u + OFFSET_ADMIN, 1);
        memcpy(&credits, u + OFFSET_CREDITS, 4);
        memcpy(&isDeleted, u + OFFSET_DELETED, 1);
        memcpy(&puntuation, u + OFFSET_PUNTUA, 4);

        esat::DrawText(250, y, tmpNick);
        esat::DrawText(250 + 100, y, tmpUser);

        // romper bucle para que no muestre basura la conversacion de la puntuation en char *
        if (tmpNick[0] == '\0') {
            continue;
        }

        snprintf(puntuationBuffer, 6, "%d", puntuation);
        esat::DrawText(550, y, puntuationBuffer);

        y += 50.0f;
    }
}

void DrawStickBar() {

    int c = ((esat::Time()/100.0f) - tempStickBar);

    esat::DrawSetTextSize(24);

    if (c % 10 != 0) {
        esat::DrawText(stickPosition.x, stickPosition.y, "--o");
    }
}

void DrawAdminSectionStickBar() {
    int c = ((esat::Time()/100.0f) - tempStickBar);

    esat::DrawSetTextSize(24);

    if (c % 10 != 0) {
        esat::DrawText(adminSectionStickPosition.x, adminSectionStickPosition.y, "--o");
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
                LoadUsersOrdered();
                currentGame.actualScene = Scenes::HIGHSCORES;
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

            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
                LoadUsersOrdered();
                currentGame.actualScene = Scenes::HIGHSCORES;
            }
        break;
        case Scenes::HIGHSCORES:
            ShowOrderedPlayersScore();
        break;
        case Scenes::ADMIN_SECTION:
            // Partes estaticas
            DrawAdminSectionStickBar();

            // Mostrar Usuarios en pantalla dependiendo de la current page y de si estan borrados o no
            ShowPlayersAdminSection();

            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab)) {
                if (adminSectionStickPosition.y == windowY / 2.75f) {
                    adminSectionStickPosition.y = windowY / 2;
                } else if (adminSectionStickPosition.y == windowY / 2) {
                    adminSectionStickPosition.y = windowY / 1.5f;
                } else if (adminSectionStickPosition.y == windowY / 1.5f) {
                    adminSectionStickPosition.y = windowY / 1.2f;
                } else if (adminSectionStickPosition.y == windowY / 1.2f) {
                    adminSectionStickPosition.y = windowY / 2.75f;
                }
            }
        break;
        case Scenes::EDIT_SECTION:
            DrawStickBar();

            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab)) {
                if (stickPosition.y == windowY / 2) {
                    stickPosition.y = windowY / 1.3f;
                } else if ( stickPosition.y == windowY / 1.3f) {
                    stickPosition.y = windowY / 4;
                } else {
                    stickPosition.y += windowY / 8;
                }

                currentEditField = (currentEditField + 1) % 4;
            }
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

    if (*(userName+0) != 'A') {
        isValid = false;
    }

    if (*(userName+1) != 'D') {
        isValid = false;
    }

    if (*(userName+2) != 'M') {
        isValid = false;
    }

    if (*(userName+3) != 'I') {
        isValid = false;
    }

    if (*(userName+4) != 'N') {
        isValid = false;
    }

    return isValid;
}

bool CheckPassword(char* password) {
    bool isValid = true;

    if (*(password+0) != '1') {
        isValid = false;
    }

    if (*(password+1) != '2') {
        isValid = false;
    }

    if (*(password+2) != '3') {
        isValid = false;
    }

    if (*(password+3) != '4') {
        isValid = false;
    }

    if (*(password+4) != '5') {
        isValid = false;
    }

    return isValid;
}

void SaveUser() {
    user.nickname = nickname;
    user.userPlayer = userPlayer;
    user.password = password;
    user.credits = 10;
    lastIdInserted++;
    user.id = lastIdInserted;
    user.isDeleted = false;
    user.puntuation = 0;

    file = fopen("users.dat", "ab");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    if (CheckUserName(userPlayer) && CheckPassword(password)) {
        user.isAdmin = true;
        user.puntuation = 99999;
    } else {
        user.isAdmin = false;
    }

    fwrite(&user.id, sizeof(user.id), 1, file);
    fwrite(user.nickname, 3, 1, file);
    fwrite(user.userPlayer, 14, 1, file);
    fwrite(user.password, 14, 1, file);
    fwrite(&user.isAdmin, sizeof(user.isAdmin), 1, file);
    fwrite(&user.credits, sizeof(user.credits), 1, file);
    fwrite(&user.isDeleted, sizeof(user.isDeleted), 1, file);
    fwrite(&user.puntuation, sizeof(user.puntuation), 1, file);

    fclose(file);
}

bool CheckUserAdmin(bool isLogin) {
    if (isLogin) {
        return CheckUserName(userLogin) && CheckPassword(passwordLogin);
    }
    return CheckUserName(userPlayer) && CheckPassword(password);
}

bool CheckOptionalUser() {
    file = fopen("users.dat", "rb");
    if (!file) {
        printf("Error abriendo users.dat\n");
        return false;
    }

    bool isValid = false;

    // reservilla de memoria dinamica para hacer coincidir los char con los strings
    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);
    char* tmpPass = (char*) malloc(15);

    bool admin;
    int credits;
    int id;
    bool isDeleted;
    int puntuation;

    FILE* f = fopen("users.dat", "rb");
    while (fread(&id, sizeof(id), 1, f) == 1 && !isValid) {
        fread(tmpNick, 3, 1, f); tmpNick[3] = '\0';
        fread(tmpUser, 14, 1, f); tmpUser[14] = '\0';
        fread(tmpPass, 14, 1, f); tmpPass[14] = '\0';
        fread(&admin, sizeof(admin), 1, f);
        fread(&credits, sizeof(credits), 1, f);
        fread(&isDeleted, sizeof(isDeleted), 1, f);
        fread(&puntuation, sizeof(puntuation), 1, f);

        // Necesario para la salud mental
        printf(" id=%d, nickname='%s', userPlayer='%s', password='%s', isAdmin=%d, credits=%d isDeleted=%d puntuation=%d \n",  id, tmpNick, tmpUser, tmpPass, admin, credits, isDeleted, puntuation);

        if ((strcmp(tmpUser, userLogin) == 0) && (strcmp(tmpPass, passwordLogin) == 0)) {
            isValid = true;
        }
    }

    fclose(f);

    free(tmpNick); free(tmpUser); free(tmpPass);

    return isValid;
}

void MarkUserAsDeleted(int id) {
    file = fopen("users.dat", "r+b");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    int idToDelete;

    while (fread(&idToDelete, sizeof(idToDelete), 1, file) == 1) {

        if (id == idToDelete) {
            bool deleted = true;

            fseek(file, 3 + 14 + 14 + sizeof(bool) + sizeof(int), SEEK_CUR);

            fwrite(&deleted, sizeof(bool), 1, file);
            break;
        }

        // saltamos directamente al siguiente usuario saltando correctamente la memoria
        fseek(file, 3 + 14 + 14 + sizeof(bool) + sizeof(int) + sizeof(bool) + sizeof(int), SEEK_CUR);

    }

    fclose(file);
    LoadUsers();
    LoadUsersLogin();
}

/*
    Every time we do tab on adminSection, we need to know which id we need to delete,
    for that we need to calculate dynamically which one is using the actual page and their position
    on the list
*/
int CalculateIdDynamic(int positionInList) {
    file = fopen("users.dat", "r+b");
    if (file == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    int globalPosition = currentPage * 4 + positionInList;

    int id, countDeleted = 0;
    bool isDeleted;
    int puntuation;

    while (fread(&id, sizeof(id), 1, file) == 1) {

        // saltar la parte restante del user que no me interesa
        fseek(file, 3 + 14 + 14 + sizeof(bool) + sizeof(int), SEEK_CUR);

        fread(&isDeleted, sizeof(isDeleted), 1, file);
        
        if (!isDeleted) {
            globalPosition--;

            if (globalPosition == 0) {
                fclose(file);

                return id;
            }
        }

        fread(&puntuation, sizeof(puntuation), 1, file);
    }

    fclose(file);

    return -1;
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

            LoadUsers();
            LoadUsersLogin();
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
                *(passwordLogin+passwordLoginLength - 1) = (char)i;
                *(passwordLogin+passwordLoginLength) = '\0';
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
            bool isUserAdmin = CheckUserAdmin(true);
            bool optionalUser = CheckOptionalUser();

            if (isUserAdmin) {

                // Cargar usuarios para listarlos
                LoadUsers();
                LoadUsersLogin();
                currentGame.actualScene = ADMIN_SECTION;
                
                // reset para que apunte siempre al primero
                adminSectionStickPosition.y = windowY / 2.75f;
                userId = CalculateIdDynamic(1);

            } else {
                if (optionalUser) {
                    currentGame.actualScene = GAMEPLAY;
                }
            }
        }
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        currentGame.actualScene = ASK_REGISTER;
    }
}

void HandleShipMovement() {
    if (esat::IsKeyDown('D')) {
        // move ship into right
    }
}

void HandleHighscoresSection() {
    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)) {
        currentGame.actualScene = ASK_REGISTER;
    }
}

void HandleAdminSection() {
    char character;

    if (adminSectionStickPosition.y == (windowY / 2.75f)) {
        positionInPage = 1;
    } else if (adminSectionStickPosition.y == (windowY / 2)) {
        positionInPage = 2;
    } else if (adminSectionStickPosition.y == (windowY / 1.5f)) {
        positionInPage = 3;
    } else if (adminSectionStickPosition.y == (windowY / 1.2f)) {
        positionInPage = 4;
    }

    if (canPassPage) {
        if (esat::IsKeyDown('N')) {
            adminSectionStickPosition.y = windowY / 2.75f;
            currentPage++;
            positionInPage = 1;
            userId = CalculateIdDynamic(positionInPage);
        }
    }

    if (currentPage > 0) {
        if (esat::IsKeyDown('L')) {
            adminSectionStickPosition.y = windowY / 2.75f;
            currentPage--;
            positionInPage = 1;
            userId = CalculateIdDynamic(positionInPage);
        }
    }

    if (userId != 1) {
        if (esat::IsKeyDown('D')) {
            MarkUserAsDeleted(userId);
        }
    }

    if (userId != 1) {
        if (esat::IsKeyDown('E')) {
            currentGame.actualScene = EDIT_SECTION;
        }
    }

    // Calculate dynamic id to know which one we need to delete or edit
    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab)) {
        positionInPage++;
        userId = CalculateIdDynamic(positionInPage);
    }

    if (esat::IsKeyDown('P')) {
        currentGame.actualScene = GAMEPLAY;
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        currentGame.actualScene = LOAD_REGISTER;
    }
}

void EditUser() {
    file = fopen("users.dat", "r+b");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    int id;

    while (fread(&id, sizeof(id), 1, file) == 1) {

        if (userId == id) {
            printf("Igual \n");

            long pos = ftell(file);

            fseek(file, pos - sizeof(int), SEEK_SET);

            fseek(file, sizeof(int), SEEK_CUR);

            fwrite(nicknameEdit, 3, 1, file);
            fwrite(userPlayerEdit, 14, 1, file);
            fwrite(passwordEdit, 14, 1, file);

            break;
        }

        fseek(file, 3 + 14 + 14 + sizeof(bool) + sizeof(int) + sizeof(bool) + sizeof(int), SEEK_CUR);

    }

    fclose(file);
}

void HandleEditSection() {

    char character;

    if (currentEditField == 0) {
        for (character = 'A'; character <= 'Z'; character++) {

            // checkeo de la longitud y de la pulsación de tecla
            if (esat::IsKeyDown(character) && nicknameEditLength < 3) {
                nicknameEditLength++;
                nicknameEdit = (char*) realloc(nicknameEdit, nicknameEditLength + 1);
                *(nicknameEdit+nicknameEditLength - 1) = character;
                *(nicknameEdit+nicknameEditLength) = '\0';
            }
        }

        for (character = 'a'; character <= 'z'; character++) {

            if (esat::IsKeyDown(character) && nicknameEditLength < 3) {
                nicknameEditLength++;
                nicknameEdit = (char*) realloc(nicknameEdit, nicknameEditLength + 1);
                *(nicknameEdit+nicknameEditLength - 1) = character;
                *(nicknameEdit+nicknameEditLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) &&
            nicknameEditLength > 0) {
            nicknameEditLength--;
            *(nicknameEdit + nicknameEditLength) = '\0';
        }
    }

    if (currentEditField == 1) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i) && userPlayerEditLength < userPlayerEditMaxLength) {
                userPlayerEditLength++;
                userPlayerEdit = (char*) realloc(userPlayerEdit, userPlayerEditLength + 1);
                *(userPlayerEdit+userPlayerEditLength - 1) = (char)i;
                *(userPlayerEdit+userPlayerEditLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) &&
            userPlayerEditLength > 0) {
            userPlayerEditLength--;
            *(userPlayerEdit+userPlayerEditLength) = '\0';
        }
    }

    if (currentEditField == 2) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && passwordEditLength < passwordEditMaxLength) {
                passwordEditLength++;
                passwordEdit = (char*) realloc(passwordEdit, passwordEditLength + 1);
                *(passwordEdit+passwordEditLength - 1) = (char)i;
                *(passwordEdit+passwordEditLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && passwordEditLength > 0) {
            passwordEditLength--;
            *(passwordEdit+passwordEditLength) = '\0';
        }
    }

    if (currentEditField == 3) {
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)) {
            EditUser();
            LoadUsers();
            LoadUsersLogin();
            currentGame.actualScene = ADMIN_SECTION;
        }
    }
    
    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        LoadUsers();
        LoadUsersLogin();
        currentGame.actualScene = ADMIN_SECTION;
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

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 3, 30, "HIGHSCORES REC:");
    esat::DrawLine(windowX / 3.1f, 45, windowX / 1.5f, 45);

    esat::DrawText(windowX / 4, 80, "1.");
    esat::DrawText(windowX / 4, 130, "2.");
    esat::DrawText(windowX / 4, 180, "3.");
    esat::DrawText(windowX / 4, 230, "4.");
    esat::DrawText(windowX / 4, 280, "5.");
    esat::DrawText(windowX / 4, 330, "6.");
    esat::DrawText(windowX / 4, 380, "7.");
    esat::DrawText(windowX / 4, 430, "8.");
    esat::DrawText(windowX / 4, 480, "9.");
    esat::DrawText((windowX / 4) - 10, 530, "10.");

    esat::DrawSetTextSize(12);
    esat::DrawText(windowX / 2.2f, windowY - 30, "NEXT (ENTER)");

}

void DrawBack() {

    esat::DrawSetTextSize(25);
    esat::DrawText(20, windowY / 7, "BACK (o---)");
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

    esat::DrawSetTextSize(25);
    esat::DrawText(windowX / 2.4f, windowY - 50, "BACK (o---)");

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

    char hiddenPass[50] = "";
    for(int i=0; i<passwordLength; i++) *(hiddenPass+i) = '*';
    hiddenPass[passwordLength] = '\0';

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 2, windowY / 1.9f, hiddenPass);

    DrawBack();
}

void DrawAdminSection() {
    esat::DrawSetFillColor(255, 255, 255, 255);

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 3, windowY / 7, "ADMIN SECTION:");

    esat::DrawText(100, windowY / 4, "NICKNAME");
    esat::DrawText(300, windowY / 4, "USER_NAME");
    esat::DrawText(500, windowY / 4, "PASSWORD");

    esat::DrawLine(windowX / 3.1f, windowY / 6, windowX / 1.5f, windowY / 6);

    DrawBack();

    esat::DrawText(20, windowY - 50, "E (EDIT)");
    esat::DrawText(windowX / 4, windowY - 50, "D (DELETE)");
    esat::DrawText(windowX / 2, windowY - 50, "N (NEXT PAGE)");
    esat::DrawText(windowX / 1.3f, windowY - 50, "L (LAST PAGE)");

    esat::DrawText(windowX - 150, windowY / 7, "PLAY (P)");

}

void DrawEditSection() {
    esat::DrawSetFillColor(255, 255, 255, 255);

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 3, windowY / 7, "EDIT USER SECTION:");

    esat::DrawLine(windowX / 3.1f, windowY / 6, windowX / 1.5f, windowY / 6);

    esat::DrawText(windowX / 5, windowY / 4, "NICKNAME: ");

    esat::DrawText(windowX / 5, windowY / 2.65f, "USER: ");
    
    esat::DrawText(windowX / 5, windowY / 2, "PASSWORD: ");

    esat::DrawText((windowX / 2.5f) - 20, windowY / 1.3f, "SAVE (S)");

    // nickname
    esat::DrawText(windowX / 2, windowY / 4, nicknameEdit);

    esat::DrawSetTextSize(26);
    esat::DrawText(windowX / 2, windowY / 2.65f, userPlayerEdit);

    // tema de password
    char hiddenPass[50] = "";
    for(int i=0; i<passwordEditLength; i++) *(hiddenPass+i) = '*';
    hiddenPass[passwordEditLength] = '\0';

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 2, windowY / 1.9f, hiddenPass);

    DrawBack();
}

void DrawGameplay() {

    // Some issues with draw ship or gameplay
}

esat::Mat3 UpdateFigurita(esat::Vec2 scale, float angle, esat::Vec2 whereMove) {

    esat::Mat3 m = esat::Mat3Identity();
    m = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 0.0f), m);
    m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
    m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
    m = esat::Mat3Multiply(esat::Mat3Translate(whereMove.x, whereMove.y), m);

    return m;
}

void DrawFigurita(esat::Mat3 m, int numberOfFigures) {

    float points[numPoints * 2];
    esat::DrawSetFillColor(0, 0, 0, 0);
    esat::DrawSetStrokeColor(255, 255, 255, 255);

    for (int i = 0; i < numPoints; i++) {
        // Necesitamos esto para transformar los Mat3 en Vec3, para dibujar
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, shipPlayer.points[i]);
        points[i*2] = tmp.x;
        points[i*2+1] = tmp.y;
    }
    esat::DrawSolidPath(points, numPoints, true);
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
                HandleHighscoresSection();
            break;
            case Scenes::ADMIN_SECTION:
                DrawAdminSection();
                HandleAdminSection();
            break;
            case Scenes::EDIT_SECTION:
                DrawEditSection();
                HandleEditSection();
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
 
                HandleShipMovement();

                // all this shit is going into handle hell function
                // think about + and - acceleration
                if (esat::IsKeyPressed('D')) {
                    shipPlayer.angle += 0.05f;
                }

                if (esat::IsKeyPressed('A')) {
                    shipPlayer.angle -= 0.05f;
                }

                if (esat::IsKeyPressed('W')) {
                    shipPlayer.acceleration = {
                        cosf(shipPlayer.angle) * 0.2f,
                        sinf(shipPlayer.angle) * 0.2f
                    };

                    shipPlayer.speed.x += shipPlayer.acceleration.x;
                    shipPlayer.speed.y += shipPlayer.acceleration.y;

                } else {
                    shipPlayer.speed.x *= deceleration;
                    shipPlayer.speed.y *= deceleration;
                }

                shipPlayer.centralPoint.x += shipPlayer.speed.x;
                shipPlayer.centralPoint.y += shipPlayer.speed.y;
                
                matriz = UpdateFigurita({1.0f, 1.0f}, shipPlayer.angle, {shipPlayer.centralPoint.x, shipPlayer.centralPoint.y});

                DrawGameplay();
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
    free(nicknameEdit);
    free(userLogin);
    free(passwordLogin);
    free(passwordEdit);
    free(userPlayerEdit);

    esat::WindowDestroy();
    return 0;
}
