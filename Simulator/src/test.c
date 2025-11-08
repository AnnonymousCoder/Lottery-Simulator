/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute 'raylib_compile_execute' script
*   Note that compiled executable is placed in the same folder as .c file
*
*   To test the examples on Web, press F6 and execute 'raylib_compile_execute_web' script
*   Web version of the program is generated in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include "box2d.h"
#include "collision.h"
#include "math_functions.h"

#include <stdio.h>


//------------------------------------------------------------------------------------
// Defines and Macros
//------------------------------------------------------------------------------------
#define FPS 60
#define BODY_MASS 1.0f
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define PPM 10.0f
#define MPP (1.0f / PPM)

//------------------------------------------------------------------------------------
// Global Variables Declarations
//------------------------------------------------------------------------------------
float timestep = 1.0f / 60.0f;
int subStepCount = 4;

//------------------------------------------------------------------------------------
// Prototype Functions Declaration
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    b2Vec2 groundScale = (b2Vec2){(200.0f*MPP)/2.0f, (200.0f*MPP)/2.0f};
    b2Vec2 crateScale = (b2Vec2){(10.0f*MPP)/2.0f, (10.0f*MPP)/2.0f};

    //Creating a Box2D world [ The world manages all physics, memory, objects and simulations. ]
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 10.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    b2BodyDef groundBody = b2DefaultBodyDef();
    groundBody.position = (b2Vec2){400.0f*MPP, 400.0f*MPP};
    groundBody.rotation = b2MakeRot(B2_PI / 4.5f); 
    b2BodyId groundId = b2CreateBody(worldId, &groundBody);

    b2Polygon groundBox = b2MakeBox(groundScale.x, groundScale.y);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    b2BodyDef crateBody = b2DefaultBodyDef();
    crateBody.type = b2_dynamicBody;
    crateBody.position = (b2Vec2){400.0f*MPP, 10.0f*MPP};
    b2BodyId crateBodyId = b2CreateBody(worldId, &crateBody);

    b2Polygon crateBox = b2MakeBox(crateScale.x, crateScale.y);
    b2ShapeDef crateBoxShapeDef = b2DefaultShapeDef();
    crateBoxShapeDef.density = 1;
    crateBoxShapeDef.material.friction = 0.3f;
    b2CreatePolygonShape(crateBodyId, &crateBoxShapeDef, &crateBox);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib + Box2D Test");

    SetTargetFPS(FPS);

    b2Vec2 groundPos = b2MulSV(PPM, b2Body_GetPosition(groundId));
    Rectangle ground = (Rectangle){groundPos.x, groundPos.y, groundScale.x*2.0f*PPM, groundScale.y*2.0f*PPM};
    float groundRotation = b2Rot_GetAngle(b2Body_GetRotation(groundId)) * (180.0f/B2_PI);
    
    b2MassData m_crate = b2Body_GetMassData(crateBodyId);
    printf("Crate mass: %.4f\n", m_crate.rotationalInertia);

    while(!WindowShouldClose()){

        b2Vec2 cratePos = b2MulSV(PPM,b2Body_GetPosition(crateBodyId));
        Rectangle crate = (Rectangle){cratePos.x, cratePos.y, crateScale.x*2.0f*PPM, crateScale.y*2.0f*PPM};
        float crateRotation = b2Rot_GetAngle(b2Body_GetRotation(crateBodyId)) * (180.0f/B2_PI);

        BeginDrawing();

            ClearBackground(RAYWHITE);

            

            DrawText(TextFormat("pb:[%.4f, %.4f], rot: %.4f", cratePos.x, cratePos.y, crateRotation), 400, 10, 20, ORANGE);
            // DrawText(TextFormat("pg:[%.4f, %.4f]", groundPos.x, groundPos.y), 600, 10, 20, ORANGE);
            
            DrawRectanglePro(crate, (Vector2){crateScale.x*PPM, crateScale.y*PPM}, crateRotation, GREEN);
            DrawRectanglePro(ground, (Vector2){groundScale.x*PPM, groundScale.y*PPM}, groundRotation, MAROON);

            b2World_Step(worldId, timestep, subStepCount);
        EndDrawing();
    }

    CloseWindow();
    b2DestroyWorld(worldId);
    worldId = b2_nullWorldId;
    return 0;
}