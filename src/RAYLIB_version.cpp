#include "RAYLIB_version.hpp"
#include <cmath>

#define CAMERA_SPEED 0.3f //degrees per pixel
#define CAMERA_DISTANCE 7.f
#define CUBE_SIZE 3.f
#define TILE_SIZE (CUBE_SIZE/SIDE_LENGTH)
#define THIN 0.01f
#define THICK 0.02f
#define THICCK 0.03f

static const std::array<Color,ColorCount> RGB = {WHITE, MAGENTA, GREEN, RED, BLUE, YELLOW};

void Draw3DSide(const Side& side, Vector3 top_left, Vector3 delta_row, Vector3 delta_col, Vector3 tile)
{
    for (unsigned i=0; i<SIDE_LENGTH; ++i)
    {
        for (unsigned j=0; j<SIDE_LENGTH; ++j)
        {
            DrawCubeV
            (
                {top_left.x+i*delta_row.x+j*delta_col.x, top_left.y+i*delta_row.y+j*delta_col.y, top_left.z+i*delta_row.z+j*delta_col.z},
                tile, RGB[side[i][j]]
            );
        }
    }
}

void Draw3DCube(const Cube& cube)
{
    Draw3DSide(cube[Top],{-TILE_SIZE,CUBE_SIZE/2,-TILE_SIZE},{0,0,TILE_SIZE},{TILE_SIZE,0,0},{TILE_SIZE,THIN,TILE_SIZE});
    Draw3DSide(cube[Left],{-CUBE_SIZE/2,TILE_SIZE,-TILE_SIZE},{0,-TILE_SIZE,0},{0,0,TILE_SIZE},{THIN,TILE_SIZE,TILE_SIZE});
    Draw3DSide(cube[Front],{-TILE_SIZE,TILE_SIZE,CUBE_SIZE/2},{0,-TILE_SIZE,0},{TILE_SIZE,0,0},{TILE_SIZE,TILE_SIZE,THIN});
    Draw3DSide(cube[Right],{CUBE_SIZE/2,TILE_SIZE,TILE_SIZE},{0,-TILE_SIZE,0},{0,0,-TILE_SIZE},{THIN,TILE_SIZE,TILE_SIZE});
    Draw3DSide(cube[Back],{TILE_SIZE,TILE_SIZE,-CUBE_SIZE/2},{0,-TILE_SIZE,0},{-TILE_SIZE,0,0},{TILE_SIZE,TILE_SIZE,THIN});
    Draw3DSide(cube[Bottom],{-TILE_SIZE,-CUBE_SIZE/2,TILE_SIZE},{0,0,-TILE_SIZE},{TILE_SIZE,0,0},{TILE_SIZE,THIN,TILE_SIZE});

    DrawCubeV({(CUBE_SIZE+THICK)/2,0,TILE_SIZE/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({(CUBE_SIZE+THICK)/2,0,-TILE_SIZE/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({(CUBE_SIZE+THICK)/2,TILE_SIZE/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({(CUBE_SIZE+THICK)/2,-TILE_SIZE/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({-(CUBE_SIZE+THICK)/2,0,TILE_SIZE/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({-(CUBE_SIZE+THICK)/2,0,-TILE_SIZE/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({-(CUBE_SIZE+THICK)/2,TILE_SIZE/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({-(CUBE_SIZE+THICK)/2,-TILE_SIZE/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({0,(CUBE_SIZE+THICK)/2,TILE_SIZE/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
    DrawCubeV({0,(CUBE_SIZE+THICK)/2,-TILE_SIZE/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
    DrawCubeV({TILE_SIZE/2,(CUBE_SIZE+THICK)/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({-TILE_SIZE/2,(CUBE_SIZE+THICK)/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({0,-(CUBE_SIZE+THICK)/2,TILE_SIZE/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
    DrawCubeV({0,-(CUBE_SIZE+THICK)/2,-TILE_SIZE/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
    DrawCubeV({TILE_SIZE/2,-(CUBE_SIZE+THICK)/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({-TILE_SIZE/2,-(CUBE_SIZE+THICK)/2,0},{THICK,THICK,CUBE_SIZE+THICK},BLACK);
    DrawCubeV({TILE_SIZE/2,0,(CUBE_SIZE+THICK)/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({-TILE_SIZE/2,0,(CUBE_SIZE+THICK)/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({0,TILE_SIZE/2,(CUBE_SIZE+THICK)/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
    DrawCubeV({0,-TILE_SIZE/2,(CUBE_SIZE+THICK)/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
    DrawCubeV({TILE_SIZE/2,0,-(CUBE_SIZE+THICK)/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({-TILE_SIZE/2,0,-(CUBE_SIZE+THICK)/2},{THICK,CUBE_SIZE+THICK,THICK},BLACK);
    DrawCubeV({0,TILE_SIZE/2,-(CUBE_SIZE+THICK)/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
    DrawCubeV({0,-TILE_SIZE/2,-(CUBE_SIZE+THICK)/2},{CUBE_SIZE+THICK,THICK,THICK},BLACK);
}

Tile FindMouseTile3D(const Ray& ray, Sides side, Vector3 top_left, Vector3 delta_row, Vector3 delta_col)
{
    RayCollision collision;
    unsigned i=0;
    do {
        collision = GetRayCollisionBox(ray,
        {
            {
                top_left.x+i*delta_row.x,
                top_left.y+i*delta_row.y,
                top_left.z+i*delta_row.z
            },
            {
                top_left.x+(i+1)*delta_row.x+SIDE_LENGTH*delta_col.x,
                top_left.y+(i+1)*delta_row.y+SIDE_LENGTH*delta_col.y,
                top_left.z+(i+1)*delta_row.z+SIDE_LENGTH*delta_col.z
            }
        });
        ++i;
    } while (!collision.hit);
    unsigned j=0;
    do {
        collision = GetRayCollisionBox(ray,
        {
            {
                top_left.x+j*delta_col.x,
                top_left.y+j*delta_col.y,
                top_left.z+j*delta_col.z
            },
            {
                top_left.x+(j+1)*delta_col.x+SIDE_LENGTH*delta_row.x,
                top_left.y+(j+1)*delta_col.y+SIDE_LENGTH*delta_row.y,
                top_left.z+(j+1)*delta_col.z+SIDE_LENGTH*delta_row.z
            }
        });
        ++j;
    } while (!collision.hit);
    return Tile(side,--i,--j);
}

Tile FindMouse3D(const Camera3D& camera)
{
    Ray ray = GetMouseRay(GetMousePosition(),camera);
    RayCollision collision =
        GetRayCollisionBox(ray,{{-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2},{CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2}});
    if (!collision.hit)
        return Tile::invalid;

    if (camera.position.x > 0)
    {
        collision = GetRayCollisionBox(ray,{{CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2},{CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2}});
        if (collision.hit)
            return FindMouseTile3D
            (
                ray, Right,
                {CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2},
                {0,-TILE_SIZE,0}, {0,0,-TILE_SIZE}
            );
    }
    else
    {
        collision = GetRayCollisionBox(ray,{{-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2},{-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2}});
        if (collision.hit)
            return FindMouseTile3D
            (
                ray, Left,
                {-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2},
                {0,-TILE_SIZE,0}, {0,0,TILE_SIZE}
            );
    }

    if (camera.position.y > 0)
    {
        collision = GetRayCollisionBox(ray,{{-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2},{CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2}});
        if (collision.hit)
            return FindMouseTile3D
            (
                ray, Top,
                {-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2},
                {0,0,TILE_SIZE}, {TILE_SIZE,0,0}
            );
    }
    else
    {
        collision = GetRayCollisionBox(ray,{{-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2},{CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2}});
        if (collision.hit)
            return FindMouseTile3D
            (
                ray, Bottom,
                {-CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2},
                {0,0,-TILE_SIZE}, {TILE_SIZE,0,0}
            );
    }

    if (camera.position.z > 0)
    {
        collision = GetRayCollisionBox(ray,{{-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2},{CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2}});
        if (collision.hit)
            return FindMouseTile3D
            (
                ray, Front,
                {-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2},
                {0,-TILE_SIZE,0}, {TILE_SIZE,0,0}
            );
    }
    else
    {
        collision = GetRayCollisionBox(ray,{{-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2},{CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2}});
        if (collision.hit)
            return FindMouseTile3D
            (
                ray, Back,
                {CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2},
                {0,-TILE_SIZE,0}, {-TILE_SIZE,0,0}
            );
    }

    // Shouldn't happen
    return Tile::invalid;
}

void RAYLIB_IO(Cube& cube, Tile& mouse)
{
    char buffer[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bool enable_keybinds = true;

    InitWindow(WIND_WIDTH, WIND_HEIGHT, "Rubik!");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Camera3D camera;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 60;

    SetCameraMode(camera,CAMERA_CUSTOM);

    Vector2 camera_rotation = {-45, 30};
    camera.position =
    {
        sinf(DEG2RAD*camera_rotation.x) * cosf(DEG2RAD*camera_rotation.y) * CAMERA_DISTANCE,
        sinf(DEG2RAD*camera_rotation.y) * CAMERA_DISTANCE,
        cosf(DEG2RAD*camera_rotation.x) * cosf(DEG2RAD*camera_rotation.y) * CAMERA_DISTANCE
    };

    while (!WindowShouldClose())
    {
        mouse = FindMouse3D(camera);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) //grab cube
        {
            enable_keybinds = false;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) //rotate part of cube
        {

        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) //release cube
        {
            enable_keybinds = true;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) //grab camera
        {
            enable_keybinds = false;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) //move camera
        {
            auto move = GetMouseDelta();
            camera_rotation.x -= move.x * CAMERA_SPEED;
            camera_rotation.y += move.y * CAMERA_SPEED;
            if (camera_rotation.x > 180)
                camera_rotation.x -= 360;
            if (camera_rotation.x < -180)
                camera_rotation.x += 360;
            if (camera_rotation.y > 89)
                camera_rotation.y = 89;
            if (camera_rotation.y < -89)
                camera_rotation.y = -89;
            camera.position =
            {
                sinf(DEG2RAD*camera_rotation.x) * cosf(DEG2RAD*camera_rotation.y) * CAMERA_DISTANCE,
                sinf(DEG2RAD*camera_rotation.y) * CAMERA_DISTANCE,
                cosf(DEG2RAD*camera_rotation.x) * cosf(DEG2RAD*camera_rotation.y) * CAMERA_DISTANCE
            };
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) //release camera
        {
            enable_keybinds = true;
        }

        if (enable_keybinds)
        {
            if (IsKeyPressed(KEY_Q)) // rotate counterclockwise
            {
                MoveRoll(cube,mouse,false);
            }

            if (IsKeyPressed(KEY_W)) // pitch up
            {
                MovePitch(cube,mouse,true);
            }

            if (IsKeyPressed(KEY_E)) // rotate clockwise
            {
                MoveRoll(cube,mouse,true);
            }

            if (IsKeyPressed(KEY_A)) // yaw left
            {
                MoveYaw(cube,mouse,true);
            }

            if (IsKeyPressed(KEY_S)) // pitch down
            {
                MovePitch(cube,mouse,false);
            }

            if (IsKeyPressed(KEY_D)) // yaw right
            {
                MoveYaw(cube,mouse,false);
            }

            if (IsKeyPressed(KEY_SPACE)) // shuffle
            {
                Shuffle(cube);
            }
        }

        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera);

                Draw3DCube(cube);

            EndMode3D();

            DrawText("Press Space to shuffle!", (WIND_WIDTH-420)/2, WIND_HEIGHT-48, 32, LIGHTGRAY);
            DrawText(itoa((int)(camera_rotation.x),buffer,10),WIND_WIDTH-144,16,32,LIGHTGRAY);
            DrawText(itoa((int)(camera_rotation.y),buffer,10),WIND_WIDTH-64,16,32,LIGHTGRAY);
            DrawText(itoa(mouse.side,buffer,10),16,16,32,LIGHTGRAY);
            DrawText(itoa(mouse.row,buffer,10),48,16,32,LIGHTGRAY);
            DrawText(itoa(mouse.column,buffer,10),80,16,32,LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
}
