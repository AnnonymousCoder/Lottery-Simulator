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
#define PIXEL_TO_METER(p) (p*MPP)   //Converts pixel scaler quantity (p) to meter unit using the defined constant PPM
#define METER_TO_PIXEL(m) (m*PPM)   //Converts meter scaler quantity (m) to pixel unit using the defined constant MPP
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

//Convert Box2D vect2 to raylib's Vector2 representation
Vector2 b2ToVec2(b2Vec2 vector){return (Vector2){vector.x, vector.y};}

//--------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------
const float timestep = 1.0f / 60.0f;
const int subStepCount = 4;


const float ballRadius = PIXEL_TO_METER(5.0f);
const float ballMass = 80.0f * 0.001f; //Rubber ball weight
const float ballFriction = 0.90f; //Friction coeff of Rubber on Glass
const float ballRestitution = 0.85f; //Bounce strength of Range 0.85-0.95 for rubber
const float ballRollingResistance = 0.01f; //Range 0.01-0.05 for rubber
const float ballVolume = (4.0f / 3.0f) * B2_PI * (ballRadius * ballRadius * ballRadius);

const float rotorTeethHalfWidth = PIXEL_TO_METER(5.0f);
const float rotorTeethHalfHeight = PIXEL_TO_METER(2.0f);
const float rotorRadius = PIXEL_TO_METER(200.0f);
const float rotorFriction = 0.3f;
const float rotorDensity = 1.0f;
const float rotorAngularVel = B2_PI/2.0f;
const float rotorResolution = 0.5f;
const int   rotorTeethSize = 2.0f / rotorResolution;

const float shellRadius = rotorRadius+ballRadius;
const float shellResolution  = 0.01f;
const int   shellSegSize     = 2.0f / shellResolution;

//--------------------------------------------------------------------------------
// Function Prototypes
//--------------------------------------------------------------------------------

//Creates and Populate the world with the specified amouunt of lotteryBalls.
//@param  worldId    world to populate with lottery balls.
//@param  out        pointer to b2BodyId array that stores the newly created balls object Ids.
void LotteryBallsCreation(b2WorldId worldId, b2BodyId out[BALL_COUNT]);

//Creates a Tumblr object in the world. Tumblr is used to describe the container that
//would hold and mix all of the lotteryBalls. A Tumblr consists of 2 parts, a rotor 
//and a shell.
// a.) Rotor - is kinematic, meaning it spins and interacts with the lotteryBalls.
//             The rotor handles the mixing. 
// b.) Shell - is the outside wall of the Tumblr that encloses the balls and rotor.
//             The shell is set to be static, meaning it doesn't move.
//
//@param    worldId         Id of world where the tumblr belongs [same as lotteryBalls].
//@param    shellSegments   pointer to array that will store the segments coordinates that comprise of the outer Tumblr Shell. 
//@param    rotorTeeth      pointer to array that will store the rotor teeth coordinates.
//@return   The newly created rotor's Id.
b2BodyId TumblrCreation(b2WorldId worldId, Vector2 shellSegments[shellSegSize], b2Vec2 rotorTeeth[rotorTeethSize]);

//Draws the Tumblr's Rotor component on screen.
//@param    rotorAngle      rotation of the rotor in radians.
//@param    rotorTransform  transform component of the rotor
//@param    teeth           pointer to array that contains the coordinates of the rotor teeth.
void DrawRotor(float rotorAngle, b2Transform rotorTransform, b2Vec2 teeth[rotorTeethSize]);

//Draws the LotteryBalls on screen
//@param    balls   pointer to array that contains the ball object Ids.
void DrawBalls(b2BodyId balls[BALL_COUNT]);

int main(void){
    //-----------World Creation----------------------
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 10.0f};
    b2WorldId worldId = b2CreateWorld(&worldDef);


    b2BodyId ballIds[BALL_COUNT];
    LotteryBallsCreation(worldId, ballIds);

    Vector2 segments[shellSegSize];
    b2Vec2 teeth[rotorTeethSize];
    b2BodyId rotorId = TumblrCreation(worldId, segments, teeth);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tumblr Test");
    SetTargetFPS(60);

    b2Transform rotorTransform = b2Body_GetTransform(rotorId);

    while(!WindowShouldClose()){

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, MAROON);
            DrawBalls(ballIds);
            DrawRotor(b2Rot_GetAngle(b2Body_GetRotation(rotorId)), rotorTransform, teeth);

            DrawLineStrip(segments, shellSegSize, BLACK);
            DrawLineV(segments[0], segments[shellSegSize-1], BLACK);

            b2World_Step(worldId, timestep, subStepCount);
        EndDrawing();
    }
    
    CloseWindow();
    b2DestroyWorld(worldId);
    worldId =  b2_nullWorldId;
    return 0; 
}


void LotteryBallsCreation(b2WorldId worldId, b2BodyId out[BALL_COUNT]){
    b2BodyDef ballBodyDef = b2DefaultBodyDef();
    ballBodyDef.type = b2_dynamicBody;

    b2Circle ballGeometry = {.center = pixelToMeterV((b2Vec2){0.0f, 0.0f}), .radius = ballRadius};
    b2ShapeDef  ballShapeDef = b2DefaultShapeDef();
    ballShapeDef.density = ballMass / ballVolume;
    ballShapeDef.material.friction = ballFriction;
    ballShapeDef.material.restitution = ballRestitution;
    ballShapeDef.material.rollingResistance = ballRollingResistance;

    int w = 5;
    int h = BALL_COUNT / w;
    for(int x = 0; x < w; x++){
        for(int y = 0; y < h; y++){
            float yp = PIXEL_TO_METER(SCREEN_HEIGHT / 3.055f) + (ballRadius * y * 2.0f);
            float xp = PIXEL_TO_METER(SCREEN_WIDTH / 2.055f) + (ballRadius * x * 2.0f);
            ballBodyDef.position = (b2Vec2){xp, yp};
            int index = (y*w) + x;
            out[index] = b2CreateBody(worldId, &ballBodyDef);
            b2CreateCircleShape(out[index], &ballShapeDef, &ballGeometry);
        }
    }
}

//Helper function to create rotor teeth for the Tumblr's Rotor component.
//@param tumblrId           rotor object's Id
//@param rotorTransform     rotor object's transform component
//@param rotorGeometry      rotor's physical geometry of type Polygon
//@param rotorShapeDef      rotor's Shape definition
void createRotorTeeth(b2BodyId rotorId, b2Transform rotorTransform, b2Polygon* rotorGeometry, b2ShapeDef* rotorShapeDef){
    for(int i = 0; i < rotorTeethSize; i++){
        float angle = 1 - (i * rotorResolution);
        b2Vec2 localPos = (b2Vec2){rotorRadius*cosf(B2_PI*angle), rotorRadius*sinf(B2_PI*angle)};
        b2Vec2 worldPos = b2TransformPoint(rotorTransform, localPos);

        b2Vec2 delta_p = b2Sub(rotorTransform.p, worldPos);
        float rot = atan2f(delta_p.y, delta_p.x) + B2_PI/2.0f;

        *rotorGeometry = b2MakeOffsetBox(rotorTeethHalfWidth, rotorTeethHalfHeight, localPos, b2MakeRot(rot));
        rotorShapeDef->density = rotorDensity;
        rotorShapeDef->material.friction = rotorFriction;
        b2CreatePolygonShape(rotorId, rotorShapeDef, rotorGeometry);
    }
}

//Creates tumblr's shell component. The shell is built as a chain of line segments.
//@param    worldId     Id of the world that the tumblr exists in.
//@param    out         Array of type Vector2 that will store the calculated segments.        
void createTumblrShell(b2WorldId worldId, Vector2 out[shellSegSize]){
    b2BodyDef tmblrShellDef = b2DefaultBodyDef();
    tmblrShellDef.position = pixelToMeterV((b2Vec2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f});
    b2BodyId tmblrShellId = b2CreateBody(worldId, &tmblrShellDef);
    
    
    b2Vec2 shellSegments[shellSegSize];
    
    for(int i = 0; i < shellSegSize; i++){
        float angle = 1 - (i * shellResolution);
        shellSegments[i] = (b2Vec2){shellRadius*cosf(B2_PI*angle), shellRadius*sinf(B2_PI*angle)};
    }

    b2ChainDef tmblrShellGeometryDef = b2DefaultChainDef();
    tmblrShellGeometryDef.points = shellSegments;
    tmblrShellGeometryDef.isLoop = true;
    tmblrShellGeometryDef.count = shellSegSize;
    b2CreateChain(tmblrShellId, &tmblrShellGeometryDef); 

    b2Transform tmblrShellTransform = b2Body_GetTransform(tmblrShellId);
    
    for(int i = 0; i < shellSegSize; i++){
        out[i] = b2ToVec2(meterToPixelV(b2TransformPoint(tmblrShellTransform, shellSegments[i])));
    }
}

b2BodyId TumblrCreation(b2WorldId worldId, Vector2 shellSegments[shellSegSize], b2Vec2 rotorTeeth[rotorTeethSize]){
    b2BodyDef tmblrRotorDef = b2DefaultBodyDef();
    tmblrRotorDef.position = pixelToMeterV((b2Vec2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f});
    tmblrRotorDef.type = b2_kinematicBody;
    tmblrRotorDef.angularVelocity = rotorAngularVel;
    b2BodyId tmblrRotorId = b2CreateBody(worldId, &tmblrRotorDef);


    b2Polygon tmblrRotorGeometry = {0};
    b2ShapeDef tmblrRotorShapeDef = b2DefaultShapeDef();


    b2Transform tmblrTransform = b2Body_GetTransform(tmblrRotorId);

    createRotorTeeth(tmblrRotorId, tmblrTransform, &tmblrRotorGeometry, &tmblrRotorShapeDef);

    b2ShapeId shapeIds[rotorTeethSize];
    int count = b2Body_GetShapes(tmblrRotorId, shapeIds, rotorTeethSize);
    B2_ASSERT(count == rotorTeethSize);

    for(int i = 0; i < rotorTeethSize; i++){
        rotorTeeth[i] =  b2TransformPoint(tmblrTransform, b2Shape_GetPolygon(shapeIds[i]).centroid);
    }

    createTumblrShell(worldId, shellSegments);

    return tmblrRotorId;
}

void DrawRotor(float rotorAngle, b2Transform rotorTransform, b2Vec2 teeth[rotorTeethSize]){
    float width  = METER_TO_PIXEL(rotorTeethHalfWidth);
    float height = METER_TO_PIXEL(rotorTeethHalfHeight);

    for(int i = 0; i < rotorTeethSize; i++){
        b2Vec2 worldPos = teeth[i];

        b2Vec2 cToPDelta = b2Sub(rotorTransform.p, worldPos);
        float angle = atan2f(cToPDelta.y, cToPDelta.x);

        float magnitude = METER_TO_PIXEL(b2Length(b2InvTransformPoint(rotorTransform, worldPos)));
        
        Rectangle rec = (Rectangle){0};
        rec.x = SCREEN_WIDTH/2.0f+magnitude*cosf(angle+rotorAngle);
        rec.y = SCREEN_HEIGHT/2.0f+magnitude*sinf(angle+rotorAngle);
        rec.width = width*2.0f;
        rec.height = height*2.0f;
        
        DrawRectanglePro(rec, (Vector2){width, height}, (((angle+rotorAngle) + B2_PI/2.0f) * RAD2DEG), MAROON);
        
        //Draw Lines to each teeth center point for debugging
        DrawLineV((Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f}, (Vector2){SCREEN_WIDTH/2.0f+magnitude*cosf(angle+rotorAngle), SCREEN_HEIGHT/2.0f+magnitude*sinf(angle+rotorAngle)}, GREEN);
    }
}

void DrawBalls(b2BodyId balls[BALL_COUNT]){
    for(int i = 0; i < BALL_COUNT; i++){
        Vector2 pos = b2ToVec2(meterToPixelV(b2Body_GetPosition(balls[i])));
        DrawCircleLinesV(pos, METER_TO_PIXEL(ballRadius), ORANGE);
    }
}
