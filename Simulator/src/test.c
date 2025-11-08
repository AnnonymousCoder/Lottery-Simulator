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

    //Create a Rigid Dynamic Body that represents our crate
    b2BodyDef crateBodyDef = b2DefaultBodyDef();
    crateBodyDef.type = b2_dynamicBody;
    crateBodyDef.isEnabled = false;
    crateBodyDef.position = (b2Vec2){400.0f*MPP, (-crateScale.y*2.0f)};

    //Create a Box Geometry and Shape for our crate body
    b2Polygon crateBox = b2MakeBox(crateScale.x, crateScale.y);
    b2ShapeDef crateBoxShapeDef = b2DefaultShapeDef();
    crateBoxShapeDef.density = 1;
    crateBoxShapeDef.material.friction = 0.3f;

    b2BodyId crateIds[10] = {0};
    for(int i = 0; i < 10; i++){
        crateIds[i] = b2CreateBody(worldId, &crateBodyDef);
        b2CreatePolygonShape(crateIds[i], &crateBoxShapeDef, &crateBox);
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib + Box2D Test");

    SetTargetFPS(FPS);

    b2Vec2 groundPos = b2MulSV(PPM, b2Body_GetPosition(groundId));
    Rectangle ground = (Rectangle){groundPos.x, groundPos.y, groundScale.x*2.0f*PPM, groundScale.y*2.0f*PPM};
    float groundRotation = b2Rot_GetAngle(b2Body_GetRotation(groundId)) * (180.0f/B2_PI);
    
    int crateIndex = 0;
    bool firstIteration = true;
    int deletionCount = 0;

    while(!WindowShouldClose()){

        if(crateIndex >= 10){
            crateIndex = 0;
            firstIteration = false;
        }

        b2Vec2 cratePos = b2MulSV(PPM,b2Body_GetPosition(crateIds[crateIndex]));
        Rectangle crate = (Rectangle){cratePos.x, cratePos.y, crateScale.x*2.0f*PPM, crateScale.y*2.0f*PPM};
        float crateRotation = b2Rot_GetAngle(b2Body_GetRotation(crateIds[crateIndex])) * (180.0f/B2_PI);
        
        if(!b2Body_IsEnabled(crateIds[crateIndex]))
        {
            b2Body_Enable(crateIds[crateIndex]);
        }

        if(cratePos.x > (SCREEN_WIDTH+(crateScale.x*PPM)) || cratePos.x < -(crateScale.x*PPM) || cratePos.y > (SCREEN_HEIGHT+(crateScale.y*PPM))){
            b2Body_Disable(crateIds[crateIndex]);
            if(crateIndex >= 5){
                b2DestroyBody(crateIds[crateIndex-5]);
                crateIds[crateIndex-5] = b2CreateBody(worldId, &crateBodyDef);
                b2CreatePolygonShape(crateIds[crateIndex-5], &crateBoxShapeDef, &crateBox);
                deletionCount += 1;
            }else if(!firstIteration){
                b2DestroyBody(crateIds[5+crateIndex]);
                crateIds[5+crateIndex] = b2CreateBody(worldId, &crateBodyDef);
                b2CreatePolygonShape(crateIds[5+crateIndex], &crateBoxShapeDef, &crateBox);
                deletionCount += 1;
            }
            crateIndex += 1;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText(TextFormat("p_del: %d vs p_ren: %d", deletionCount, crateIndex), 400, 10, 20, ORANGE);
            
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