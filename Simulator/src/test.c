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


//--------------------------------------------------------------------------------
// Function Prototypes
//--------------------------------------------------------------------------------



int main(void){
    //-----------World Creation----------------------
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 10.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);

    //-----------Ball Creation----------------------
    b2BodyDef ballBodyDef = b2DefaultBodyDef();
    ballBodyDef.type = b2_dynamicBody;

    b2Circle ballGeometry = {.center = pixelToMeterV((b2Vec2){0.0f, 0.0f}), .radius = pixelToMeterS(5.0f)};
    b2ShapeDef  ballShapeDef = b2DefaultShapeDef();
    ballShapeDef.material.friction = 0.5f;

    b2BodyId ballIds[BALL_COUNT] = {0};
    int w = 5;
    int h = BALL_COUNT / w;
    for(int x = 0; x < w; x++){
        for(int y = 0; y < h; y++){
            float yp = pixelToMeterS(SCREEN_HEIGHT / 16.055f) + (ballGeometry.radius * y * 2.0f);
            float xp = pixelToMeterS(SCREEN_WIDTH / 2.055f) + (ballGeometry.radius * x * 2.0f);
            ballBodyDef.position = (b2Vec2){xp, yp};
            int index = (y*w) + x;
            ballIds[index] = b2CreateBody(worldId, &ballBodyDef);
            b2CreateCircleShape(ballIds[index], &ballShapeDef, &ballGeometry);
        }
    }

    //-----------Tumblr Creation----------------------
    b2BodyDef tmblrBodyDef = b2DefaultBodyDef();
    tmblrBodyDef.position = pixelToMeterV((b2Vec2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f});
    tmblrBodyDef.rotation = b2MakeRot(B2_PI/4.0f);
    b2BodyId tmblrId = b2CreateBody(worldId, &tmblrBodyDef);

    b2Polygon tmblrGeometry = {0};
    b2ShapeDef tmblrShapeDef = b2DefaultShapeDef();

    float cmpntHalfWidth = pixelToMeterS(20.0f)/2.0f;
    float cmpntHalfHeight = pixelToMeterS(10.0f)/2.0f;
    float radius = pixelToMeterS(100.0f);
    float resolution = 0.1f;
    int size = 2.0f / resolution;

    for(int i = 0; i < size; i++){
        float angle = 1 - (i * resolution);
        b2Vec2 pos = (b2Vec2){radius*cosf(B2_PI*angle), radius*sinf(B2_PI*angle)};
        b2Vec2 delta_p = b2Sub(tmblrBodyDef.position, pos);
        float rot =atan2f(delta_p.y, delta_p.x) + (B2_PI/2.0f);

        tmblrGeometry = b2MakeOffsetBox(cmpntHalfWidth, cmpntHalfHeight, pos, b2MakeRot(rot));
        tmblrShapeDef.density = 1.0f;
        tmblrShapeDef.material.friction = 0.3f;
        b2CreatePolygonShape(tmblrId, &tmblrShapeDef, &tmblrGeometry);
    }

    b2ShapeId shapeIds[size];
    int count = b2Body_GetShapes(tmblrId, shapeIds, size);
    B2_ASSERT(count == size);
    b2Vec2 cmpntPos[size]; 

    for(int i = 0; i < size; i++){
        cmpntPos[i] = b2Add(tmblrBodyDef.position, b2Shape_GetPolygon(shapeIds[i]).centroid);      
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tumblr Test");
    SetTargetFPS(60);

    while(!WindowShouldClose()){

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for(int i = 0; i < BALL_COUNT; i++){
                Vector2 pos = b2ToVec2(meterToPixelV(b2Body_GetPosition(ballIds[i])));
                DrawCircleLinesV(pos, meterToPixelS(ballGeometry.radius), ORANGE);
            }

            for(int i = 0; i < size; i++){
                Vector2 pos = b2ToVec2(meterToPixelV(cmpntPos[i]));
                Rectangle rec = (Rectangle){pos.x, pos.y, meterToPixelS(cmpntHalfWidth)*2.0f, meterToPixelS(cmpntHalfHeight)*2.0f};

                b2Vec2 cToPDelta = meterToPixelV(b2Sub(tmblrBodyDef.position, cmpntPos[i]));
                float angle = atan2f(cToPDelta.y, cToPDelta.x);
                DrawRectanglePro(rec, (Vector2){rec.width/2.0f, rec.height/2.0f}, ((angle + B2_PI/2.0f) * 180/B2_PI), MAROON);
                DrawLineV(pos, (Vector2){pos.x+10.0f*cosf(angle+B2_PI/2.0f), pos.y+10.0f*sinf(angle+B2_PI/2.0f)}, GREEN);
            }

            b2World_Step(worldId, timestep, subStepCount);
        EndDrawing();
    }
    
    CloseWindow();
    b2DestroyWorld(worldId);
    worldId =  b2_nullWorldId;
    return 0; 
}