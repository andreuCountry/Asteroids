#include <stdio.h>
#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <stdio.h>
#include <esat/math.h>
#include <esat/time.h>
#include <math.h>
#include <mathlib.h>

const int numPoints = 5, pi = 3.141592f;

esat::Vec3 g_figurita[numPoints] {
    {0.0f, 1.0f, 1.0f}, // esta ultima es la goat, homogenea, se repite por cada par de puntos, por eso hacemos Vec3
    {0.5f, 0.0f, 1.0f},
    //{0.25f, 0.0f, 1.0f},
    //{0.0f, 0.0f, 1.0f},
    {-0.5f, 0.0f, 1.0f},
};

float DegreeToRadians(float degree) {
    return degree * pi / 180.0f;
}
// Posicion de la naves
esat::Vec3 currentPosition {400, 300, 1};
esat::Vec3 speed = {0.2f, 0.2f, 0};
// Inicialización chapucera
esat::Vec3 newPosition = {0, 0, 0};

void InitShip() {
    g_figurita[0] = {cosf(DegreeToRadians(0.0f)) * 50, sinf(DegreeToRadians(0.0f)) * 50, 1.0f};
    g_figurita[1] = {cosf(DegreeToRadians(160.0f)) * 20, sinf(DegreeToRadians(160.0f)) * 20, 1.0f};
    g_figurita[2] = {cosf(DegreeToRadians(170.0f)) * 15, sinf(DegreeToRadians(170.0f)) * 15, 1.0f};
    g_figurita[3] = {cosf(DegreeToRadians(-170.0f)) * 15, sinf(DegreeToRadians(-170.0f)) * 15, 1.0f};
    g_figurita[4] = {cosf(DegreeToRadians(-160.0f)) * 20, sinf(DegreeToRadians(-160.0f)) * 20, 1.0f};
}

void Movement() {
    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Right)) {
        
        esat::Vec3 newPosition = {currentPosition.x + speed.x, currentPosition.y, 1};
        currentPosition = newPosition;
    }

    if (esat::IsSpecialKeyPressed(esat::kSpecialKey_Down)) {
        
        esat::Vec3 newPosition = {currentPosition.x, currentPosition.y + speed.y, 1};
        currentPosition = newPosition;
    }
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
    esat::DrawSetStrokeColor(0,0,0,255);
    esat::DrawSetFillColor(255,0,0,255);

    float points[numPoints * 2];

    m = esat::Mat3Multiply(esat::Mat3Translate(currentPosition.x, currentPosition.y), m);

    Movement();

    for (int i = 0; i < numPoints; i++) {
        // Necesitamos esto para transformar los Mat3 en Vec3, para dibujar
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, g_figurita[i]);
        points[i*2] = tmp.x;
        points[i*2+1] = tmp.y;
    }

    // Checkeito del movimiento de la nave
    esat::DrawSolidPath(points, numPoints);
}

int esat::main(int argc, char **argv) {

    esat::WindowInit(800, 600);
    WindowSetMouseVisibility(true);

    InitShip();

    esat::Mat3 matriz = UpdateFigurita({1.0f, 1.0f}, 0.0f, {0.0f, 0.0f});

    while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {

    	esat::DrawBegin();
    	esat::DrawClear(120,120,120);

        DrawFigurita(matriz, numPoints);
            

    	esat::DrawEnd();
        esat::WindowFrame();
        
    }

    esat::WindowDestroy();
    
    return 0;
}
