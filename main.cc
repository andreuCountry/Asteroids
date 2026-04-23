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

char* birthday = (char*) malloc (1);
int birthdayLength = 0, birthdayMaxLength = 11;

char* province = (char*) malloc (1);
int provinceLength = 0, provinceMaxLength = 15;

char* email = (char*) malloc (1);
int emailLength = 0, emailMaxLength = 15;

int credits = 0;
int creditsMax = 999999;

char* nicknameEdit = (char*) malloc (1);
int nicknameEditLength = 0;

char* userPlayerEdit = (char*) malloc (1);
int userPlayerEditLength = 0, userPlayerEditMaxLength = 15;

char* passwordEdit = (char*) malloc (1);
int passwordEditLength = 0, passwordEditMaxLength = 15;

char* birthdayEdit = (char*) malloc (1);
int birthdayEditLength = 0, birthdayEditMaxLength = 10;

char* provinceEdit = (char*) malloc (1);
int provinceEditLength = 0, provinceEditMaxLength = 15;

char* emailEdit = (char*) malloc (1);
int emailEditLength = 0, emailEditMaxLength = 15;

int creditsEdit = 0;
int creditsMaxEdit = 999999;

int currentField = 0, currentLoginField = 0, currentEditField = 0;
int userId = 0;
int positionInPage = 1;

FILE *file;

struct User {
    int id;
    char* nickname;
    char* userPlayer;
    char* password;
    char* birthday;
    char* province;
    char* email;
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
#define OFFSET_BIRTHDAY  35
#define OFFSET_PROVINCE  45
#define OFFSET_EMAIL     59

#define OFFSET_ADMIN     73
#define OFFSET_CREDITS   74
#define OFFSET_DELETED   78
#define OFFSET_PUNTUA    79

struct Shoot {
    esat::Vec3* points;
    bool isVisible;
    esat::Vec3 vectorDirector;
    float distanceTravelled;
    float maxDistanceTravelled;
};

struct Ship {
    esat::Vec3* points;
    int lifes = 3;
    bool isAlive;
    esat::Vec3 centralPoint;
    esat::Vec2 speed = {0.0f, 0.0f};
    esat::Vec2 acceleration;
    float angle = 0.0f;
    esat::Vec3 vectorDirector;
    Shoot* shoots;
    int numberShoots = 4;
};

User user, userLooked;
User* usersToShow = nullptr;
User* usersOrdered = nullptr;

Ship shipPlayer;

const float acceleration = 1.005f;
const float deceleration = 0.995;
const float maxSpeed = 10.0f;
const float minimumSpeed = 0.0f;

int lastIdInserted = 0, countUsersNotDeleted = 0, currentPage = 0, usersOrderedCount = 0;

// si esto es true, podemos pasar de pagina en la sección de admin
bool canPassPage = false;

enum AsteroidsLevel {
    LEVEL_1,
    LEVEL_2,
    LEVEL_3
};

enum AsteroidsType {
    V1,
    V2,
    V3,
    V4
};

struct Asteroids {
    AsteroidsLevel level;
    AsteroidsType type;
    esat::Vec3 *vertices;
    esat::Vec3 centralPoint;
    esat::Vec2 direction;
    bool isAlive;
    int numVertices;
};

int actualLevel, totalAsteroidsPerLevels;
int asteroidsV1Count = 0, asteroidsV2Count = 0, asteroidsV3Count = 0, asteroidsV4Count = 0;

const int numPointsAsteroidsV1 = 10, numPointsAsteroidsV2 = 12, numPointsAsteroidsV3 = 12, numPointsAsteroidsV4 = 11;

bool pendingLevelChange = false;
bool isChangingLevel = false;

Asteroids* asteroids = nullptr;
const int maxAsteroids = 180;
int activeAsteroids = 4;

int pendingLevel = -1;

// Asteroids vertexs
void VertsAsteroid1(esat::Vec3 *vertices){
    *(vertices) = {0.00f, 0.40f, 1.0f};
    *(vertices + 1) = {0.20f, 0.60f, 1.0f};
    *(vertices + 2) = {0.49f, 0.30f, 1.0f};
    *(vertices + 3) = {0.36f, -0.06f, 1.0f};
    *(vertices + 4) = {0.49f, -0.12f, 1.0f};
    *(vertices + 5) = {0.13f, -0.44f, 1.0f};
    *(vertices + 6) = {-0.27f, -0.44f, 1.0f};
    *(vertices + 7) = {-0.48f, -0.12f, 1.0f};
    *(vertices + 8) = {-0.49f, 0.30f, 1.0f};
    *(vertices + 9) = {-0.20f, 0.60f, 1.0f};
}

void VertsAsteroid2(esat::Vec3 *vertices){
    *(vertices) = {0.0769f, -0.20274f, 1.0f};
    *(vertices + 1) = {-0.1679f, -0.39662f, 1.0f};
    *(vertices + 2) = {0.2f, -0.4f, 1.0f};
    *(vertices + 3) = {0.40188f, -0.28188f, 1.0f};
    *(vertices + 4) = {0.4f, -0.2f, 1.0f};
    *(vertices + 5) = {0.30691f, -0.0049f, 1.0f};
    *(vertices + 6) = {0.4f, 0.2f, 1.0f};
    *(vertices + 7) = {0.2f, 0.4f, 1.0f};
    *(vertices + 8) = {0.08533f, 0.3156, 1.0f};
    *(vertices + 9) = {-0.12042, 0.39869, 1.0f};
    *(vertices + 10) = {-0.31035, 0.13359, 1.0f};
    *(vertices + 11) = {-0.3143, -0.20274, 1.0f};
}

void VertsAsteroid3(esat::Vec3 *vertices){
    *(vertices) = {0.15f, 0.47f, 1.0f};
    *(vertices + 1) = {0.00f, 0.40f, 1.0f};
    *(vertices + 2) = {-0.20f, 0.47f, 1.0f};
    *(vertices + 3) = {-0.41f, 0.36f, 1.0f};
    *(vertices + 4) = {-0.30f, 0.11f, 1.0f};
    *(vertices + 5) = {-0.45f, -0.12f, 1.0f};
    *(vertices + 6) = {-0.19f, -0.51f, 1.0f};
    *(vertices + 7) = {0.05f, -0.44f, 1.0f};
    *(vertices + 8) = {0.20f, -0.51f, 1.0f};
    *(vertices + 9) = {0.46f, -0.13f, 1.0f};
    *(vertices + 10) = {0.34f, 0.08f, 1.0f};
    *(vertices + 11) = {0.46f, 0.36f, 1.0f};
}

void VertsAsteroid4(esat::Vec3 *vertices){
    *(vertices) = {-0.34f, 0.70f, 1.0f};
    *(vertices + 1) = {0.25f, 0.69f, 1.0f};
    *(vertices + 2) = {0.67f, 0.09f, 1.0f};
    *(vertices + 3) = {0.67f, -0.12f, 1.0f};
    *(vertices + 4) = {0.26f, -0.71f, 1.0f};
    *(vertices + 5) = {0.00f, -0.71f, 1.0f};
    *(vertices + 6) = {0.00f, -0.31f, 1.0f};
    *(vertices + 7) = {-0.24f, -0.71f, 1.0f};
    *(vertices + 8) = {-0.63f, -0.13f, 1.0f};
    *(vertices + 9) = {-0.51f, 0.00f, 1.0f};
    *(vertices + 10) = {-0.64f, 0.10f, 1.0f};
}

float DegreeToRadians(float degree) {
    return degree * pi / 180.0f;
}

// Inicializar
void InitShip() {
    *(points+0) = {cosf(DegreeToRadians(0.0f)) * 25, sinf(DegreeToRadians(0.0f)) * 50, 1.0f};
    *(points+1) = {cosf(DegreeToRadians(160.0f)) * 15, sinf(DegreeToRadians(160.0f)) * 20, 1.0f};
    *(points+2) = {cosf(DegreeToRadians(170.0f)) * 10, sinf(DegreeToRadians(170.0f)) * 10, 1.0f};
    *(points+3) = {cosf(DegreeToRadians(-170.0f)) * 10, sinf(DegreeToRadians(-170.0f)) * 10, 1.0f};
    *(points+4) = {cosf(DegreeToRadians(-160.0f)) * 15, sinf(DegreeToRadians(-160.0f)) * 20, 1.0f};

    shipPlayer.centralPoint = {windowX / 2, windowY / 2, 1.0f};
    shipPlayer.points = points;

    shipPlayer.shoots = (Shoot*) malloc(sizeof(Shoot) * 4);

    // inicializamos memory para que tenga siempre algo y no crashea, se podria iniciar a nullptr
    for (int i = 0; i < shipPlayer.numberShoots; i++) {
        shipPlayer.shoots[i].isVisible = false;

        shipPlayer.shoots[i].points = (esat::Vec3*)malloc(sizeof(esat::Vec3) * 1);
    }
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
        fseek(file, 3 + 14 + 14 + 10 + 14 + 14 + sizeof(bool) + sizeof(int), SEEK_CUR);

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

    usersToShow = (User*) malloc(countUsersNotDeleted * 83);
    if (usersToShow == NULL) {
        printf("No hay memoria\n");
        fclose(file);
        return;
    }

    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);
    char* tmpPass = (char*) malloc(15);
    char* tmpBirth = (char*) malloc(11);
    char* tmpProvince = (char*) malloc(15);
    char* tmpEmail = (char*) malloc(15);

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
        fread(tmpBirth, 10, 1, file); tmpBirth[10] = '\0';
        fread(tmpProvince, 14, 1, file); tmpProvince[14] = '\0';
        fread(tmpEmail, 14, 1, file); tmpEmail[14] = '\0';
        fread(&admin, sizeof(admin), 1, file);
        fread(&credits, sizeof(credits), 1, file);
        fread(&isDeleted, sizeof(isDeleted), 1, file);
        fread(&puntuation, sizeof(puntuation), 1, file);

        if (!isDeleted) {

            // fumada histórica, para copiar en bloques de memoria, memcpy, parecido al strcpy
            unsigned char* ptr = ((unsigned char*)usersToShow) + index * 83;

            memcpy(ptr + OFFSET_ID, &id, 4);
            memcpy(ptr + OFFSET_NICK, tmpNick, 3);
            memcpy(ptr + OFFSET_USER, tmpUser, 14);
            memcpy(ptr + OFFSET_PASS, tmpPass, 14);
            memcpy(ptr + OFFSET_BIRTHDAY, tmpBirth, 10);
            memcpy(ptr + OFFSET_PROVINCE, tmpProvince, 14);
            memcpy(ptr + OFFSET_EMAIL, tmpEmail, 14);
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

void GenerateSemilla() {
    srand(time(NULL));
}

void LoadUsersOrdered() {
    file = fopen("users.dat", "r+b");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    usersOrdered = (User*) malloc(10 * 83);
    if (usersOrdered == NULL) {
        printf("No hay memoria ni jugadores para asociar puntuacion \n");
        fclose(file);
        return;
    }

    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);
    char* tmpPass = (char*) malloc(15);
    char* tmpBirth = (char*) malloc(11);
    char* tmpProvince = (char*) malloc(15);
    char* tmpEmail = (char*) malloc(15);

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
        fread(tmpBirth, 10, 1, file); tmpBirth[10] = '\0';
        fread(tmpProvince, 14, 1, file); tmpProvince[14] = '\0';
        fread(tmpEmail, 14, 1, file); tmpEmail[14] = '\0';
        fread(&admin, sizeof(admin), 1, file);
        fread(&credits, sizeof(credits), 1, file);
        fread(&isDeleted, sizeof(isDeleted), 1, file);
        fread(&puntuation, sizeof(puntuation), 1, file);

        if (isDeleted) continue;

        int pos = 0;
        for (; pos < count; pos++) {
            unsigned char* current = ((unsigned char*)usersOrdered) + pos * 83;

            int currentScore;
            memcpy(&currentScore, current + OFFSET_PUNTUA, 4);

            if (puntuation > currentScore) {
                break;
            }
        }

        if (pos < 10) {

            for (int j = (count < 10 ? count : 9); j > pos; j--) {
                unsigned char* destiny = ((unsigned char*)usersOrdered) + j * 83;
                unsigned char* source = ((unsigned char*)usersOrdered) + (j - 1) * 83;

                memcpy(destiny, source, 83);
            }

            unsigned char* ptr = ((unsigned char*)usersOrdered) + pos * 83;

            memcpy(ptr + OFFSET_ID, &id, 4);
            memcpy(ptr + OFFSET_NICK, tmpNick, 3);
            memcpy(ptr + OFFSET_USER, tmpUser, 14);
            memcpy(ptr + OFFSET_PASS, tmpPass, 14);
            memcpy(ptr + OFFSET_BIRTHDAY, tmpBirth, 10);
            memcpy(ptr + OFFSET_PROVINCE, tmpProvince, 14);
            memcpy(ptr + OFFSET_EMAIL, tmpEmail, 14);
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

void CalculateAsteroidsPerLevel(int level) {
    int selectedAsteroidsPerLevel = 0;
    int indexAsteroid = 0;
    int nextIndex = 0;

    while (indexAsteroid < totalAsteroidsPerLevels) {

        AsteroidsType type;

        int r = rand() % 4;

        switch (r) {
            case 0: type = AsteroidsType::V1; asteroidsV1Count++; break;
            case 1: type = AsteroidsType::V2; asteroidsV2Count++; break;
            case 2: type = AsteroidsType::V3; asteroidsV3Count++; break;
            case 3: type = AsteroidsType::V4; asteroidsV4Count++; break;
        }

        asteroids[indexAsteroid].type = type;

        indexAsteroid++;
    }
}

void InitAsteroids() {
    
    for (int i = 0; i < totalAsteroidsPerLevels; i++) {

        int count = 0;

        switch (asteroids[i].type) {
            case V1:
                count = numPointsAsteroidsV1;
                break;
            case V2:
                count = numPointsAsteroidsV2;
                break;
            case V3:
                count = numPointsAsteroidsV3;
                break;
            case V4:
                count = numPointsAsteroidsV4;
                break;
        }

        asteroids[i].numVertices = count;

        asteroids[i].vertices = (esat::Vec3*)malloc(sizeof(esat::Vec3) * count);

        asteroids[i].level = AsteroidsLevel::LEVEL_3;
        asteroids[i].isAlive = true;

        float speedX = rand()%1000 / 1000.0f;
        float speedY = rand()%1000 / 1000.0f;

        int mOrD = rand()%2;

        float centralPointX = rand()% (int) windowX;
        float centralPointY = rand()% (int) windowY;

        asteroids[i].direction.x = cosf(speedX * (mOrD == 1 ? 1 : -1));
        asteroids[i].direction.y = sinf(speedY * (mOrD == 1 ? 1 : -1));

        asteroids[i].centralPoint.x = centralPointX;
        asteroids[i].centralPoint.y = centralPointY;

        switch (asteroids[i].type) {
            case V1: VertsAsteroid1(asteroids[i].vertices); break;
            case V2: VertsAsteroid2(asteroids[i].vertices); break;
            case V3: VertsAsteroid3(asteroids[i].vertices); break;
            case V4: VertsAsteroid4(asteroids[i].vertices); break;
        }
    }
}

void ResetConfig() {
    asteroidsV1Count = 0;
    asteroidsV2Count = 0;
    asteroidsV3Count = 0;
    asteroidsV4Count = 0;

    // checkeamos si hay asteroids y sino metemos clean a vertices y asteroids
    if (asteroids != nullptr) {

        for (int i = 0; i < totalAsteroidsPerLevels; i++) {
            if (asteroids[i].vertices != nullptr) {
                free(asteroids[i].vertices);
                asteroids[i].vertices = nullptr;
            }
        }

        //free(asteroids);
        //asteroids = nullptr;
    }
}

void LevelConfig(int level) {
    //Resetear contadores de asteroids
    ResetConfig();

    switch (level) {
        case 1:
            totalAsteroidsPerLevels = 4;
        break;
        case 2:
            totalAsteroidsPerLevels = 5;
        break;
        case 3:
            totalAsteroidsPerLevels = 6;
        break;
        case 4:
            totalAsteroidsPerLevels = 7;
        break;
        case 5:
            totalAsteroidsPerLevels = 8;
        break;
        case 6:
            totalAsteroidsPerLevels = 9;
        break;
        case 7:
            totalAsteroidsPerLevels = 10;
        break;
        case 8:
            totalAsteroidsPerLevels = 11;
        break;
        case 9:
            totalAsteroidsPerLevels = 12;
        break;
    }

    for (int i = 0; i < totalAsteroidsPerLevels; i++) {
        asteroids[i].vertices = nullptr;
    }

    CalculateAsteroidsPerLevel(level);
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

    actualLevel = 1;
    asteroids = (Asteroids*)malloc(sizeof(Asteroids) * maxAsteroids);
    LevelConfig(actualLevel);
    InitAsteroids();
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
        char* u = ((char*)usersToShow) + i * 83;

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
    char* tmpBirth = (char*) malloc(11);
    char* tmpProvince = (char*) malloc(15);
    char* tmpEmail = (char*) malloc(15);

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
        char* u = ((char*)usersOrdered) + i * 83;

        memcpy(tmpNick, u + OFFSET_NICK, 3);
        tmpNick[3] = '\0';

        memcpy(tmpUser, u + OFFSET_USER, 14);
        tmpUser[14] = '\0';

        memcpy(tmpPass, u + OFFSET_PASS, 14);
        tmpPass[14] = '\0';

        memcpy(tmpBirth, u + OFFSET_BIRTHDAY, 10);
        tmpBirth[10] = '\0';

        memcpy(tmpProvince, u + OFFSET_PROVINCE, 14);
        tmpProvince[14] = '\0';

        memcpy(tmpEmail, u + OFFSET_EMAIL, 14);
        tmpEmail[14] = '\0';

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
                if (stickPosition.y == windowY / 4) {
                    stickPosition.y = windowY / 3.2f;
                } else if (stickPosition.y == windowY / 3.2f) {
                    stickPosition.y = windowY / 2.6f;
                } else if (stickPosition.y == windowY / 2.6f) {
                    stickPosition.y = windowY / 2.2f;
                } else if (stickPosition.y == windowY / 2.2f) {
                    stickPosition.y = windowY / 1.9f;
                } else if (stickPosition.y == windowY / 1.9f) {
                    stickPosition.y = windowY / 1.7f;
                } else if (stickPosition.y == windowY / 1.7f) {
                    stickPosition.y = windowY / 1.5f;
                } else if (stickPosition.y == windowY - 50) {
                    stickPosition.y = windowY / 4;
                } else {
                    stickPosition.y = windowY - 50;
                }

                currentEditField = (currentEditField + 1) % 8;
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
                if (stickPosition.y == windowY / 4) {
                    stickPosition.y = windowY / 3.2f;
                } else if (stickPosition.y == windowY / 3.2f) {
                    stickPosition.y = windowY / 2.6f;
                } else if (stickPosition.y == windowY / 2.6f) {
                    stickPosition.y = windowY / 2.2f;
                } else if (stickPosition.y == windowY / 2.2f) {
                    stickPosition.y = windowY / 1.9f;
                } else if (stickPosition.y == windowY / 1.9f) {
                    stickPosition.y = windowY / 1.7f;
                } else if (stickPosition.y == windowY / 1.7f) {
                    stickPosition.y = windowY / 1.5f;
                } else if (stickPosition.y == windowY - 50) {
                    stickPosition.y = windowY / 4;
                } else {
                    stickPosition.y = windowY - 50;
                }

                currentField = (currentField + 1) % 8;
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
    user.birthday = birthday;
    user.province = province;
    user.email = email;
    user.credits = credits;
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

    fwrite(&user.id, sizeof(int), 1, file);
    fwrite(user.nickname, 3, 1, file);
    fwrite(user.userPlayer, 14, 1, file);
    fwrite(user.password, 14, 1, file);
    fwrite(user.birthday, 10, 1, file);
    fwrite(user.province, 14, 1, file);
    fwrite(user.email, 14, 1, file);
    fwrite(&user.isAdmin, 1, 1, file);
    fwrite(&user.credits, sizeof(int), 1, file);
    fwrite(&user.isDeleted, 1, 1, file);
    fwrite(&user.puntuation, sizeof(int), 1, file);

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
    char* tmpBirth = (char*) malloc(10);
    char* tmpProvince = (char*) malloc(15);
    char* tmpEmail = (char*) malloc(15);

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
        fread(tmpBirth, 10, 1, f); tmpBirth[10] = '\0';
        fread(tmpProvince, 14, 1, f); tmpProvince[14] = '\0';
        fread(tmpEmail, 14, 1, f); tmpEmail[14] = '\0';
        fread(&admin, sizeof(admin), 1, f);
        fread(&credits, sizeof(credits), 1, f);
        fread(&isDeleted, sizeof(isDeleted), 1, f);
        fread(&puntuation, sizeof(puntuation), 1, f);

        // Necesario para la salud mental
        printf("id=%d", id);
        printf(" nickname='%s'", tmpNick);
        printf(" userPlayer='%s'", tmpUser);
        printf(" password='%s'", tmpPass);
        printf(" birthday='%s'", tmpBirth);
        printf(" province='%s'", tmpProvince);
        printf(" email='%s'", tmpEmail);
        printf(" isAdmin=%d", admin);
        printf(" credits=%d", credits);
        printf(" isDeleted=%d", isDeleted);
        printf(" puntuation=%d \n", puntuation);

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

            fseek(file, 3 + 14 + 14 + 10 + 14 + 14 + sizeof(bool) + sizeof(int), SEEK_CUR);

            fwrite(&deleted, sizeof(bool), 1, file);
            break;
        }

        // saltamos directamente al siguiente usuario saltando correctamente la memoria
        fseek(file, 3 + 14 + 14 + 10 + 14 + 14 + sizeof(bool) + sizeof(int) + sizeof(bool) + sizeof(int), SEEK_CUR);

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
        fseek(file, 3 + 14 + 14 + 10 + 14 + 14 + sizeof(bool) + sizeof(int), SEEK_CUR);

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
        for (int i = 48; i <= 57; i++) {

            // añadimos / cada 2 numeros excepto final
            if (birthdayLength == 2 || birthdayLength == 5) {
                birthdayLength++;
                birthday = (char*)realloc(birthday, birthdayLength + 1);
                *(birthday + birthdayLength - 1) = '/';
                *(birthday + birthdayLength) = '\0';
            }

            if (esat::IsKeyDown(i)  && birthdayLength < birthdayMaxLength) {
                birthdayLength++;
                birthday = (char*) realloc(birthday, birthdayLength + 1);
                *(birthday+birthdayLength - 1) = (char)i;
                *(birthday+birthdayLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && birthdayLength > 0) {
            if (*(birthday + birthdayLength - 1) == '/') {
                birthdayLength -= 2;
                if (birthdayLength < 0) birthdayLength = 0;
            } else {
                birthdayLength--;
            }

            *(birthday + birthdayLength) = '\0';
        }
    }

    if (currentField == 4) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && provinceLength < provinceMaxLength) {
                provinceLength++;
                province = (char*) realloc(province, provinceLength + 1);
                *(province+provinceLength - 1) = (char)i;
                *(province+provinceLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && provinceLength > 0) {
            provinceLength--;
            *(province+provinceLength) = '\0';
        }
    }

    if (currentField == 5) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && emailLength < emailMaxLength) {
                emailLength++;
                email = (char*) realloc(email, emailLength + 1);
                *(email+emailLength - 1) = (char)i;
                *(email+emailLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && emailLength > 0) {
            emailLength--;
            *(email+emailLength) = '\0';
        }
    }

    if (currentField == 6) {
        for (int i = '0'; i <= '9'; i++) {
            if (esat::IsKeyDown(i)) {
                int digit = i - '0';

                if (credits <= creditsMax / 10) {
                    credits = credits * 10 + digit;
                }
            }
        }

        // BORRAR (quitar última cifra)
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) {
            credits = credits / 10;
        }
    }

    if (currentField == 7) {
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

            long pos = ftell(file);

            fseek(file, pos - sizeof(int), SEEK_SET);

            fseek(file, sizeof(int), SEEK_CUR);

            fwrite(nicknameEdit, 3, 1, file);
            fwrite(userPlayerEdit, 14, 1, file);
            fwrite(passwordEdit, 14, 1, file);
            fwrite(birthdayEdit, 10, 1, file);
            fwrite(provinceEdit, 14, 1, file);
            fwrite(emailEdit, 14, 1, file);
            fwrite(0, 1, 1, file);
            fwrite(&creditsEdit, 4, 1, file);

            break;
        }

        fseek(file, 3 + 14 + 14 + 10 + 14 + 14 + sizeof(bool) + sizeof(int) + sizeof(bool) + sizeof(int), SEEK_CUR);

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
        for (int i = 48; i <= 57; i++) {

            // añadimos / cada 2 numeros excepto final
            if (birthdayEditLength == 2 || birthdayEditLength == 5) {
                birthdayEditLength++;
                birthdayEdit = (char*)realloc(birthdayEdit, birthdayEditLength + 1);
                *(birthdayEdit + birthdayEditLength - 1) = '/';
                *(birthdayEdit + birthdayEditLength) = '\0';
            }

            if (esat::IsKeyDown(i)  && birthdayEditLength < birthdayEditMaxLength) {
                birthdayEditLength++;
                birthdayEdit = (char*) realloc(birthdayEdit, birthdayEditLength + 1);
                *(birthdayEdit+birthdayEditLength - 1) = (char)i;
                *(birthdayEdit+birthdayEditLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && birthdayEditLength > 0) {
            if (*(birthdayEdit + birthdayEditLength - 1) == '/') {
                birthdayEditLength -= 2;
                if (birthdayEditLength < 0) birthdayEditLength = 0;
            } else {
                birthdayEditLength--;
            }

            *(birthdayEdit + birthdayEditLength) = '\0';
        }
    }

    if (currentEditField == 4) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && provinceEditLength < provinceEditMaxLength) {
                provinceEditLength++;
                provinceEdit = (char*) realloc(provinceEdit, provinceEditLength + 1);
                *(provinceEdit+provinceEditLength - 1) = (char)i;
                *(provinceEdit+provinceEditLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && provinceEditLength > 0) {
            provinceEditLength--;
            *(provinceEdit+provinceEditLength) = '\0';
        }
    }

    if (currentEditField == 5) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && emailEditLength < emailEditMaxLength) {
                emailEditLength++;
                emailEdit = (char*) realloc(emailEdit, emailEditLength + 1);
                *(emailEdit+emailEditLength - 1) = (char)i;
                *(emailEdit+emailEditLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && emailEditLength > 0) {
            emailEditLength--;
            *(emailEdit+emailEditLength) = '\0';
        }
    }

    if (currentEditField == 6) {
        for (int i = '0'; i <= '9'; i++) {
            if (esat::IsKeyDown(i)) {
                int digit = i - '0';

                if (creditsEdit <= creditsMaxEdit / 10) {
                    creditsEdit = creditsEdit * 10 + digit;
                }
            }
        }

        // BORRAR (quitar última cifra)
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) {
            creditsEdit = creditsEdit / 10;
        }
    }

    if (currentEditField == 7) {
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
    esat::DrawSetTextSize(15);

    esat::DrawText(windowX / 5, windowY / 4, "NICKNAME: ");

    esat::DrawText(windowX / 5, windowY / 3.2f, "USER: ");
    
    esat::DrawText(windowX / 5, windowY / 2.6f, "PASSWORD: ");

    esat::DrawText(windowX / 5, windowY / 2.2f, "BIRTHDAY: ");

    esat::DrawText(windowX / 5, windowY / 1.9f, "PROVINCE: ");

    esat::DrawText(windowX / 5, windowY / 1.7f, "EMAIL: ");

    esat::DrawText(windowX / 5, windowY / 1.5f, "CREDITS: ");

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 2.5f, windowY - 50, "SAVE");

    esat::DrawSetTextSize(12);
    // nickname
    esat::DrawText(windowX / 2, windowY / 4, nickname);

    esat::DrawText(windowX / 2, windowY / 3.2f, userPlayer);

    char hiddenPass[50] = "";
    for(int i=0; i<passwordLength; i++) *(hiddenPass+i) = '*';
    hiddenPass[passwordLength] = '\0';

    esat::DrawSetTextSize(20);
    esat::DrawText(windowX / 2, windowY / 2.6f, hiddenPass);

    esat::DrawSetTextSize(15);
    esat::DrawText(windowX / 2, windowY / 2.2f, birthday);
    esat::DrawText(windowX / 2, windowY / 1.9f, province);
    esat::DrawText(windowX / 2, windowY /1.7f, email);

    char creditsText[20];
    sprintf(creditsText, "%d", credits);
    esat::DrawText(windowX / 2, windowY / 1.5f, creditsText);


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

    esat::DrawSetTextSize(15);

    esat::DrawText(windowX / 5, windowY / 4, "NICKNAME: ");

    esat::DrawText(windowX / 5, windowY / 3.2f, "USER: ");
    
    esat::DrawText(windowX / 5, windowY / 2.6f, "PASSWORD: ");

    esat::DrawText(windowX / 5, windowY / 2.2f, "BIRTHDAY: ");

    esat::DrawText(windowX / 5, windowY / 1.9f, "PROVINCE: ");

    esat::DrawText(windowX / 5, windowY / 1.7f, "EMAIL: ");

    esat::DrawText(windowX / 5, windowY / 1.5f, "CREDITS: ");

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 2.5f, windowY - 50, "SAVE");

    esat::DrawSetTextSize(12);
    // nickname
    esat::DrawText(windowX / 2, windowY / 4, nicknameEdit);

    esat::DrawText(windowX / 2, windowY / 3.2f, userPlayerEdit);

    // tema de password
    char hiddenPass[50] = "";
    for(int i=0; i<passwordEditLength; i++) *(hiddenPass+i) = '*';
    hiddenPass[passwordEditLength] = '\0';

    esat::DrawSetTextSize(20);
    esat::DrawText(windowX / 2, windowY / 2.6f, hiddenPass);

    esat::DrawSetTextSize(15);
    esat::DrawText(windowX / 2, windowY / 2.2f, birthdayEdit);
    esat::DrawText(windowX / 2, windowY / 1.9f, provinceEdit);
    esat::DrawText(windowX / 2, windowY /1.7f, emailEdit);

    char creditsText[20];
    sprintf(creditsText, "%d", creditsEdit);
    esat::DrawText(windowX / 2, windowY / 1.5f, creditsText);

    DrawBack();
}

esat::Mat3 UpdateFigurita(esat::Vec2 scale, float angle, esat::Vec2 whereMove) {

    esat::Mat3 m = esat::Mat3Identity();
    m = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 0.0f), m);
    m = esat::Mat3Multiply(esat::Mat3Scale(scale.x, scale.y), m);
    m = esat::Mat3Multiply(esat::Mat3Rotate(angle), m);
    m = esat::Mat3Multiply(esat::Mat3Translate(whereMove.x, whereMove.y), m);

    return m;
}

void DrawFigurita(esat::Mat3 m) {

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
    
    // dibujar vector director 
    /*esat::DrawLine(
        shipPlayer.centralPoint.x, 
        shipPlayer.centralPoint.y,
        shipPlayer.vectorDirector.x,
        shipPlayer.vectorDirector.y
    );*/
}

void UpdateGame() {

    if (pendingLevelChange) {


        pendingLevelChange = false;

        pendingLevel = actualLevel + 1;

        if (pendingLevel > 9) {
            pendingLevel = 9;
        }

        actualLevel = pendingLevel;

        LevelConfig(actualLevel);
        InitAsteroids();
    }
}

void UpdateAsteroids() {
    for (int i = 0; i < totalAsteroidsPerLevels; i++) {

        asteroids[i].centralPoint.x += asteroids[i].direction.x;
        asteroids[i].centralPoint.y += asteroids[i].direction.y;

        if (asteroids[i].centralPoint.x > windowX) {
            asteroids[i].centralPoint.x = 0;
        } else if (asteroids[i].centralPoint.x < 0) {
            asteroids[i].centralPoint.x = windowX;
        }

        if (asteroids[i].centralPoint.y > windowY) {
            asteroids[i].centralPoint.y = 0;
        } else if (asteroids[i].centralPoint.y < 0) {
            asteroids[i].centralPoint.y = windowY;
        }
    }
}

void DrawAsteroidsVer(Asteroids* asteroid) {
    // static con un maximo
    static float points[24 * 2];
    esat::DrawSetFillColor(0, 0, 0, 0);
    esat::DrawSetStrokeColor(255, 255, 255, 255);

    for (int i = 0; i < asteroid->numVertices; i++) {
        
        float scale = 25.0f;
        switch (asteroid->level) {
            case AsteroidsLevel::LEVEL_1:
                scale *= 1;
            break;
            case AsteroidsLevel::LEVEL_2:
                scale *= 2;
            break;
            case AsteroidsLevel::LEVEL_3:
                scale *= 3;
            break;
        }

        points[i * 2] = (asteroid->vertices[i].x  * scale) + asteroid->centralPoint.x;
        points[i * 2 + 1] = (asteroid->vertices[i].y * scale) + asteroid->centralPoint.y;
    }

    esat::DrawSolidPath(points, asteroid->numVertices, true);
}

void FireShoot() {
    float bulletSpeed = 10.0f;
    float offsetInsideShip = 20.0f;

    for (int i = 0; i < shipPlayer.numberShoots; i++) {
        if (!shipPlayer.shoots[i].isVisible) {

            shipPlayer.shoots[i].isVisible = true;

            // no puedo usar shipPlayer.points[lo que sea], porque esta en local, no en global
            // creamos offset fictio usando el vector director
            shipPlayer.shoots[i].points[0].x = shipPlayer.centralPoint.x + cosf(shipPlayer.angle) * offsetInsideShip;
            shipPlayer.shoots[i].points[0].y = shipPlayer.centralPoint.y + sinf(shipPlayer.angle) * offsetInsideShip;
            shipPlayer.shoots[i].points[0].z = 1.0f;

            shipPlayer.shoots[i].vectorDirector.x = cosf(shipPlayer.angle) * bulletSpeed + shipPlayer.speed.x;
            shipPlayer.shoots[i].vectorDirector.y = sinf(shipPlayer.angle) * bulletSpeed + shipPlayer.speed.y;

            shipPlayer.shoots[i].distanceTravelled = 0.0f;
            shipPlayer.shoots[i].maxDistanceTravelled = 400.0f;

            break;
        }
    }
}

void UpdateShoots() {

    const float speed = 8.0f;

    for (int i = 0; i < shipPlayer.numberShoots; i++) {

        if (shipPlayer.shoots[i].isVisible) {

            float distanceX = shipPlayer.shoots[i].vectorDirector.x;
            float distanceY = shipPlayer.shoots[i].vectorDirector.y;

            shipPlayer.shoots[i].points[0].x += distanceX;
            shipPlayer.shoots[i].points[0].y += distanceY;

            shipPlayer.shoots[i].distanceTravelled += speed;

            if (shipPlayer.shoots[i].distanceTravelled >= shipPlayer.shoots[i].maxDistanceTravelled) {

                shipPlayer.shoots[i].isVisible = false;

                shipPlayer.shoots[i].distanceTravelled = 0.0f;

                shipPlayer.shoots[i].points[0].x = shipPlayer.points[0].x;
                shipPlayer.shoots[i].points[0].y = shipPlayer.points[0].y;
            }
        }
    }
}

esat::Vec2 CalculateVectorDirector(esat::Vec3 point1, esat::Vec3 point2) {

    esat::Vec2 vectorDirector = {0, 0};

    vectorDirector.x = point2.x - point1.x;
    vectorDirector.y = point2.y - point1.y;

    return vectorDirector;
}

bool CollisionDetected(esat::Vec2 vectorDirector, esat::Vec2 vectorDirector2) {
    float x, y;
    //esat::Vec2 vectorDirector1, vectorDirector2;

    // vectorDirector1 = 

    return true;
}

void DrawShoots() {
    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 255);

    for (int i = 0; i < shipPlayer.numberShoots; i++) {
        if (shipPlayer.shoots[i].isVisible) {

            // punta de la nave
            float distanceX1 = shipPlayer.shoots[i].points[0].x;
            float distanceY1 = shipPlayer.shoots[i].points[0].y;

            float distanceX2 = shipPlayer.shoots[i].vectorDirector.x;
            float distanceY2 = shipPlayer.shoots[i].vectorDirector.y;

            float line[4] = {
                distanceX1, distanceY1,
                distanceX1 - distanceX2, distanceY1 - distanceY2
            };

            esat::DrawSolidPath(line, 2, false);
        }
    }
}

void DrawAsteroids() {
    for (int i = 0; i < totalAsteroidsPerLevels; i++) {
        if (asteroids[i].isAlive) {
            DrawAsteroidsVer(&asteroids[i]);
        }
    }
}

void ActivateNewAsteroid(Asteroids asteroid) {

    int count = 0;

    switch (asteroid.type) {
        case V1:
            count = numPointsAsteroidsV1;
            break;
        case V2:
            count = numPointsAsteroidsV2;
            break;
        case V3:
            count = numPointsAsteroidsV3;
            break;
        case V4:
            count = numPointsAsteroidsV4;
        break;
    }

    asteroids[totalAsteroidsPerLevels].numVertices = count;

    asteroids[totalAsteroidsPerLevels].vertices = (esat::Vec3*)malloc(sizeof(esat::Vec3) * count);

    asteroids[totalAsteroidsPerLevels].isAlive = true;
    asteroids[totalAsteroidsPerLevels].level = asteroid.level;
    asteroids[totalAsteroidsPerLevels].type = asteroid.type;

    float speedX = rand()%1000 / 1000.0f;
    float speedY = rand()%1000 / 1000.0f;

    int mOrD = rand()%2;

    // float centralPointX = asteroid.centralPoint.x;
    // float centralPointY = asteroid.centralPoint.y;

    asteroids[totalAsteroidsPerLevels].direction.x = cosf(speedX * (mOrD == 1 ? 1 : -1));
    asteroids[totalAsteroidsPerLevels].direction.y = sinf(speedY * (mOrD == 1 ? 1 : -1));

    asteroids[totalAsteroidsPerLevels].centralPoint = asteroid.centralPoint;
    //asteroids[totalAsteroidsPerLevels].centralPoint.y = asteroid.centralPoint.y;

    switch (asteroids[totalAsteroidsPerLevels].type) {
        case V1: VertsAsteroid1(asteroids[totalAsteroidsPerLevels].vertices); break;
        case V2: VertsAsteroid2(asteroids[totalAsteroidsPerLevels].vertices); break;
        case V3: VertsAsteroid3(asteroids[totalAsteroidsPerLevels].vertices); break;
        case V4: VertsAsteroid4(asteroids[totalAsteroidsPerLevels].vertices); break;
    }

    totalAsteroidsPerLevels++;
}

void BrokeAsteroid(Asteroids* asteroid_broke) {

    if (asteroid_broke->level == AsteroidsLevel::LEVEL_1) {
        asteroid_broke->isAlive = false;
        return;
    }

    switch (asteroid_broke->level) {
        case AsteroidsLevel::LEVEL_3:
            asteroid_broke->level = LEVEL_2;
        break;
        case AsteroidsLevel::LEVEL_2:
            asteroid_broke->level = LEVEL_1;
        break;
    }
    
    ActivateNewAsteroid(*asteroid_broke);
}

int esat::main(int argc, char **argv) {

    esat::WindowInit(windowX, windowY);
    esat::WindowSetMouseVisibility(true);

    GenerateSemilla();
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
                UpdateAsteroids();
                if (!isChangingLevel) {
                    DrawAsteroids();
                }
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
                UpdateAsteroids();
                if (!isChangingLevel) {
                    DrawAsteroids();
                }
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

                if (esat::IsKeyDown('I')) {
                    pendingLevelChange = true;
                }

                if (esat::IsKeyDown('O')) {
                    BrokeAsteroid(&asteroids[0]);
                }

                UpdateAsteroids();
                UpdateGame();

                if (esat::IsSpecialKeyDown(esat::kSpecialKey_Space)) {
                    FireShoot();
                }

                UpdateShoots();

                for (int i = 0; i < 5; i++) {
                    esat::Vec3 nextVertexToSee = shipPlayer.points[0];
                    if (i + 1 < 5) {
                        esat::Vec3 nextVertexToSee = shipPlayer.points[i + 1];
                    }
                    
                    esat::Vec2 vectorDirectorShip = CalculateVectorDirector(shipPlayer.points[i], nextVertexToSee);

                    /*for (int j = 0; j < totalAsteroidsPerLevels; j++) {
                        for (int k = 0; k < asteroids[j].num)
                    }*/

                }

                // all this shit is going into handle hell function
                // think about + and - acceleration
                if (esat::IsKeyPressed('D')) {
                    shipPlayer.angle += 0.1f;
                }

                if (esat::IsKeyPressed('A')) {
                    shipPlayer.angle -= 0.1f;
                }

                if (esat::IsKeyPressed('W')) {
                    shipPlayer.acceleration = {
                        cosf(shipPlayer.angle) * 0.06f,
                        sinf(shipPlayer.angle) * 0.06f
                    };

                    if (shipPlayer.speed.x + shipPlayer.acceleration.x > maxSpeed) {
                        shipPlayer.speed.x = maxSpeed;
                    } else {
                        shipPlayer.speed.x += shipPlayer.acceleration.x;
                    }
                    
                    if (shipPlayer.speed.y + shipPlayer.acceleration.y > maxSpeed) {
                        shipPlayer.speed.y = maxSpeed;
                    } else {
                        shipPlayer.speed.y += shipPlayer.acceleration.y;
                    }

                } else {
                    shipPlayer.speed.x *= deceleration;
                    shipPlayer.speed.y *= deceleration;
                }

                shipPlayer.centralPoint.x += shipPlayer.speed.x;
                shipPlayer.centralPoint.y += shipPlayer.speed.y;

                // calculinho del vector director
                float pointX = cosf(shipPlayer.angle) * 60.0f;
                float pointY = sinf(shipPlayer.angle) * 60.0f;

                shipPlayer.vectorDirector = {
                    shipPlayer.centralPoint.x + pointX,
                    shipPlayer.centralPoint.y + pointY
                };

                if (shipPlayer.centralPoint.x > windowX) {
                    shipPlayer.centralPoint.x = 0;
                } else if (shipPlayer.centralPoint.x < 0) {
                    shipPlayer.centralPoint.x = windowX;
                }

                if (shipPlayer.centralPoint.y > windowY) {
                    shipPlayer.centralPoint.y = 0;
                } else if (shipPlayer.centralPoint.y < 0) {
                    shipPlayer.centralPoint.y = windowY;
                }

                matriz = UpdateFigurita({1.0f, 1.0f}, shipPlayer.angle, {shipPlayer.centralPoint.x, shipPlayer.centralPoint.y});

                DrawFigurita(matriz);
                DrawShoots();
                if (!isChangingLevel) {
                    DrawAsteroids();
                }
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
