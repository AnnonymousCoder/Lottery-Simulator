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
#include "raymath.h"
#include <stdio.h>

//------------------------------------------------------------------------------------
// Defines and Macros
//------------------------------------------------------------------------------------
#define FPS 60
#define BODY_MASS 1.0f
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define COEFF_OF_RESITUTION 0.7f
#define GRAVITY_ACCELERATION 9.81f
//------------------------------------------------------------------------------------
// Global Variables Declarations
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Prototype Functions Declaration
//------------------------------------------------------------------------------------
void u_physics(Vector2);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lottery Simulator");

    SetTargetFPS(FPS);

    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(RAYWHITE);

        
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void u_physics(Vector2 force){

}