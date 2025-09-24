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
// Program main entry point
//------------------------------------------------------------------------------------

Vector2 acc;
Vector2 vel;
Vector2 pos;
Vector2 force;
Vector2 ref_vel; 
Vector2 delta_vel;

float grav_acc = 9.8;
float mass = 5.0;
double ref_time = 0.0;
double delta_t = 0.0;

void u_physics(Vector2);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1040;
    const int screenHeight = 820;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    Vector2 momentum  = {0.0f, 0.0f};

    bool apply_force = false;
    float mag = 0.0f;
    float angle = 0.0f;

    pos.x = screenWidth/2;
    pos.y = 28.0f;
    vel   = Vector2Zero();
    force = Vector2Zero();
    ref_vel   = Vector2Zero();
    delta_vel = Vector2Zero();

    force.y = mass * grav_acc;  //Applying Gravity force

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        ref_vel = vel;

        // if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        //     apply_force = true;
        //     float force_x = -mag * cosf(angle);
        //     float force_y = -mag * sinf(angle);
        //     // vel.x = (force_x * delta_t) / mass;
        //     vel.y += (force_y * delta_t) / mass;
        //     // printf("Velocity: (%.3f, %.3f), Force: (%.3f, %.3f), delta_t: %.6f\n", vel.x, vel.y, force_x, force_y, delta_t);
        // }

        if(!apply_force){
            int px = GetMouseX() - screenWidth/2;
            int py = GetMouseY() - screenHeight/2;
            mag = sqrtf(px * px + py * py);
            angle = atan2f(py, px);
        }

        if(pos.x + 25 > screenWidth || pos.x - 25 < 0.0f){
            momentum.x = delta_vel.x * mass;
            force.x += (-momentum.x / (delta_t*1e6));
            vel.x *= -0.5f;
            pos.x = (pos.x + 25 > screenWidth) ? screenWidth - 25 : 25;
        }

        if(pos.y + 25 > screenHeight || pos.y - 25 < 0.0f){
            momentum.y = delta_vel.y * mass;
            force.y += (-momentum.y / (delta_t*1e6));
            vel.y *= -0.5f;
            pos.y = (pos.y + 25 > screenHeight) ? screenHeight - 25 : 25;
            // printf("Force: (%.3f, %.3f), Vel: (%.3f, %.3f), Momentum: (%.3f, %.3f), delta_vel: (%.6f, %.6f), delta_t: %.6f\n", force.x, force.y, vel.x, vel.y, momentum.x, momentum.y, delta_vel.x, delta_vel.y, delta_t);
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            apply_force = true;
            float force_x = -mag * cosf(angle) + force.x;
            float force_y = -mag * sinf(angle) + force.y;
            vel.y   += (force_y * delta_t*1e6) / mass;
            vel.x   += (force_x * delta_t*1e6) / mass;
            // printf("Velocity: (%.3f, %.3f), Force: (%.3f, %.3f), delta_t: %.6f\n", vel.x, vel.y, force_x, force_y, delta_t);
        }

        u_physics(force);
        // printf("Force: (%.3f, %.3f), Vel: (%.3f, %.3f), Momentum: (%.3f, %.3f), delta_vel: (%.6f, %.6f), delta_t: %.6f\n", force.x, force.y, vel.x, vel.y, momentum.x, momentum.y, delta_vel.x, delta_vel.y, delta_t);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText(TextFormat("pressed: %s, Mag : %.2f, Angle: %.3f, Force : (%.3f, %.3f), vel:(%.3f, %.3f)", apply_force ? "true" : "false", mag, angle, force.x, force.y, vel.x, vel.y), 10, 10, 20, LIGHTGRAY);
            DrawLine(screenWidth/2, screenHeight/2, (-mag * cosf(angle)) + screenWidth/2, (-mag * sinf(angle)) + screenHeight/2, BLACK);
            DrawCircleV(pos, 25, MAROON);

        EndDrawing();
        ref_time = GetTime();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void u_physics(Vector2 force){

    delta_t = GetTime() - ref_time;

    acc.x += force.x / mass;
    acc.y += force.y / mass;

    vel.x += acc.x * delta_t;
    vel.y += acc.y * delta_t;

    pos.x += vel.x;
    pos.y += vel.y;

    delta_vel = Vector2Subtract(vel, ref_vel);
}