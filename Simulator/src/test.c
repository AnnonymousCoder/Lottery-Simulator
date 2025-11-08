#include "raylib.h"
#include "box2d.h"
#include "math_functions.h"

#include <stdio.h>
//--------------------------------------------------------------------------------
// Macro Definitions
//--------------------------------------------------------------------------------
#define PPM 10.0f
#define MPP (1.0f / PPM)
#define BALL_COUNT 60
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

//--------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------
float timestep = 1.0f / 60.0f;
int subStepCount = 4;

//--------------------------------------------------------------------------------
// Helper Function Definitions
//--------------------------------------------------------------------------------

//Converts pixel vector coordinates to meters vector coordinates using the predefined PPM [pixel per meter] constant 
b2Vec2 pixelToMeterV(b2Vec2 pixel){
    return b2MulSV(MPP, pixel);
}

//Converts vector coordinates in meters to pixel vector coordinates using the predefined MPP [meter per pixel] constant 
b2Vec2 meterToPixelV(b2Vec2 meter){
    return b2MulSV(PPM, meter);
}

//Converts pixel scaler quantity to meter unit using the defined constant PPM
float pixelToMeterS(float pixel){
    return pixel * MPP;
}

//Converts meter scaler quantity to pixel unit using the defined constant MPP
float meterToPixelS(float meter){
    return meter * PPM;
}

//Convert Box2D vect2 to raylib's Vector2 representation
Vector2 b2ToVec2(b2Vec2 vector){return (Vector2){vector.x, vector.y};}

int main(void){
    // Physics World Creation for simulation
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 10.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    //Creating a ball body, geometry and shape
    b2BodyDef ballBodyDef = b2DefaultBodyDef();
    ballBodyDef.type = b2_dynamicBody;

    b2Circle ballGeometry = {.center=(b2Vec2){0.0f, 0.0f}, .radius=pixelToMeterS(5.0f)};
    b2ShapeDef ballShapeDef = b2DefaultShapeDef();
    ballShapeDef.material.friction = 0.3f;


    //Creating a Circular 'glass' container's body, geometry and shape
    b2BodyDef containerBodyDef = b2DefaultBodyDef();
    containerBodyDef.position = pixelToMeterV((b2Vec2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f});
    b2BodyId containerBodyId = b2CreateBody(worldId, &containerBodyDef);

    float rad = pixelToMeterS(200.0f)+0.2f;
    float res = 0.01f;
    const int size = 2.0f / res;

    b2Vec2 points[size];
    for(int i = 0; i < size; i++){
        float angle = 1 - i*res;
        float x = rad*cosf(B2_PI*angle);
        float y = rad*sinf(B2_PI*angle);
        points[i] = (b2Vec2){x, y};
    }

    b2ChainDef containerShapeDef = b2DefaultChainDef();
    containerShapeDef.points = points;
    containerShapeDef.isLoop = true;
    containerShapeDef.count = size;
    b2ChainId containerId = b2CreateChain(containerBodyId, &containerShapeDef);


    //Populating the World with 60 ball objects
    b2BodyId ballIds[BALL_COUNT] = {0};

    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 12; y++){
            float yp = pixelToMeterS(SCREEN_HEIGHT / 16.0f) + (ballGeometry.radius * y * 2.0f);
            float xp = pixelToMeterS(SCREEN_WIDTH / 2.05f) + (ballGeometry.radius * x * 2.0f);
            int index = (y*5) + x;
            ballBodyDef.position = (b2Vec2){xp, yp};
            ballIds[index] = b2CreateBody(worldId, &ballBodyDef);
            b2CreateCircleShape(ballIds[index], &ballShapeDef, &ballGeometry);
        }
    }

    // b2Vec2 bp = b2Body_GetPosition(ballIds[0]);
    // printf("p:[%.4f, %.4f]\n", bp.x, bp.y);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lottery Simulator v.1.0.0");

    SetTargetFPS(60);
    
    while(!WindowShouldClose()){

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for(int i = 0; i < BALL_COUNT; i++){
                b2Vec2 bp = b2Body_GetPosition(ballIds[i]);
                DrawCircleLinesV(b2ToVec2(meterToPixelV(bp)), meterToPixelS(ballGeometry.radius), MAROON);
            }

            for(int i = 0; i < size; i++){
                b2Vec2 pos = b2Add(meterToPixelV(points[i]), (b2Vec2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f});
                DrawCircleV(b2ToVec2(pos), 1.0f, ORANGE);
            }

            b2World_Step(worldId, timestep, subStepCount);
        EndDrawing();
    }

    CloseWindow();
    b2DestroyWorld(worldId);
    worldId = b2_nullWorldId;
    return 0; 
}