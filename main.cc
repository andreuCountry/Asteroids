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

float windowX = 800.0f, windowY = 608.0f, tempTime = 0, tempAskRegister = 0, tempInmortality = 0, tempStickBar = 0;

const int numPoints = 5, pi = 3.141592f;

esat::Vec3* points = (esat::Vec3*) malloc (numPoints * sizeof(esat::Vec3));

Game currentGame;

esat::Vec2 stickPosition, stickPositionEdit, stickLoginPosition, stickSecondLoginPosition, adminSectionStickPosition;

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

char* userSecondLogin = (char*) malloc (1);
int userSecondLoginLength = 0, userSecondLoginMaxLength = 15;

char* passwordSecondLogin = (char*) malloc (1);
int passwordSecondLoginLength = 0, passwordSecondLoginMaxLength = 15;

char* birthday = (char*) malloc (1);
int birthdayLength = 0, birthdayMaxLength = 11;

char* province = (char*) malloc (1);
int provinceLength = 0, provinceMaxLength = 15;

char* email = (char*) malloc (1);
int emailLength = 0, emailMaxLength = 15;

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

char* currentNick1 = (char*) malloc (4);
char* currentNick2 = (char*) malloc (4);

int creditsEdit = 0;
int creditsMaxEdit = 999999;
int creditsPlayer1 = 0, creditsPlayer2;

int currentField = 0, currentLoginField = 0, currentEditField = 0, currentSecondLoginField = 0;
int userId = 0;
int userPlayerId1 = 0, userPlayerId2 = 0;
int positionInPage = 1;

FILE* file, filePuntuation;

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

int puntuationInGame1 = 0, puntuationInGame2 = 0;

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
    esat::Vec2 centralPoint;
};

struct Ship {
    esat::Vec3* points;
    int lifes = 4;
    bool isAlive;
    esat::Vec3 centralPoint;
    esat::Vec2 speed = {0.0f, 0.0f};
    esat::Vec2 acceleration;
    float angle = 0.0f;
    esat::Vec3 vectorDirector;
    Shoot* shoots;
    int numberShoots = 4;
    bool inmortality = false;
    esat::Vec3 deadZone;
    bool showDeadZone = false;
    bool isSecondPlayer = false;
};

struct ShipFragment {
    esat::Vec2 point1;
    esat::Vec2 point2;
    float angle;
};

struct AsteroidFragment {
    esat::Vec2 point1;
    float angle;
    float speed;
    float life;
    bool isAlive = false;
};

ShipFragment* fragments = (ShipFragment*) malloc(5 * sizeof(ShipFragment));
AsteroidFragment* fragmentsAsteroids = (AsteroidFragment*) malloc(12 * sizeof(AsteroidFragment));

const int kCirclePoints = 20;

esat::Vec3* g_circle = (esat::Vec3*) malloc(kCirclePoints * sizeof(esat::Vec3));

float timeInmortality = 3.0f;
float timeDeadShip = 3.0f;

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
    esat::Vec3 deadZone;
    esat::Vec2 direction;
    bool isAlive;
    int numVertices;
    bool canCollide = false;
};

struct UFO {
    bool isAlive;
    int numVertices;
    bool canCollide = false;
    bool canMove = false;
    esat::Vec3 *vertices;
    esat::Vec3 centralPoint;
    esat::Vec2 direction;
    Shoot shoot;
    bool isLittle = false;
    bool canShoot = false;
};

int actualLevel, totalAsteroidsPerLevels;
int asteroidsV1Count = 0, asteroidsV2Count = 0, asteroidsV3Count = 0, asteroidsV4Count = 0;

const int numPointsAsteroidsV1 = 10, numPointsAsteroidsV2 = 12, numPointsAsteroidsV3 = 12, numPointsAsteroidsV4 = 11;

bool pendingLevelChange = false;
bool isChangingLevel = false;

Asteroids* asteroids = nullptr;
const int maxAsteroids = 140;
int activeAsteroids = 4;

int pendingLevel = -1;

UFO ufo;
float ufoSpawnTimer, ufoShootTimer = 0.0f;

// booleana para saber si estamos modo multiplayer
bool isMultiplayerActive = false;

bool playerHighscored = false;

// segundo jugador vidas para acabar el gameplay
int secondPlayerLifes = 4;

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

void VertsUFO(esat::Vec3 *vertices) {
    *(vertices) = {2.0f, -1.0f, 1.0f};
    *(vertices + 1) = {-2.0f, -1.0f, 1.0f};
    *(vertices + 2) = {-5.0f, 1.0f, 1.0f};
    *(vertices + 3) = {5.0f, 1.0f, 1.0f};
    *(vertices + 4) = {2.0f, 3.0f, 1.0f};
    *(vertices + 5) = {-2.0f, 3.0f, 1.0f};
    *(vertices + 6) = {-5.0f, 1.0f, 1.0f};
    *(vertices + 7) = {-2.0f, -1.0f, 1.0f};
    *(vertices + 8) = {-1.0f, -3.0f, 1.0f};
    *(vertices + 9) = {1.0f, -3.0f, 1.0f};
    *(vertices + 10) = {2.0f, -1.0f, 1.0f};
    *(vertices + 11) = {5.0f, 1.0f, 1.0f};
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
    shipPlayer.isAlive = true;

    shipPlayer.shoots = (Shoot*) malloc(sizeof(Shoot) * 4);

    // inicializamos memory para que tenga siempre algo y no crashea, se podria iniciar a nullptr
    for (int i = 0; i < shipPlayer.numberShoots; i++) {
        (*(shipPlayer.shoots + i)).isVisible = false;

        (*(shipPlayer.shoots + i)).points = (esat::Vec3*)malloc(sizeof(esat::Vec3) * 1);
        (*(shipPlayer.shoots + i)).centralPoint = {shipPlayer.centralPoint.x, shipPlayer.centralPoint.y};
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
        fread(tmpNick, 3, 1, file); *(tmpNick + 3) = '\0';
        fread(tmpUser, 14, 1, file); *(tmpUser + 14) = '\0';
        fread(tmpPass, 14, 1, file); *(tmpPass + 14) = '\0';
        fread(tmpBirth, 10, 1, file); *(tmpBirth + 10) = '\0';
        fread(tmpProvince, 14, 1, file); *(tmpProvince + 14) = '\0';
        fread(tmpEmail, 14, 1, file); *(tmpEmail + 14) = '\0';
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
    FILE* filePuntuation = fopen("puntuations.dat", "r+b");
    if (filePuntuation == NULL) {
        printf("Error opening file\n");
        return;
    }

    usersOrdered = (User*) malloc(10 * 21);
    if (usersOrdered == NULL) {
        printf("No hay memoria ni jugadores para asociar puntuacion \n");
        fclose(filePuntuation);
        return;
    }

    int puntuation;
    int auxPuntuation = 0;
    char* tmpNick = (char*) malloc(4);
    char* tmpUser = (char*) malloc(15);

    int count = 0;
    while (1) {

        long startPos = ftell(filePuntuation);

        if (fread(&puntuation, sizeof(int), 1, filePuntuation) != 1) {
            break;
        }

        fread(tmpNick, 3, 1, filePuntuation); 
        *(tmpNick + 3) = '\0';
        fread(tmpUser, 14, 1, filePuntuation); 
        *(tmpUser + 14) = '\0';

        int pos = 0;
        for (; pos < count; pos++) {
            unsigned char* current = ((unsigned char*)usersOrdered) + pos * 21;

            int currentScore;
            memcpy(&currentScore, current + 0, 4);

            if (puntuation > currentScore) {
                break;
            }
        }

        if (pos < 10) {

            for (int j = (count < 10 ? count : 9); j > pos; j--) {
                unsigned char* destiny = ((unsigned char*)usersOrdered) + j * 21;
                unsigned char* source = ((unsigned char*)usersOrdered) + (j - 1) * 21;

                memcpy(destiny, source, 21);
            }

            unsigned char* ptr = ((unsigned char*)usersOrdered) + pos * 21;

            memcpy(ptr + 0, &puntuation, 4);
            memcpy(ptr + 4, tmpNick, 3);
            memcpy(ptr + 7, tmpUser, 14);

            if (count < 10) count++;
        }

        // esta movida hace que nos saltemos correctamente todo el reguistro actual
        fseek(filePuntuation, startPos + 21, SEEK_SET);
    }

    fclose(filePuntuation);

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

        (*(asteroids + indexAsteroid)).type = type;

        indexAsteroid++;
    }
}

void InitAsteroids() {
    
    for (int i = 0; i < totalAsteroidsPerLevels; i++) {

        int count = 0;

        switch ((*(asteroids + i)).type) {
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

        (*(asteroids + i)).numVertices = count;

        (*(asteroids + i)).vertices = (esat::Vec3*)malloc(sizeof(esat::Vec3) * count);

        (*(asteroids + i)).level = AsteroidsLevel::LEVEL_3;
        (*(asteroids + i)).isAlive = true;
        (*(asteroids + i)).canCollide = true;

        float speedX = rand()%2000 / 1000.0f;
        float speedY = rand()%2000 / 1000.0f;

        int mOrD = rand()%2;

        float centralPointX = rand()% (int) windowX;
        float centralPointY = rand()% (int) windowY;

        (*(asteroids + i)).direction.x = cosf(speedX * (mOrD == 1 ? 1 : -1));
        (*(asteroids + i)).direction.y = sinf(speedY * (mOrD == 1 ? 1 : -1));

        (*(asteroids + i)).centralPoint.x = centralPointX;
        (*(asteroids + i)).centralPoint.y = centralPointY;

        switch ((*(asteroids + i)).type) {
            case V1: VertsAsteroid1((*(asteroids + i)).vertices); break;
            case V2: VertsAsteroid2((*(asteroids + i)).vertices); break;
            case V3: VertsAsteroid3((*(asteroids + i)).vertices); break;
            case V4: VertsAsteroid4((*(asteroids + i)).vertices); break;
        }
    }
}

void InitUfos() {
    ufo.numVertices = 12;

    ufo.vertices = (esat::Vec3*)malloc(sizeof(esat::Vec3) * 12);

    VertsUFO(ufo.vertices);

    ufo.isAlive = false;
    ufo.canCollide = false;

    float speedX = rand()%2000 / 1000.0f;
    float speedY = rand()%2000 / 1000.0f;

    int mOrD = rand()%2;

    float centralPointX = -50;
    float centralPointY = 100;

    ufo.direction.x = cosf(speedX);
    ufo.direction.y = 0;

    ufo.centralPoint.x = centralPointX;
    ufo.centralPoint.y = centralPointY;

    // ahora tema disparo

    ufo.shoot.isVisible = false;
    ufo.shoot.distanceTravelled = 0.0f;
    ufo.shoot.maxDistanceTravelled = 400.0f;

    ufo.shoot.points = (esat::Vec3*)malloc(sizeof(esat::Vec3) * 1);
}

void ResetConfig() {
    asteroidsV1Count = 0;
    asteroidsV2Count = 0;
    asteroidsV3Count = 0;
    asteroidsV4Count = 0;

    // checkeamos si hay asteroids y sino metemos clean a vertices y asteroids
    if (asteroids != nullptr) {

        for (int i = 0; i < totalAsteroidsPerLevels; i++) {
            if ((*(asteroids + i)).vertices != nullptr) {
                free((*(asteroids + i)).vertices);
                (*(asteroids + i)).vertices = nullptr;
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
        (*(asteroids + i)).vertices = nullptr;
    }

    CalculateAsteroidsPerLevel(level);
}

void InitCircle() {
    float angle = 6.283185f / (float)kCirclePoints;

    for (int i = 0; i < kCirclePoints; ++i) {
        (*(g_circle + i)).x = cosf(angle * i);
        (*(g_circle + i)).y = sinf(angle * i);
        (*(g_circle + i)).z = 1.0f;
    }
}

void InitConfig() {

    esat::DrawSetTextFont("./Recursos/Fuentes/horrendo.ttf");
    
    currentGame.actualScene = Scenes::MAIN_MENU;

    // Variables que se modifican con el paso del tiempo y de la lectura
    stickPosition.x = windowX / 7;
    stickPosition.y = windowY / 4;

    stickPositionEdit.x = windowX / 7;
    stickPositionEdit.y = windowY / 4;

    // Variables que se modifican con el login
    stickLoginPosition.x = windowX / 7;
    stickLoginPosition.y = windowY / 2.5f;

    stickSecondLoginPosition.x = windowX / 7;
    stickSecondLoginPosition.y = windowY / 2.5;

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

    // Seteo de los UFO's y de su timer
    InitUfos();
    ufoSpawnTimer = 0;

    // particulitas de los asteroids cuanto palman
    InitCircle();
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
        *(tmpNick + 3) = '\0';

        memcpy(tmpUser, u + OFFSET_USER, 14);
        *(tmpUser + 14) = '\0';

        memcpy(tmpPass, u + OFFSET_PASS, 14);
        *(tmpPass + 14) = '\0';

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

    int puntuation;

    char* puntuationBuffer = (char*) malloc(6);

    // recordamos usar el conteo de los usuarios que hay ordenados, por si hay menos de 10
    // validación pocha pero nos aseguramos de que no se inserte basura en memoria
    // además solo trabajamos con los usuarios que tenemos, con máximo de 10
    for (int i = 0; i < usersOrderedCount; i++) {
        char* u = ((char*)usersOrdered) + i * 21;

        memcpy(&puntuation, u + 0, 4);

        memcpy(tmpNick, u + 4, 3);
        *(tmpNick + 3) = '\0';

        memcpy(tmpUser, u + 7, 14);
        *(tmpUser + 14) = '\0';

        esat::DrawText(250, y, tmpNick);
        esat::DrawText(250 + 100, y, tmpUser);

        // romper bucle para que no muestre basura la conversacion de la puntuation en char *
        if (*(tmpNick + 0) == '\0') {
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

void DrawStickBarEdit() {

    int c = ((esat::Time()/100.0f) - tempStickBar);

    esat::DrawSetTextSize(24);

    if (c % 10 != 0) {
        esat::DrawText(stickPositionEdit.x, stickPositionEdit.y, "--o");
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

void DrawStickSecondLoginBar() {
    int c = ((esat::Time()/100.0f) - tempStickBar);

    esat::DrawSetTextSize(24);

    if (c % 10 != 0) {
        esat::DrawText(stickSecondLoginPosition.x, stickSecondLoginPosition.y, "--o");
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
            DrawStickBarEdit();

            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab)) {
                if (stickPositionEdit.y == windowY / 4) {
                    stickPositionEdit.y = windowY / 3.2f;
                } else if (stickPositionEdit.y == windowY / 3.2f) {
                    stickPositionEdit.y = windowY / 2.6f;
                } else if (stickPositionEdit.y == windowY / 2.6f) {
                    stickPositionEdit.y = windowY / 2.2f;
                } else if (stickPositionEdit.y == windowY / 2.2f) {
                    stickPositionEdit.y = windowY / 1.9f;
                } else if (stickPositionEdit.y == windowY / 1.9f) {
                    stickPositionEdit.y = windowY / 1.7f;
                } else if (stickPositionEdit.y == windowY / 1.7f) {
                    stickPositionEdit.y = windowY / 1.5f;
                } else if (stickPositionEdit.y == windowY - 50) {
                    stickPositionEdit.y = windowY / 4;
                } else {
                    stickPositionEdit.y = windowY - 50;
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
        case Scenes::ASK_SECOND_LOGIN:
            DrawStickSecondLoginBar();

            if (esat::IsSpecialKeyDown(esat::kSpecialKey_Tab)) {
                if (stickSecondLoginPosition.y == windowY / 2.5f) {
                    stickSecondLoginPosition.y = windowY / 2;
                } else if ( stickSecondLoginPosition.y == windowY / 2) {
                    stickSecondLoginPosition.y = windowY / 1.5f;
                } else if (stickSecondLoginPosition.y == windowY / 1.5f){
                    stickSecondLoginPosition.y = windowY / 2.5f;
                }

                currentSecondLoginField = (currentSecondLoginField + 1) % 3;
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
                    stickPosition.y = windowY - 50;
                } else if (stickPosition.y == windowY - 50) {
                    stickPosition.y = windowY / 4;
                } else {
                    stickPosition.y = windowY - 50;
                }

                currentField = (currentField + 1) % 7;
            }

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

bool CheckOptionalUser(bool secondUserEnable) {
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
        fread(tmpNick, 3, 1, f); *(tmpNick + 3) = '\0';
        fread(tmpUser, 14, 1, f); *(tmpUser + 14) = '\0';
        fread(tmpPass, 14, 1, f); *(tmpPass + 14) = '\0';
        fread(tmpBirth, 10, 1, f); *(tmpBirth + 10) = '\0';
        fread(tmpProvince, 14, 1, f); *(tmpProvince + 14) = '\0';
        fread(tmpEmail, 14, 1, f); *(tmpEmail + 14) = '\0';
        fread(&admin, sizeof(admin), 1, f);
        fread(&credits, sizeof(credits), 1, f);
        // asignacion de creditos para modificarlos despues en inicio de partida
        if (!secondUserEnable) {
            creditsPlayer1 = credits;
        } else {
            creditsPlayer2 = credits;
        }
        fread(&isDeleted, sizeof(isDeleted), 1, f);
        fread(&puntuation, sizeof(puntuation), 1, f);

        // Necesario para la salud mental
        /*printf("id=%d", id);
        printf(" nickname='%s'", tmpNick);
        printf(" userPlayer='%s'", tmpUser);
        printf(" password='%s'", tmpPass);
        printf(" birthday='%s'", tmpBirth);
        printf(" province='%s'", tmpProvince);
        printf(" email='%s'", tmpEmail);
        printf(" isAdmin=%d", admin);
        printf(" credits=%d", credits);
        printf(" isDeleted=%d", isDeleted);
        printf(" puntuation=%d \n", puntuation);*/

        if (secondUserEnable) {
            if ((strcmp(tmpUser, userSecondLogin) == 0) && (strcmp(tmpPass, passwordSecondLogin) == 0)) {
                isValid = true;
                memcpy(currentNick2, tmpNick, 4);
            }
        } else {
            if ((strcmp(tmpUser, userLogin) == 0) && (strcmp(tmpPass, passwordLogin) == 0)) {
                isValid = true;

                memcpy(currentNick1, tmpNick, 4);
            }
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

void HandleAskGameplay() {
    if (esat::IsKeyDown('S')) {

        if (creditsPlayer1 > 0) {
            timeInmortality = 3;
            shipPlayer.inmortality = true;
            currentGame.actualScene = GAMEPLAY;
        }
        
    }

    if (esat::IsKeyDown('M')) {
        timeInmortality = 3;
        shipPlayer.inmortality = true;
        isMultiplayerActive = true;
        currentGame.actualScene = ASK_SECOND_LOGIN;
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        currentGame.actualScene = LOAD_REGISTER;
    }
}

void HandleSecondLogin() {
    char character;

    if (currentSecondLoginField == 0) {
        for (character = 'A'; character <= 'Z'; character++) {

            if (esat::IsKeyDown(character) && userSecondLoginLength < userSecondLoginMaxLength) {
                userSecondLoginLength++;
                userSecondLogin = (char*) realloc(userSecondLogin, userSecondLoginLength + 1);
                *(userSecondLogin+userSecondLoginLength - 1) = character;
                *(userSecondLogin+userSecondLoginLength) = '\0';
            }
        }

        for (character = 'a'; character <= 'z'; character++) {

            if (esat::IsKeyDown(character) && userSecondLoginLength < userSecondLoginMaxLength) {
                userSecondLoginLength++;
                userSecondLogin = (char*) realloc(userSecondLogin, userSecondLoginLength + 1);
                *(userSecondLogin+userSecondLoginLength - 1) = character;
                *(userSecondLogin+userSecondLoginLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete)) &&
            userSecondLoginLength > 0) {
            userSecondLoginLength--;
            *(userSecondLogin + userSecondLoginLength) = '\0';
        }
    }

    if (currentSecondLoginField == 1) {
        for (int i = 32; i <= 126; i++) {
            if (esat::IsKeyDown(i)  && passwordSecondLoginLength < passwordSecondLoginMaxLength) {
                passwordSecondLoginLength++;
                passwordSecondLogin = (char*) realloc(passwordSecondLogin, passwordSecondLoginLength + 1);
                *(passwordSecondLogin+passwordSecondLoginLength - 1) = (char)i;
                *(passwordSecondLogin+passwordSecondLoginLength) = '\0';
            }
        }

        if ((esat::IsSpecialKeyDown(esat::kSpecialKey_Delete))
            && passwordSecondLoginLength > 0) {
            passwordSecondLoginLength--;
            *(passwordSecondLogin+passwordSecondLoginLength) = '\0';
        }
    }

    if (currentSecondLoginField == 2) {
        if (esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)) {
            bool optionalUser = CheckOptionalUser(true);

            if (isMultiplayerActive) {
                if (creditsPlayer1 <= 0 || creditsPlayer2 <= 0) {
                    optionalUser = false;
                }
            } else {
                if (creditsPlayer1 <= 0) {
                    optionalUser = false;
                }
            }

            if (optionalUser) {
                timeInmortality = 3;
                shipPlayer.inmortality = true;
                currentGame.actualScene = GAMEPLAY;
            }
        }
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        currentGame.actualScene = ASK_GAMEPLAY;
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
            bool optionalUser = CheckOptionalUser(false);

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
                    currentGame.actualScene = ASK_GAMEPLAY;
                }
            }
        }
    }

    if (esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace)) {
        currentGame.actualScene = ASK_REGISTER;
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
        currentGame.actualScene = ASK_GAMEPLAY;
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

            bool notAdmin = false;
            fwrite(nicknameEdit, 3, 1, file);
            fwrite(userPlayerEdit, 14, 1, file);
            fwrite(passwordEdit, 14, 1, file);
            fwrite(birthdayEdit, 10, 1, file);
            fwrite(provinceEdit, 14, 1, file);
            fwrite(emailEdit, 14, 1, file);
            fwrite(&notAdmin, 1, 1, file);
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

    // user
    esat::DrawText(windowX / 2, windowY / 2.5f, userLogin);

    // tema de password
    char hiddenPass[50] = "";

    for (int i = 0; i < passwordLoginLength; i++) {
        *(hiddenPass + i) = '*';
    }

    *(hiddenPass + passwordLoginLength) = '\0';

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

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 2.5f, windowY - 50, "SAVE");

    esat::DrawSetTextSize(12);
    // nickname
    esat::DrawText(windowX / 2, windowY / 4, nickname);

    esat::DrawText(windowX / 2, windowY / 3.2f, userPlayer);

    char hiddenPass[50] = "";
    for(int i=0; i<passwordLength; i++) *(hiddenPass+i) = '*';
    *(hiddenPass + passwordLength) = '\0';

    esat::DrawSetTextSize(20);
    esat::DrawText(windowX / 2, windowY / 2.6f, hiddenPass);

    esat::DrawSetTextSize(15);
    esat::DrawText(windowX / 2, windowY / 2.2f, birthday);
    esat::DrawText(windowX / 2, windowY / 1.9f, province);
    esat::DrawText(windowX / 2, windowY /1.7f, email);


    DrawBack();
}

void DrawAskGameplay() {

    esat::DrawSetFillColor(255, 255, 255, 255);

    int c = ((esat::Time()/100.0f) - tempAskRegister);

    if (c % 10 != 0) {
        esat::DrawSetTextSize(36);
        esat::DrawText(windowX / 2.5f, windowY / 4, "MODE?");
    }

    esat::DrawSetTextSize(24);
    esat::DrawText(windowX / 6, windowY / 1.5f, "SINGLE (S)");
    esat::DrawText(windowX / 1.5f, windowY / 1.5f, "MULTIPLAYER (M)");

    esat::DrawSetTextSize(25);
    esat::DrawText(windowX / 2.4f, windowY - 50, "BACK (o---)");
}

void DrawAdminSection() {
    esat::DrawSetFillColor(255, 255, 255, 255);

    esat::DrawSetTextSize(30);
    esat::DrawText(windowX / 3, windowY / 7, "ADMIN SECTION:");

    esat::DrawText(100, windowY / 4, "NICKNAME");
    esat::DrawText(300, windowY / 4, "USERNAME");
    esat::DrawText(500, windowY / 4, "PASSWORD");

    esat::DrawLine(windowX / 3.1f, windowY / 6, windowX / 1.5f, windowY / 6);

    DrawBack();

    esat::DrawText(20, windowY - 50, "E (EDIT)");
    esat::DrawText(windowX / 4, windowY - 50, "D (DELETE)");
    esat::DrawText(windowX / 2, windowY - 50, "N (NEXT PAGE)");
    esat::DrawText(windowX / 1.3f, windowY - 50, "L (LAST PAGE)");

    esat::DrawText(windowX - 150, windowY / 7, "PLAY (P)");

}

void DrawAskSecondLogin() {
    esat::DrawSetFillColor(255, 255, 255, 255);

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 4, windowY / 4, "LOGIN SECOND USER");

    esat::DrawSetTextSize(24);
    esat::DrawText(windowX / 3.5f, windowY / 2.5f, "USER: ");
    esat::DrawText(windowX / 3.5f, windowY / 2, "PASSWORD: ");
    esat::DrawText(windowX / 2.5f, windowY / 1.5f, "PLAY.....");

    // nickname
    esat::DrawText(windowX / 2, windowY / 2.5f, userSecondLogin);

    // tema de password
    char hiddenPass[50] = "";
    for(int i=0; i<passwordSecondLoginLength; i++) *(hiddenPass+i) = '*';
    *(hiddenPass + passwordSecondLoginLength) = '\0';

    esat::DrawSetTextSize(40);
    esat::DrawText(windowX / 2, windowY / 1.9f, hiddenPass);

    DrawBack();
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
    *(hiddenPass + passwordEditLength) = '\0';

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

    int c = ((esat::Time()/100.0f) - tempInmortality);
        
    esat::DrawSetFillColor(0, 0, 0, 0);
    esat::DrawSetStrokeColor(255, 255, 255, 255);

    for (int i = 0; i < numPoints; i++) {
        // Necesitamos esto para transformar los Mat3 en Vec3, para dibujar
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, *(shipPlayer.points + i));
        *(points + i * 2) = tmp.x;
        *(points + i * 2 + 1) = tmp.y;
    }

    // check inmortality values
    if (shipPlayer.inmortality && shipPlayer.isAlive) {
        if (c % 2 != 0) {
            esat::DrawSolidPath(points, numPoints, true);
        }

        if (timeInmortality <= 0) {
            shipPlayer.inmortality = false;
        }
    } else if (shipPlayer.isAlive) {
        esat::DrawSolidPath(points, numPoints, true);
    }
    
    
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
        // set inmortality again
        timeInmortality = 3;
        shipPlayer.inmortality = true;

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

        (*(asteroids + i)).centralPoint.x += (*(asteroids + i)).direction.x;
        (*(asteroids + i)).centralPoint.y += (*(asteroids + i)).direction.y;

        if ((*(asteroids + i)).centralPoint.x > windowX) {
            (*(asteroids + i)).centralPoint.x = 0;
        } else if ((*(asteroids + i)).centralPoint.x < 0) {
            (*(asteroids + i)).centralPoint.x = windowX;
        }

        if ((*(asteroids + i)).centralPoint.y > windowY) {
            (*(asteroids + i)).centralPoint.y = 0;
        } else if ((*(asteroids + i)).centralPoint.y < 0) {
            (*(asteroids + i)).centralPoint.y = windowY;
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

        *(points + i * 2) = ((*(asteroid->vertices + i)).x  * scale) + asteroid->centralPoint.x;
        *(points + i * 2 + 1) = ((*(asteroid->vertices + i)).y * scale) + asteroid->centralPoint.y;
    }

    esat::DrawSolidPath(points, asteroid->numVertices, true);
}

void FireShoot() {
    float bulletSpeed = 10.0f;
    float offsetInsideShip = 20.0f;

    for (int i = 0; i < shipPlayer.numberShoots; i++) {
        if (!(*(shipPlayer.shoots + i)).isVisible) {

            (*(shipPlayer.shoots + i)).isVisible = true;

            // no puedo usar shipPlayer.*(points + lo que sea), porque esta en local, no en global
            // creamos offset fictio usando el vector director
            (*(shipPlayer.shoots + i)).points->x = shipPlayer.centralPoint.x + cosf(shipPlayer.angle) * offsetInsideShip;
            (*(shipPlayer.shoots + i)).points->y = shipPlayer.centralPoint.y + sinf(shipPlayer.angle) * offsetInsideShip;
            (*(shipPlayer.shoots + i)).points->z = 1.0f;

            (*(shipPlayer.shoots + i)).vectorDirector.x = cosf(shipPlayer.angle) * bulletSpeed + shipPlayer.speed.x;
            (*(shipPlayer.shoots + i)).vectorDirector.y = sinf(shipPlayer.angle) * bulletSpeed + shipPlayer.speed.y;

            (*(shipPlayer.shoots + i)).distanceTravelled = 0.0f;
            (*(shipPlayer.shoots + i)).maxDistanceTravelled = 400.0f;

            break;
        }
    }
}

void UpdateShoots() {

    const float speed = 8.0f;

    for (int i = 0; i < shipPlayer.numberShoots; i++) {

        if ((*(shipPlayer.shoots + i)).isVisible) {

            float distanceX = (*(shipPlayer.shoots + i)).vectorDirector.x;
            float distanceY = (*(shipPlayer.shoots + i)).vectorDirector.y;

            (*(shipPlayer.shoots + i)).points->x += distanceX;
            (*(shipPlayer.shoots + i)).points->y += distanceY;

            (*(shipPlayer.shoots + i)).distanceTravelled += speed;

            if ((*(shipPlayer.shoots + i)).distanceTravelled >= (*(shipPlayer.shoots + i)).maxDistanceTravelled) {

                (*(shipPlayer.shoots + i)).isVisible = false;

                (*(shipPlayer.shoots + i)).distanceTravelled = 0.0f;

                (*(shipPlayer.shoots + i)).points->x = (*(shipPlayer.points + 0)).x;
                (*(shipPlayer.shoots + i)).points->y = (*(shipPlayer.points + 0)).y;

                // para sacar colisiones necesitamos un punto central fijo que se mueva dinamicamente
            }

            if ((*(shipPlayer.shoots + i)).points->x > windowX) {
                (*(shipPlayer.shoots + i)).points->x = 0;
            } else if ((*(shipPlayer.shoots + i)).points->x < 0) {
                (*(shipPlayer.shoots + i)).points->x = windowX;
            }

            if ((*(shipPlayer.shoots + i)).points->y > windowY) {
                (*(shipPlayer.shoots + i)).points->y = 0;
            } else if ((*(shipPlayer.shoots + i)).points->y < 0) {
                (*(shipPlayer.shoots + i)).points->y = windowY;
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

float CrossProduct(esat::Vec2 point1, esat::Vec2 point2) {
    return point1.x * point2.y - point1.y * point2.x;
}

bool CollisionDetected(esat::Vec2 point1, esat::Vec2 point2, esat::Vec2 point3, esat::Vec2 point4) {
    
    esat::Vec2 vectorDirector1 = {point2.x - point1.x, point2.y - point1.y};
    esat::Vec2 vectorDirector2 = {point4.x - point3.x, point4.y - point3.y};

    float divisor = CrossProduct(vectorDirector1, vectorDirector2);

    // checkeo que se nos lia, TO_DO check better 0, inusual to be 0 as fuck
    if (divisor == 0.0f) {
        return false;
    }

    // suponemos que ahora queremos los dos inicios de los segmentos de colisiones vaya

    esat::Vec2 distanceBetween = {point3.x - point1.x, point3.y - point1.y};

    float intersection1 = CrossProduct(distanceBetween, vectorDirector2) / divisor; 
    float intersection2 = CrossProduct(distanceBetween, vectorDirector1) / divisor;

    return (intersection1 <= 1 && intersection1 >= 0 && intersection2 <= 1 && intersection2 >= 0);
}

void DrawShoots() {
    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 255);

    for (int i = 0; i < shipPlayer.numberShoots; i++) {
        if ((*(shipPlayer.shoots + i)).isVisible) {

            // punta de la nave
            float distanceX1 = (*(shipPlayer.shoots + i)).points->x;
            float distanceY1 = (*(shipPlayer.shoots + i)).points->y;

            float distanceX2 = (*(shipPlayer.shoots + i)).vectorDirector.x;
            float distanceY2 = (*(shipPlayer.shoots + i)).vectorDirector.y;

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
        if ((*(asteroids + i)).isAlive) {
            DrawAsteroidsVer(&*(asteroids + i));
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

    (*(asteroids + totalAsteroidsPerLevels)).numVertices = count;

    (*(asteroids + totalAsteroidsPerLevels)).vertices = (esat::Vec3*)malloc(sizeof(esat::Vec3) * count);

    (*(asteroids + totalAsteroidsPerLevels)).isAlive = true;
    (*(asteroids + totalAsteroidsPerLevels)).canCollide = true;
    (*(asteroids + totalAsteroidsPerLevels)).level = asteroid.level;
    (*(asteroids + totalAsteroidsPerLevels)).type = asteroid.type;

    float speedX = rand()%2000 / 1000.0f;
    float speedY = rand()%2000 / 1000.0f;

    int mOrD = rand()%2;

    (*(asteroids + totalAsteroidsPerLevels)).direction.x = cosf(speedX * (mOrD == 1 ? 1 : -1));
    (*(asteroids + totalAsteroidsPerLevels)).direction.y = sinf(speedY * (mOrD == 1 ? 1 : -1));

    (*(asteroids + totalAsteroidsPerLevels)).centralPoint = asteroid.centralPoint;

    switch ((*(asteroids + totalAsteroidsPerLevels)).type) {
        case V1: VertsAsteroid1((*(asteroids + totalAsteroidsPerLevels)).vertices); break;
        case V2: VertsAsteroid2((*(asteroids + totalAsteroidsPerLevels)).vertices); break;
        case V3: VertsAsteroid3((*(asteroids + totalAsteroidsPerLevels)).vertices); break;
        case V4: VertsAsteroid4((*(asteroids + totalAsteroidsPerLevels)).vertices); break;
    }

    totalAsteroidsPerLevels++;
}

void BrokeAsteroid(Asteroids* asteroid_broke) {

    if (asteroid_broke->level == AsteroidsLevel::LEVEL_1) {
        if (shipPlayer.isSecondPlayer) {
            puntuationInGame2 += 100;
        } else {
            puntuationInGame1 += 100;
        }
        asteroid_broke->isAlive = false;
        asteroid_broke->canCollide = false;
        return;
    }

    switch (asteroid_broke->level) {
        case AsteroidsLevel::LEVEL_3:
            if (shipPlayer.isSecondPlayer) {
                puntuationInGame2 += 20;
            } else {
                puntuationInGame1 += 20;
            }
            asteroid_broke->level = LEVEL_2;
        break;
        case AsteroidsLevel::LEVEL_2:
            if (shipPlayer.isSecondPlayer) {
                puntuationInGame2 += 50;
            } else {
                puntuationInGame1 += 50;
            }
            asteroid_broke->level = LEVEL_1;
        break;
    }
    
    ActivateNewAsteroid(*asteroid_broke);
}

void SpawnPlayer() {
    shipPlayer.centralPoint = {windowX / 2, windowY / 2};
}

void RestLifes() {
    if (!shipPlayer.inmortality) {
        if (shipPlayer.isSecondPlayer) {
            secondPlayerLifes--;
        } else {
            shipPlayer.lifes--;
        }
        
        if (isMultiplayerActive) {
            shipPlayer.isSecondPlayer = !shipPlayer.isSecondPlayer;
        }

        shipPlayer.inmortality = true;
        timeInmortality = 3.0f;
    }
}

void CheckInmortality(float deltaTime) {
    if (shipPlayer.inmortality) {

        // Logic
        printf("DELTA TIME: [%f] \n", deltaTime);
        timeInmortality += deltaTime;
        printf("Inmortality timer: [%f] \n", timeInmortality);

        if (timeInmortality <= 0) {
            shipPlayer.inmortality = false;
        }
    }
}

void CheckAnimatedDead(float deltaTime) {
    if (shipPlayer.showDeadZone) {

        timeDeadShip += deltaTime;

        if (timeDeadShip <= 0) {

            shipPlayer.isAlive = true;
            SpawnPlayer();

            shipPlayer.inmortality = true;
            timeInmortality = 2.0f;

            shipPlayer.showDeadZone = false;
            timeDeadShip = 0.0f;
        }
    }
}

void DrawPuntuation() {
    char score[6];
	// Puntuacion
    esat::DrawSetTextSize(20);
	esat::DrawSetFillColor(255, 255, 255);
	itoa(puntuationInGame1 + 1000000, score, 10);

    esat::DrawText(40, 50, score + 1);

    if (isMultiplayerActive) {
        char score[6];
        // Puntuacion
        esat::DrawSetTextSize(20);
        esat::DrawSetFillColor(255, 255, 255);
        itoa(puntuationInGame2 + 1000000, score, 10);

        esat::DrawText(windowX - 120, 50, score + 1);
    }
}

void DrawLifes(esat::Mat3 m) {
    float points[numPoints * 2];

    esat::DrawSetFillColor(0, 0, 0, 0);
    esat::DrawSetStrokeColor(255, 255, 255, 255);

    for (int i = 0; i < numPoints; i++) {
        // Necesitamos esto para transformar los Mat3 en Vec3, para dibujar
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, (*(shipPlayer.points + i)));
        *(points + i*2) = tmp.x;
        *(points + i*2+1) = tmp.y;
    }

    esat::DrawSolidPath(points, numPoints, true);
}

void SpaceJump() {
    timeInmortality = 3;
    shipPlayer.inmortality = true;

    shipPlayer.centralPoint.x = (float) (rand()%800);
    shipPlayer.centralPoint.y = (float) (rand()%608);
}

void SavePuntuation(char* nick, char* user, int newPuntuation) {
    FILE *file = fopen("puntuations.dat", "rb");

    unsigned char buffer[11 * 21];
    int count = 0;

    if (file != NULL) {
        while (count < 10) {
            if (fread(buffer + count * 21, 21, 1, file) != 1) break;
            count++;
        }
        fclose(file);
    }

    unsigned char* newEntry = buffer + count * 21;

    memcpy(newEntry + 0, &newPuntuation, 4);
    memcpy(newEntry + 4, nick, 3);
    memcpy(newEntry + 7, user, 14);

    count++;

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {

            int scoreA, scoreB;

            memcpy(&scoreA, buffer + i * 21, 4);
            memcpy(&scoreB, buffer + j * 21, 4);

            if (scoreB > scoreA) {
                unsigned char tmp[21];
                memcpy(tmp, buffer + i * 21, 21);
                memcpy(buffer + i * 21, buffer + j * 21, 21);
                memcpy(buffer + j * 21, tmp, 21);
            }
        }
    }

    playerHighscored = false;

    int lowestScore = 0;

    if (count >= 10) {
        memcpy(&lowestScore, buffer + 9 * 21, 4);
    }

    // checkeo de si esta dentro de los minimos y si ademas es mayor que el ultimo
    if (count < 10 || newPuntuation >= lowestScore) {
        playerHighscored = true;
    }

    if (count > 10) count = 10;

    file = fopen("puntuations.dat", "wb");

    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fwrite(buffer + i * 21, 21, 1, file);
    }

    fclose(file);
}

void RestCredits(char* username) {
    file = fopen("users.dat", "r+b");

    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    int id;
    int credits;
    char *user = (char*)malloc(14 * sizeof(char));

    while (fread(&id, sizeof(int), 1, file) == 1) {

        fseek(file, 3, SEEK_CUR);

        fread(user, 14, 1, file);

        if (strcmp(user, username) == 0) {

            fseek(file, 14 + 10 + 14 + 14 + sizeof(bool), SEEK_CUR);

            fread(&credits, sizeof(int), 1, file);

            //printf("Credits before: [%d] \n", credits);

            credits--;

            if (playerHighscored) {
                credits += 5;
            }

            //printf("Credits after: [%d] \n", credits);


            fseek(file, -sizeof(int), SEEK_CUR);

            fwrite(&credits, sizeof(int), 1, file);

            break;
        }

        fseek(file,
            14 + 10 + 14 + 14 +
            sizeof(bool) + sizeof(int) +
            sizeof(bool) + sizeof(int),
            SEEK_CUR
        );
    }

    fclose(file);
}

void CheckLifes() {
    if (puntuationInGame1 % 10000 == 0 && puntuationInGame1 != 0) {
        shipPlayer.lifes++;
    }

    if (puntuationInGame2 % 10000 == 0 && puntuationInGame2 != 0) {
        secondPlayerLifes++;
    }

    if (isMultiplayerActive) {

        if (shipPlayer.lifes == 0 && secondPlayerLifes == 0) {
            SavePuntuation(currentNick1, userLogin, puntuationInGame1);
            RestCredits(userLogin);
            SavePuntuation(currentNick2, userSecondLogin, puntuationInGame2);
            RestCredits(userSecondLogin);

            // Load users too, to ordered users b 
            LoadUsersOrdered();

            // reinicio de la nave para evitar problemas de compatiblidad
            shipPlayer.inmortality = true;
            shipPlayer.isAlive = true;
            shipPlayer.showDeadZone = false;
            timeInmortality = 3.0f;
            shipPlayer.centralPoint = {windowX / 2, windowY / 2, 1.0f};
            puntuationInGame1 = 0;
            puntuationInGame2 = 0;
            ufo.centralPoint = {-50, 100};

            // reinicio de level en cada repeticion de game
            LevelConfig(actualLevel);
            InitAsteroids();

            // cambiamos a escena de highscores
            currentGame.actualScene = HIGHSCORES;
            shipPlayer.lifes = 4;
            secondPlayerLifes = 4;
            isMultiplayerActive = false;
        }
    } else {
        if (shipPlayer.lifes == 0) {
            SavePuntuation(currentNick1, userLogin, puntuationInGame1);
            RestCredits(userLogin);

            // Load users too, to ordered users b 
            LoadUsersOrdered();

            // reinicio de la nave para evitar problemas de compatiblidad
            shipPlayer.inmortality = true;
            shipPlayer.isAlive = true;
            shipPlayer.showDeadZone = false;
            timeInmortality = 3.0f;
            shipPlayer.centralPoint = {windowX / 2, windowY / 2, 1.0f};
            puntuationInGame1 = 0;
            puntuationInGame2 = 0;
            ufo.centralPoint = {-50, 100};

            // reinicio de level en cada repeticion de game
            LevelConfig(actualLevel);
            InitAsteroids();

            // cambiamos a escena de highscores
            currentGame.actualScene = HIGHSCORES;
            shipPlayer.lifes = 4;
            secondPlayerLifes = 4;
            isMultiplayerActive = false;
        }
    }
    
}

void InitFragments(Ship shipCopy) {
    for (int i = 0; i < 5; i++) {
        // siguiente vertice al que apunta
        int next = (i + 1) % 5;

        (*(fragments + i)).point1 = {
            (*(shipCopy.points + i)).x + shipCopy.centralPoint.x,
            (*(shipCopy.points + i)).y + shipCopy.centralPoint.y
        };

        (*(fragments + i)).point2 = {
            (*(shipCopy.points + next)).x + shipCopy.centralPoint.x,
            (*(shipCopy.points + next)).y + shipCopy.centralPoint.y
        };

        // usamos radianes para el angulo esta vez
        (*(fragments + i)).angle = (rand() % 360) * 3.1416f / 180.0f;
    }
}

void DrawDeadShip(float deltaTime) {
    if (timeDeadShip > 0 && shipPlayer.showDeadZone) {
        timeDeadShip += deltaTime;

        esat::DrawSetFillColor(0, 0, 0, 0);
        esat::DrawSetStrokeColor(255, 255, 255, 255);

        float speed = 30.0f;

         for (int i = 0; i < 5; i++) {

            (*(fragments + i)).point1.x += cosf((*(fragments + i)).angle) * deltaTime * speed;
            (*(fragments + i)).point1.y += sinf((*(fragments + i)).angle) * deltaTime * speed;

            (*(fragments + i)).point2.x += cosf((*(fragments + i)).angle) * deltaTime * speed;
            (*(fragments + i)).point2.y += sinf((*(fragments + i)).angle) * deltaTime * speed;

            float points[4] = {
                (*(fragments + i)).point1.x, (*(fragments + i)).point1.y,
                (*(fragments + i)).point2.x, (*(fragments + i)).point2.y
            };

            esat::DrawSolidPath(points, 2);
        }

    }
}

void SpawnAsteroidParticles(esat::Vec2 center) {

    for (int i = 0; i < 12; i++) {

        (*(fragmentsAsteroids + i)).point1 = center;

        float angle = (rand() % 360) * 3.1416f / 180.0f;

        (*(fragmentsAsteroids + i)).angle = angle;
        (*(fragmentsAsteroids + i)).speed = 50.0f;
        (*(fragmentsAsteroids + i)).life = 1.5f;

        (*(fragmentsAsteroids + i)).isAlive = true;
    }
}

void UpdateParticles(float deltaTime) {

    for (int i = 0; i < 12; i++) {

        if (!(*(fragmentsAsteroids + i)).isAlive) continue;

        (*(fragmentsAsteroids + i)).point1.x += cosf((*(fragmentsAsteroids + i)).angle) * (*(fragmentsAsteroids + i)).speed * deltaTime;
        (*(fragmentsAsteroids + i)).point1.y += sinf((*(fragmentsAsteroids + i)).angle) * (*(fragmentsAsteroids + i)).speed * deltaTime;

        (*(fragmentsAsteroids + i)).life += deltaTime;
        // printf("[%f] \n", *(fragmentsAsteroids + i).life);

        if ((*(fragmentsAsteroids + i)).life <= 0) {
            (*(fragmentsAsteroids + i)).isAlive = false;
        }
    }
}

void DrawParticles() {

    esat::Vec2 pointsFlex[20];

    for (int i = 0; i < 12; i++) {

        if (!(*(fragmentsAsteroids + i)).isAlive) continue;

        esat::Mat3 m = esat::Mat3Identity();

        m = esat::Mat3Multiply(esat::Mat3Scale(2.0f, 2.0f), m);

        esat::Vec2 scale = {2.0f, 2.0f};

        m = esat::Mat3Multiply(
            esat::Mat3Translate(
                (*(fragmentsAsteroids + i)).point1.x,
                (*(fragmentsAsteroids + i)).point1.y
            ),
            m
        );

        for (int j = 0; j < 20; j++) {

            esat::Vec3 tmp = esat::Mat3TransformVec3(m, *(g_circle + j));

            *(pointsFlex + j) = {tmp.x, tmp.y};
        }

        esat::DrawSetFillColor(255, 0, 0);
        esat::DrawSolidPath(&(*(pointsFlex + 0)).x, 20);
    }
}

void MoveUFO(UFO* ufo, float deltaTime) {
    if (ufo->isAlive) {
        if (!ufo->isLittle) {
            ufo->centralPoint.x += 1.0f;
        } else {
            // logica del pequeñin
            ufo->centralPoint.x += 1.5f;
        }

        if (ufo->centralPoint.x - 40 > windowX) {
            float yDistance = rand()%600;
            ufo->centralPoint = {-50.0f, yDistance, 0.0f};
            ufoSpawnTimer = 0;
        }
    }
}

void FireUFOShoot(UFO* ufo) {
    // codigo que rompe el ecosistema directamente
    if (!ufo->isAlive || ufo->shoot.isVisible) return;

    ufo->shoot.isVisible = true;

    // posición inicial de la bala
    ufo->shoot.points->x = ufo->centralPoint.x;
    ufo->shoot.points->y = ufo->centralPoint.y;
    ufo->shoot.points->z = 1.0f;

    // dirección hacia el jugador, vector director calculado en 
    esat::Vec2 vecDir;
    vecDir.x = shipPlayer.centralPoint.x - ufo->centralPoint.x;
    vecDir.y = shipPlayer.centralPoint.y - ufo->centralPoint.y;

    float length = sqrtf(vecDir.x * vecDir.x + vecDir.y * vecDir.y);

    vecDir.x /= length;
    vecDir.y /= length;

    float bulletSpeed = 6.0f;

    ufo->shoot.vectorDirector.x = vecDir.x * bulletSpeed;
    ufo->shoot.vectorDirector.y = vecDir.y * bulletSpeed;

    ufo->shoot.distanceTravelled = 0.0f;
    ufo->shoot.maxDistanceTravelled = 400.0f;
}

void CheckUFOSpawn(float deltaTime) {

    ufoSpawnTimer += deltaTime;

    // printf("Timer:  [%f] \n", ufoSpawnTimer);
    if (ufoSpawnTimer < -5.0f) {
        ufo.isAlive = true;
        MoveUFO(&ufo, deltaTime);
    }

    ufoShootTimer += deltaTime;

    if (ufoShootTimer < -2.0f) {
        FireUFOShoot(&ufo);
        ufoShootTimer = 0.0f;
    }
}

void DrawUfo(UFO* ufo) {
    // static con un maximo
    if (ufo->isAlive) {
        static float points[24 * 2];
        esat::DrawSetFillColor(0, 0, 0, 0);
        esat::DrawSetStrokeColor(255, 255, 255, 255);

        for (int i = 0; i < ufo->numVertices; i++) {
            
            float scale = 8.0f;

            if (ufo->isLittle) {
                scale = 2.0f;
            }

            *(points + i * 2) = ((ufo->vertices + i)->x  * scale) + ufo->centralPoint.x;
            *(points + i * 2 + 1) = ((ufo->vertices + i)->y * scale) + ufo->centralPoint.y;
        }

        esat::DrawSolidPath(points, ufo->numVertices, true);
    } 
}

void UpdateUFOShoot(UFO* ufo) {
    if (!ufo->shoot.isVisible) return;

    ufo->shoot.points->x += ufo->shoot.vectorDirector.x;
    ufo->shoot.points->y += ufo->shoot.vectorDirector.y;

    ufo->shoot.distanceTravelled += 1.0f;

    if (ufo->shoot.distanceTravelled >= ufo->shoot.maxDistanceTravelled) {

        ufo->shoot.isVisible = false;
        ufo->shoot.distanceTravelled = 0.0f;
    }
}

void DrawUFOShoot(UFO* ufo) {
    if (!ufo->shoot.isVisible) return;

    esat::DrawSetFillColor(255, 255, 255, 255);
    esat::DrawSetStrokeColor(255, 255, 255, 255);

    float line[4] = {
        ufo->shoot.points->x,
        ufo->shoot.points->y,
        ufo->shoot.points->x - ufo->shoot.vectorDirector.x,
        ufo->shoot.points->y - ufo->shoot.vectorDirector.y
    };

    esat::DrawSolidPath(line, 2, false);
}

void ClearMemory() {
    memset(nickname, 0, 1);

    memset(userPlayer, 0, 1);
    memset(password, 0, 1);

    memset(userLogin, 0, 1);
    memset(passwordLogin, 0, 1);

    memset(userSecondLogin, 0, 1);
    memset(passwordSecondLogin, 0, 1);

    memset(birthday, 0, 1);
    memset(province, 0, 1);
    memset(email, 0, 1);

    memset(nicknameEdit, 0, 1);

    memset(userPlayerEdit, 0, 1);
    memset(passwordEdit, 0, 1);

    memset(birthdayEdit, 0, 1);
    memset(provinceEdit, 0, 1);
    memset(emailEdit, 0, 1);

    memset(currentNick1, 0, 4);
    memset(currentNick2, 0, 4);
}

int esat::main(int argc, char **argv) {

    esat::WindowInit(windowX, windowY);
    esat::WindowSetMouseVisibility(true);

    GenerateSemilla();
    InitConfig();
    InitShip();

    esat::Mat3 matriz = UpdateFigurita({1.0f, 1.0f}, 0.0f, {0.0f, 0.0f});
    esat::Mat3 matrizLittle = UpdateFigurita({1.0f, 1.0f}, 0.0f, {0.0f, 0.0f});
    Ship shipCopy;


    ClearMemory();


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

                // Debug
                if (esat::IsSpecialKeyDown(esat::kSpecialKey_F1)) {
                    // test inmortality
                    timeInmortality = 3;
                    shipPlayer.inmortality = true;
                    currentGame.actualScene = GAMEPLAY;
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
            case Scenes::ASK_GAMEPLAY:

                DrawAskGameplay();
                HandleAskGameplay();

            break;
            case Scenes::ASK_SECOND_LOGIN:
                DrawAskSecondLogin();
                HandleSecondLogin();

            break;
            case Scenes::GAMEPLAY:
 
            printf("Nick 1: [%s] \n", currentNick1);
            printf("Nick 2: [%s] \n", currentNick2);  
                // control de paso de nivel;
                pendingLevelChange = true;
 
                for (int l = 0; l < totalAsteroidsPerLevels; l++) {
                    if ((*(asteroids + l)).isAlive) {
                        pendingLevelChange = false;
                    }
                }

                UpdateAsteroids();
                UpdateGame();

                if (esat::IsSpecialKeyDown(esat::kSpecialKey_Space)) {
                    if (!shipPlayer.inmortality) {
                        FireShoot();
                    }
                }

                UpdateShoots();

                // fumadinha de colisiones, primero por nave con asteroids

                bool collision = false;

                if (!shipPlayer.inmortality) {
                    for (int i = 0; i < 5 && !collision; i++) {

                        int nextI = (i + 1) % 5;
                        
                        for (int j = 0; j < totalAsteroidsPerLevels; j++) {
                            if ((*(asteroids + j)).canCollide) {
                                for (int k = 0; k < (*(asteroids + j)).numVertices; k++) {

                                    int nextK = (k + 1) % (*(asteroids + j)).numVertices;

                                    //printf("Puntos nave: [%f]   ----    [%f] \n", shipPlayer.*(points + i).x + shipPlayer.centralPoint.x, shipPlayer.*(points + i).y + shipPlayer.centralPoint.y);
                                    //printf("Puntos asteroids: [%f]   ----    [%f] \n", *(asteroids + j).*(vertices + k).x + + *(asteroids + j).centralPoint.x, *(asteroids + j).*(vertices + k).y + + *(asteroids + j).centralPoint.y);

                                    esat::Vec2 centralPointShip = {shipPlayer.centralPoint.x, shipPlayer.centralPoint.y};
                                    esat::Vec2 centralPointAsteroid = {(*(asteroids + j)).centralPoint.x, (*(asteroids + j)).centralPoint.y};

                                    float scaleAsteroid = 25.0f;

                                    switch ((*(asteroids + j)).level) {
                                        case AsteroidsLevel::LEVEL_1:
                                            scaleAsteroid *= 1;
                                        break;
                                        case AsteroidsLevel::LEVEL_2:
                                            scaleAsteroid *= 2;
                                        break;
                                        case AsteroidsLevel::LEVEL_3:
                                            scaleAsteroid *= 3;
                                        break;
                                    };

                                    esat::Vec2 point1 = {(*(shipPlayer.points + i)).x + centralPointShip.x, (*(shipPlayer.points + i)).y + centralPointShip.y};
                                    esat::Vec2 point2 = {(*(shipPlayer.points + nextI)).x + centralPointShip.x, (*(shipPlayer.points + nextI)).y + centralPointShip.y};
                                    esat::Vec2 point3 = {(((*(asteroids + j)).vertices + k)->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + k)->y * scaleAsteroid) + centralPointAsteroid.y};
                                    esat::Vec2 point4 = {(((*(asteroids + j)).vertices + nextK)->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + nextK)->y * scaleAsteroid) + centralPointAsteroid.y};

                                    if (CollisionDetected(point1, point2, point3, point4)) {
                                        collision = true;
                                        (*(asteroids + j)).deadZone = (*(asteroids + j)).centralPoint;
                                        SpawnAsteroidParticles({(*(asteroids + j)).deadZone.x, (*(asteroids + j)).deadZone.y});
                                        BrokeAsteroid(&*(asteroids + j));
                                        break;
                                    }
                                }
                            }
                            if (collision) break;
                        }

                        if (collision) break;
                    }

                }

                // Colision de las balas del player con asteroids
                bool bulletCollision = false;
                for (int i = 0; i < shipPlayer.numberShoots && !bulletCollision; i++) {
                    if ((*(shipPlayer.shoots + i)).isVisible) {
                        for (int j = 0; j < totalAsteroidsPerLevels; j++) {
                            if ((*(asteroids + j)).canCollide) {
                                for (int k = 0; k < (*(asteroids + j)).numVertices; k++) {

                                    int nextK = (k + 1) % (*(asteroids + j)).numVertices;

                                    //printf("Puntos nave: [%f]   ----    [%f] \n", shipPlayer.*(points + i).x + shipPlayer.centralPoint.x, shipPlayer.*(points + i).y + shipPlayer.centralPoint.y);
                                    //printf("Puntos asteroids: [%f]   ----    [%f] \n", *(asteroids + j).*(vertices + k).x + + *(asteroids + j).centralPoint.x, *(asteroids + j).*(vertices + k).y + + *(asteroids + j).centralPoint.y);

                                    esat::Vec2 centralPointBullet = {(*(shipPlayer.shoots + i)).points->x, (*(shipPlayer.shoots + i)).points->y};
                                    esat::Vec2 centralPointAsteroid = {(*(asteroids + j)).centralPoint.x, (*(asteroids + j)).centralPoint.y};

                                    float scaleAsteroid = 25.0f;

                                    switch ((*(asteroids + j)).level) {
                                        case AsteroidsLevel::LEVEL_1:
                                            scaleAsteroid *= 1;
                                        break;
                                        case AsteroidsLevel::LEVEL_2:
                                            scaleAsteroid *= 2;
                                        break;
                                        case AsteroidsLevel::LEVEL_3:
                                            scaleAsteroid *= 3;
                                        break;
                                    };

                                    esat::Vec2 point1 = {(*(shipPlayer.shoots + i)).points->x, (*(shipPlayer.shoots + i)).points->y};
                                    esat::Vec2 point2 = {(*(shipPlayer.shoots + i)).points->x + (*(shipPlayer.shoots + i)).vectorDirector.x, (*(shipPlayer.shoots + i)).points->y + (*(shipPlayer.shoots + i)).vectorDirector.y};
                                    esat::Vec2 point3 = {(((*(asteroids + j)).vertices + k)->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + k)->y * scaleAsteroid) + centralPointAsteroid.y};
                                    esat::Vec2 point4 = {(((*(asteroids + j)).vertices + nextK)->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + nextK)->y * scaleAsteroid) + centralPointAsteroid.y};


                                    if (CollisionDetected(point1, point2, point3, point4)) {
                                        bulletCollision = true;
                                        (*(shipPlayer.shoots + i)).isVisible = false;
                                        (*(asteroids + j)).deadZone = (*(asteroids + j)).centralPoint;
                                        SpawnAsteroidParticles({(*(asteroids + j)).deadZone.x, (*(asteroids + j)).deadZone.y});
                                        BrokeAsteroid(&*(asteroids + j));
                                        break;
                                    }
                                }
                            }
                            if (bulletCollision) break;
                        }
                        if (bulletCollision) break;
                    }
                }
                
                // TO_DO separate logic into inmortality and show dead ship
                if (collision && !shipPlayer.inmortality) {
                    shipPlayer.deadZone = shipPlayer.centralPoint;
                    shipPlayer.showDeadZone = true;
                    shipCopy = shipPlayer;
                    shipPlayer.isAlive = false;
                    InitFragments(shipCopy);
                    RestLifes();
                    shipPlayer.acceleration = {0.0f, 0.0f};
                    shipPlayer.speed = {0.0f, 0.0f};
                    timeDeadShip = 2;
                }

                bool ufoCollision = false;
                for (int i = 0; i < 12 && !ufoCollision; i++) {

                    int nextI = (i + 1) % 12;
                    
                    for (int j = 0; j < totalAsteroidsPerLevels; j++) {
                        if ((*(asteroids + j)).canCollide) {
                            for (int k = 0; k < (*(asteroids + j)).numVertices; k++) {

                                int nextK = (k + 1) % (*(asteroids + j)).numVertices;

                                //printf("Puntos nave: [%f]   ----    [%f] \n", shipPlayer.*(points + i).x + shipPlayer.centralPoint.x, shipPlayer.*(points + i).y + shipPlayer.centralPoint.y);
                                //printf("Puntos asteroids: [%f]   ----    [%f] \n", *(asteroids + j).*(vertices + k).x + + *(asteroids + j).centralPoint.x, *(asteroids + j).*(vertices + k).y + + *(asteroids + j).centralPoint.y);

                                esat::Vec2 centralPointUFO = {ufo.centralPoint.x, ufo.centralPoint.y};
                                esat::Vec2 centralPointAsteroid = {(*(asteroids + j)).centralPoint.x, (*(asteroids + j)).centralPoint.y};

                                float scaleAsteroid = 25.0f;

                                switch ((*(asteroids + j)).level) {
                                    case AsteroidsLevel::LEVEL_1:
                                        scaleAsteroid *= 1;
                                    break;
                                    case AsteroidsLevel::LEVEL_2:
                                        scaleAsteroid *= 2;
                                    break;
                                    case AsteroidsLevel::LEVEL_3:
                                        scaleAsteroid *= 3;
                                    break;
                                };

                                float scaleUFO = 8.0f;

                                if (ufo.isLittle) {
                                    scaleUFO = 2.0f;
                                }

                                esat::Vec2 point1 = {((*(ufo.vertices + i)).x * scaleUFO) + centralPointUFO.x, ((*(ufo.vertices + i)).y * scaleUFO) + centralPointUFO.y};
                                esat::Vec2 point2 = {((*(ufo.vertices + nextI)).x * scaleUFO) + centralPointUFO.x, ((*(ufo.vertices + nextI)).y * scaleUFO) + centralPointUFO.y};
                                esat::Vec2 point3 = {((((*(asteroids + j)).vertices + k))->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + k)->y * scaleAsteroid) + centralPointAsteroid.y};
                                esat::Vec2 point4 = {((((*(asteroids + j)).vertices + nextK))->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + nextK)->y * scaleAsteroid) + centralPointAsteroid.y};

                                if (CollisionDetected(point1, point2, point3, point4)) {
                                    ufoCollision = true;
                                    (*(asteroids + j)).deadZone = (*(asteroids + j)).centralPoint;
                                    SpawnAsteroidParticles({(*(asteroids + j)).deadZone.x, (*(asteroids + j)).deadZone.y});
                                    ufoSpawnTimer = 0;
                                    float yDistance = rand()%600;
                                    ufo.centralPoint = {-50.0f, yDistance, 0.0f};
                                    ufoShootTimer = 0;
                                    ufo.shoot.isVisible = false;
                                    ufo.isAlive = false;
                                    ufo.canCollide = false;
                                    ufo.isLittle = !ufo.isLittle;

                                    BrokeAsteroid(&*(asteroids + j));
                                    break;
                                }
                            }
                        }
                        if (ufoCollision) break;
                    }

                    if (ufoCollision) break;
                }

                bool bulletCollisionWithUFO = false;
                for (int i = 0; i < shipPlayer.numberShoots && !bulletCollisionWithUFO; i++) {
                    if ((*(shipPlayer.shoots + i)).isVisible) {
                        for (int k = 0; k < 12; k++) {

                            int nextK = (k + 1) % 12;

                            esat::Vec2 centralPointBullet = {(*(shipPlayer.shoots + i)).points->x, (*(shipPlayer.shoots + i)).points->y};
                            esat::Vec2 centralPointUFO = {ufo.centralPoint.x, ufo.centralPoint.y};

                            float scaleUFO = 8.0f;

                            if (ufo.isLittle) {
                                scaleUFO = 2.0f;
                            }

                            esat::Vec2 point1 = {((*(ufo.vertices + k)).x * scaleUFO) + centralPointUFO.x, ((*(ufo.vertices + k)).y * scaleUFO) + centralPointUFO.y};
                            esat::Vec2 point2 = {((*(ufo.vertices + nextK)).x * scaleUFO) + centralPointUFO.x, ((*(ufo.vertices + nextK)).y * scaleUFO) + centralPointUFO.y};
                            esat::Vec2 point3 = centralPointBullet;
                            esat::Vec2 point4 = {centralPointBullet.x - ((*(shipPlayer.shoots + i)).vectorDirector.x * 2), centralPointBullet.y - ((*(shipPlayer.shoots + i)).vectorDirector.y * 2)};


                            if (CollisionDetected(point1, point2, point3, point4)) {
                                if (!shipPlayer.isSecondPlayer) {
                                    puntuationInGame1 += 200;
                                    if (ufo.isLittle) {
                                        puntuationInGame1 += 100;
                                    }    
                                } else {
                                    puntuationInGame2 += 200;
                                    if (ufo.isLittle) {
                                        puntuationInGame2 += 100;
                                    }
                                }
                                
                                (*(shipPlayer.shoots + i)).isVisible = false;
                                ufoSpawnTimer = 0;
                                float yDistance = rand()%600;
                                ufo.centralPoint = {-50.0f, yDistance, 0.0f};
                                ufoShootTimer = 0;
                                ufo.shoot.isVisible = false;
                                ufo.isAlive = false;
                                ufo.canCollide = false;
                                ufo.isLittle = !ufo.isLittle;
                                bulletCollisionWithUFO = true;
                                break;
                            }
                        }

                        if (bulletCollisionWithUFO) break;
                    }
                }

                printf("Credits Player 1: [%d] \n", creditsPlayer1);
        printf("Credits Player 2: [%d] \n", creditsPlayer2);

                bool ufoCollisionBulletAsteroid = false;
                for (int i = 0; i < 12 && !ufoCollisionBulletAsteroid; i++) {

                    int nextI = (i + 1) % 12;
                    
                    for (int j = 0; j < totalAsteroidsPerLevels; j++) {
                        if ((*(asteroids + j)).canCollide) {
                            for (int k = 0; k < (*(asteroids + j)).numVertices; k++) {

                                int nextK = (k + 1) % (*(asteroids + j)).numVertices;

                                //printf("Puntos nave: [%f]   ----    [%f] \n", shipPlayer.*(points + i).x + shipPlayer.centralPoint.x, shipPlayer.*(points + i).y + shipPlayer.centralPoint.y);
                                //printf("Puntos asteroids: [%f]   ----    [%f] \n", *(asteroids + j).*(vertices + k).x + + *(asteroids + j).centralPoint.x, *(asteroids + j).*(vertices + k).y + + *(asteroids + j).centralPoint.y);

                                esat::Vec2 centralPointBullet = {ufo.shoot.points->x, ufo.shoot.points->y};
                                esat::Vec2 centralPointAsteroid = {(*(asteroids + j)).centralPoint.x, (*(asteroids + j)).centralPoint.y};

                                float scaleAsteroid = 25.0f;

                                switch ((*(asteroids + j)).level) {
                                    case AsteroidsLevel::LEVEL_1:
                                        scaleAsteroid *= 1;
                                    break;
                                    case AsteroidsLevel::LEVEL_2:
                                        scaleAsteroid *= 2;
                                    break;
                                    case AsteroidsLevel::LEVEL_3:
                                        scaleAsteroid *= 3;
                                    break;
                                };

                                float scaleUFO = 8.0f;

                                if (ufo.isLittle) {
                                    scaleUFO = 2.0f;
                                }

                                esat::Vec2 point1 = {centralPointBullet.x, centralPointBullet.y};
                                esat::Vec2 point2 = {centralPointBullet.x - (ufo.shoot.vectorDirector.x * 2), centralPointBullet.y - (ufo.shoot.vectorDirector.y * 2)};
                                esat::Vec2 point3 = {(((*(asteroids + j)).vertices + k)->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + k)->y * scaleAsteroid) + centralPointAsteroid.y};
                                esat::Vec2 point4 = {(((*(asteroids + j)).vertices + nextK)->x * scaleAsteroid) + centralPointAsteroid.x, (((*(asteroids + j)).vertices + nextK)->y * scaleAsteroid) + centralPointAsteroid.y};

                                if (CollisionDetected(point1, point2, point3, point4)) {
                                    ufoCollisionBulletAsteroid = true;
                                    (*(asteroids + j)).deadZone = (*(asteroids + j)).centralPoint;
                                    SpawnAsteroidParticles({(*(asteroids + j)).deadZone.x, (*(asteroids + j)).deadZone.y});
                                    ufoShootTimer = 0;
                                    ufo.shoot.isVisible = false;

                                    BrokeAsteroid(&*(asteroids + j));
                                    break;
                                }
                            }
                        }
                        if (ufoCollisionBulletAsteroid) break;
                    }

                    if (ufoCollisionBulletAsteroid) break;
                }

                bool ufoCollisionBulletShip = false;

                for (int i = 0; i < 5 && !ufoCollisionBulletShip; i++) {

                    int nextI = (i + 1) % 5;

                    //printf("Puntos nave: [%f]   ----    [%f] \n", shipPlayer.*(points + i).x + shipPlayer.centralPoint.x, shipPlayer.*(points + i).y + shipPlayer.centralPoint.y);
                    //printf("Puntos asteroids: [%f]   ----    [%f] \n", *(asteroids + j).*(vertices + k).x + + *(asteroids + j).centralPoint.x, *(asteroids + j).*(vertices + k).y + + *(asteroids + j).centralPoint.y);

                    esat::Vec2 centralPointBullet = {ufo.shoot.points->x, ufo.shoot.points->y};
                    esat::Vec2 centralPointShip = {shipPlayer.centralPoint.x, shipPlayer.centralPoint.y};

                    esat::Vec2 point1 = {centralPointBullet.x, centralPointBullet.y};
                    esat::Vec2 point2 = {centralPointBullet.x - (ufo.shoot.vectorDirector.x * 2), centralPointBullet.y - (ufo.shoot.vectorDirector.y * 2)};
                    esat::Vec2 point3 = {(*(shipPlayer.points + i)).x + centralPointShip.x, (*(shipPlayer.points + i)).y + centralPointShip.y};
                    esat::Vec2 point4 = {(*(shipPlayer.points + nextI)).x + centralPointShip.x, (*(shipPlayer.points + nextI)).y + centralPointShip.y};

                    //esat::DrawSetStrokeColor(255,0,0);

                    /*esat::DrawLine(
                        point1.x, point1.y,
                        point2.x, point2.y
                    );*/
                                    
                    if (CollisionDetected(point1, point2, point3, point4) && !shipPlayer.inmortality) {
                        ufo.shoot.isVisible = false;
                        ufoCollisionBulletShip = true;
                        shipPlayer.deadZone = shipPlayer.centralPoint;
                        shipPlayer.showDeadZone = true;
                        shipCopy = shipPlayer;
                        shipPlayer.isAlive = false;
                        InitFragments(shipCopy);
                        RestLifes();
                        shipPlayer.acceleration = {0.0f, 0.0f};
                        shipPlayer.speed = {0.0f, 0.0f};
                        timeDeadShip = 2;
                        break;
                    }

                    if (ufoCollisionBulletShip) break;
                }

                DrawDeadShip((current_time - last_time) / 1000 * (fps * 0.6f));

                UpdateParticles((current_time - last_time) / 1000 * (fps * 0.6f));
                DrawParticles();

                CheckInmortality((current_time - last_time) / 1000 * (fps * 0.6f));
                CheckAnimatedDead((current_time - last_time) / 1000 * (fps * 0.6f));

                CheckUFOSpawn((current_time - last_time) / 1000 * (fps * 0.6f));
                DrawUfo(&ufo);

                UpdateUFOShoot(&ufo);
                DrawUFOShoot(&ufo);

                // all this shit is going into handle hell function
                // think about + and - acceleration
                if (esat::IsKeyPressed('D')) {
                    shipPlayer.angle += 0.1f;
                }

                if (esat::IsKeyPressed('A')) {
                    shipPlayer.angle -= 0.1f;
                }

                if (esat::IsKeyPressed('W') && shipPlayer.isAlive) {
                    shipPlayer.acceleration = {
                        cosf(shipPlayer.angle) * 0.1f,
                        sinf(shipPlayer.angle) * 0.1f
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

                }

                // desacelera siempre
                shipPlayer.speed.x *= deceleration;
                shipPlayer.speed.y *= deceleration;

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

                if (esat::IsKeyDown('G')) {
                    // inmortal otra vez
                    timeInmortality = 3;
                    SpaceJump();
                }

                matriz = UpdateFigurita({1.0f, 1.0f}, shipPlayer.angle, {shipPlayer.centralPoint.x, shipPlayer.centralPoint.y});

                DrawFigurita(matriz);
                DrawPuntuation();

                // handle lifes and all logic around theme
                CheckLifes();

                for (int i = 0; i < shipPlayer.lifes - 1; i++) {

                    matrizLittle = UpdateFigurita({1.0f, 1.0f}, -3.14f / 2, {40.0f * (i + 1), 90.0f});
                    DrawLifes(matrizLittle);
                }

                // duplicar codigo para vidas restantes del otro jugador
                if (isMultiplayerActive) {

                    for (int i = 0; i < secondPlayerLifes - 1; i++) {

                        matrizLittle = UpdateFigurita({1.0f, 1.0f}, -3.14f / 2, {(windowX - 120) + (i * 40), 90.0f});
                        DrawLifes(matrizLittle);
                    }
                }

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
