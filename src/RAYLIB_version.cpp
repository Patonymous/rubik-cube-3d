#ifndef USE_SFML

#include "RAYLIB_version.hpp"
#include <cmath>
#include <queue>

#define CAMERA_MOUSE_SPEED 0.3f //degrees per pixel
#define CAMERA_KEYBOARD_SPEED 1.f //degrees per frame
#define CAMERA_DISTANCE 7.f
#define VIS_THRESHOLD (asinf(CUBE_SIZE/2/CAMERA_DISTANCE)) //radians
#define CUBE_SIZE 3.f
#define TILE_SIZE (CUBE_SIZE/SIDE_LENGTH)
#define THICK 0.00375f
#define ANIM_MOUSE_SPEED 0.3f //degrees per pixel
#define ANIM_AUTO_SPEED 5.f //degrees per frame
#define ANIM_SHUFFLE_SPEED 15.f //degrees per frame
#define LINE_COLOR BLACK
#define SIDE_COLOR GRAY

static const std::array<Color,ColorCount> RGB = {WHITE, ORANGE, GREEN, RED, BLUE, YELLOW};

void DrawQuad3D(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Color color)
{
    DrawTriangle3D(v1,v2,v3,color);
    DrawTriangle3D(v3,v4,v1,color);
}

void DrawRubikLine3D
    (const Side& side, Vector3 top_left, Vector3 delta_row, Vector3 delta_col,
    Vector3 line_offset, unsigned which, bool vertical)
{
    if (vertical)
        for (unsigned i=0; i<SIDE_LENGTH; ++i)
        {
            DrawQuad3D
            (
                {
                    top_left.x+i*delta_row.x,
                    top_left.y+i*delta_row.y,
                    top_left.z+i*delta_row.z
                },
                {
                    top_left.x+(i+1)*delta_row.x,
                    top_left.y+(i+1)*delta_row.y,
                    top_left.z+(i+1)*delta_row.z
                },
                {
                    top_left.x+(i+1)*delta_row.x+delta_col.x,
                    top_left.y+(i+1)*delta_row.y+delta_col.y,
                    top_left.z+(i+1)*delta_row.z+delta_col.z
                },
                {
                    top_left.x+i*delta_row.x+delta_col.x,
                    top_left.y+i*delta_row.y+delta_col.y,
                    top_left.z+i*delta_row.z+delta_col.z
                },
                RGB[side[i][which]]
            );
            if (i != 0)
                DrawLine3D
                (
                    {
                        top_left.x+i*delta_row.x+line_offset.x,
                        top_left.y+i*delta_row.y+line_offset.y,
                        top_left.z+i*delta_row.z+line_offset.z
                    },
                    {
                        top_left.x+i*delta_row.x+delta_col.x+line_offset.x,
                        top_left.y+i*delta_row.y+delta_col.y+line_offset.y,
                        top_left.z+i*delta_row.z+delta_col.z+line_offset.z
                    },
                    LINE_COLOR
                );
        }
    else
        for (unsigned i=0; i<SIDE_LENGTH; ++i)
        {
            DrawQuad3D
            (
                {
                    top_left.x+i*delta_col.x,
                    top_left.y+i*delta_col.y,
                    top_left.z+i*delta_col.z
                },
                {
                    top_left.x+i*delta_col.x+delta_row.x,
                    top_left.y+i*delta_col.y+delta_row.y,
                    top_left.z+i*delta_col.z+delta_row.z
                },
                {
                    top_left.x+(i+1)*delta_col.x+delta_row.x,
                    top_left.y+(i+1)*delta_col.y+delta_row.y,
                    top_left.z+(i+1)*delta_col.z+delta_row.z
                },
                {
                    top_left.x+(i+1)*delta_col.x,
                    top_left.y+(i+1)*delta_col.y,
                    top_left.z+(i+1)*delta_col.z
                },
                RGB[side[which][i]]
            );
            if (i != 0)
                DrawLine3D
                (
                    {
                        top_left.x+i*delta_col.x+line_offset.x,
                        top_left.y+i*delta_col.y+line_offset.y,
                        top_left.z+i*delta_col.z+line_offset.z
                    },
                    {
                        top_left.x+i*delta_col.x+delta_row.x+line_offset.x,
                        top_left.y+i*delta_col.y+delta_row.y+line_offset.y,
                        top_left.z+i*delta_col.z+delta_row.z+line_offset.z
                    },
                    LINE_COLOR
                );
        }
}

void DrawRubikSide3D
    (const Side& side, Vector3 top_left, Vector3 delta_row, Vector3 delta_col,
    Vector3 line_offset, bool vertical)
{
    for (unsigned i=0; i<SIDE_LENGTH; ++i)
    {
        DrawRubikLine3D
        (
            side,
            {
                top_left.x+i*(vertical?delta_col.x:delta_row.x),
                top_left.y+i*(vertical?delta_col.y:delta_row.y),
                top_left.z+i*(vertical?delta_col.z:delta_row.z)
            },
            delta_row, delta_col,
            line_offset,
            i, vertical
        );
        if (i != 0)
            DrawLine3D
            (
                {
                    top_left.x+i*(vertical?delta_col.x:delta_row.x)+line_offset.x,
                    top_left.y+i*(vertical?delta_col.y:delta_row.y)+line_offset.y,
                    top_left.z+i*(vertical?delta_col.z:delta_row.z)+line_offset.z
                },
                {
                    top_left.x+i*(vertical?delta_col.x:delta_row.x)
                        +SIDE_LENGTH*(vertical?delta_row.x:delta_col.x)+line_offset.x,
                    top_left.y+i*(vertical?delta_col.y:delta_row.y)
                        +SIDE_LENGTH*(vertical?delta_row.y:delta_col.y)+line_offset.y,
                    top_left.z+i*(vertical?delta_col.z:delta_row.z)
                        +SIDE_LENGTH*(vertical?delta_row.z:delta_col.z)+line_offset.z
                },
                LINE_COLOR
            );
    }
}

Vector3 Transform3D(Animation anim, Vector3 vector)
{
    switch (anim.type)
    {
    case Animation::Pitch:
        return
        {
            vector.x,
            cosf(anim.angle*DEG2RAD)*vector.y+sinf(anim.angle*DEG2RAD)*vector.z,
            cosf(anim.angle*DEG2RAD)*vector.z-sinf(anim.angle*DEG2RAD)*vector.y
        };
        break;

    case Animation::Yaw:
        return
        {
            cosf(anim.angle*DEG2RAD)*vector.x+sinf(anim.angle*DEG2RAD)*vector.z,
            vector.y,
            cosf(anim.angle*DEG2RAD)*vector.z-sinf(anim.angle*DEG2RAD)*vector.x
        };
        break;

    case Animation::Roll:
        return
        {
            cosf(anim.angle*DEG2RAD)*vector.x+sinf(anim.angle*DEG2RAD)*vector.y,
            cosf(anim.angle*DEG2RAD)*vector.y-sinf(anim.angle*DEG2RAD)*vector.x,
            vector.z
        };
        break;

    default:
        return vector;
        break;
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="

void DrawRubikCube3D(const Cube& cube, const CameraProperties& camera, const Animation& anim)
{
    // Draw Top
    if (true)
    {
        Sides side = Top;
        Vector3 top_left = {-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2};
        Vector3 delta_row = {0,0,TILE_SIZE};
        Vector3 delta_col = {TILE_SIZE,0,0};
        Vector3 line_offset = {0, THICK, 0};
        Vector3 rot_top_left = Transform3D(anim,top_left);
        Vector3 rot_delta_row = Transform3D(anim,delta_row);
        Vector3 rot_delta_col = Transform3D(anim,delta_col);
        switch (anim.type)
        {
        case Animation::Yaw:
            if (anim.where.row == 0)
            {
                DrawRubikSide3D(cube[side],rot_top_left,rot_delta_row,rot_delta_col,line_offset,true);
                break;
            }
        default:
        case Animation::None:
            DrawRubikSide3D(cube[Top],top_left,delta_row,delta_col,line_offset,true);
            break;

        case Animation::Pitch:
            {
                Tile trans_where = anim.where;
                if (trans_where.side == Back)
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.column != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_col.x,
                                top_left.y+i*delta_col.y,
                                top_left.z+i*delta_col.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, true
                        );
                    if (i != 0)
                        DrawLine3D
                        (
                            {
                                top_left.x+i*delta_col.x+line_offset.x,
                                top_left.y+i*delta_col.y+line_offset.y,
                                top_left.z+i*delta_col.z+line_offset.z
                            },
                            {
                                top_left.x+i*delta_col.x+SIDE_LENGTH*delta_row.x+line_offset.x,
                                top_left.y+i*delta_col.y+SIDE_LENGTH*delta_row.y+line_offset.y,
                                top_left.z+i*delta_col.z+SIDE_LENGTH*delta_row.z+line_offset.z
                            },
                            LINE_COLOR
                        );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.column*delta_col.x,
                        rot_top_left.y+trans_where.column*delta_col.y,
                        rot_top_left.z+trans_where.column*delta_col.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.column, true
                );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column)*delta_col.x,
                            top_left.y+(trans_where.column)*delta_col.y,
                            top_left.z+(trans_where.column)*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.column)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column+1)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column+1)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x,
                            top_left.y+(trans_where.column+1)*delta_col.y,
                            top_left.z+(trans_where.column+1)*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                break;
            }

        case Animation::Roll:
            {
                Tile trans_where = anim.where;
                switch (trans_where.side)
                {
                default:
                    break;

                case Bottom:
                    trans_where.row = SIDE_LENGTH-trans_where.row-1;
                    break;

                case Left:
                    trans_where.row = trans_where.column;
                    break;

                case Right:
                    trans_where.row = SIDE_LENGTH-trans_where.column-1;
                    break;
                }
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.row != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_row.x,
                                top_left.y+i*delta_row.y,
                                top_left.z+i*delta_row.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, false
                        );
                    if (i != 0)
                    DrawLine3D
                    (
                        {
                            top_left.x+i*delta_row.x+line_offset.x,
                            top_left.y+i*delta_row.y+line_offset.y,
                            top_left.z+i*delta_row.z+line_offset.z
                        },
                        {
                            top_left.x+i*delta_row.x+SIDE_LENGTH*delta_col.x+line_offset.x,
                            top_left.y+i*delta_row.y+SIDE_LENGTH*delta_col.y+line_offset.y,
                            top_left.z+i*delta_row.z+SIDE_LENGTH*delta_col.z+line_offset.z
                        },
                        LINE_COLOR
                    );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.row*delta_row.x,
                        rot_top_left.y+trans_where.row*delta_row.y,
                        rot_top_left.z+trans_where.row*delta_row.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.row, false
                );
                if (trans_where.row < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.row+1)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                            rot_top_left.y+(trans_where.row+1)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                            rot_top_left.z+(trans_where.row+1)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.row+1)*rot_delta_row.x,
                            rot_top_left.y+(trans_where.row+1)*rot_delta_row.y,
                            rot_top_left.z+(trans_where.row+1)*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row+1)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row+1)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row+1)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.row > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.row)*delta_row.x+SIDE_LENGTH*delta_col.x,
                            top_left.y+(trans_where.row)*delta_row.y+SIDE_LENGTH*delta_col.y,
                            top_left.z+(trans_where.row)*delta_row.z+SIDE_LENGTH*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.row)*delta_row.x,
                            top_left.y+(trans_where.row)*delta_row.y,
                            top_left.z+(trans_where.row)*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.row > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.row)*rot_delta_row.x,
                            rot_top_left.y+(trans_where.row)*rot_delta_row.y,
                            rot_top_left.z+(trans_where.row)*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.row)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                            rot_top_left.y+(trans_where.row)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                            rot_top_left.z+(trans_where.row)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.row < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.row+1)*delta_row.x,
                            top_left.y+(trans_where.row+1)*delta_row.y,
                            top_left.z+(trans_where.row+1)*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.row+1)*delta_row.x+SIDE_LENGTH*delta_col.x,
                            top_left.y+(trans_where.row+1)*delta_row.y+SIDE_LENGTH*delta_col.y,
                            top_left.z+(trans_where.row+1)*delta_row.z+SIDE_LENGTH*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row+1)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row+1)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row+1)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                break;
            }
        }
    }
    
    // Draw Bottom
    if (true)
    {
        Sides side = Bottom;
        Vector3 top_left = {-CUBE_SIZE/2,-CUBE_SIZE/2,CUBE_SIZE/2};
        Vector3 delta_row = {0,0,-TILE_SIZE};
        Vector3 delta_col = {TILE_SIZE,0,0};
        Vector3 line_offset = {0, -THICK, 0};
        Vector3 rot_top_left = Transform3D(anim,top_left);
        Vector3 rot_delta_row = Transform3D(anim,delta_row);
        Vector3 rot_delta_col = Transform3D(anim,delta_col);
        switch (anim.type)
        {
        case Animation::Yaw:
            if (anim.where.row == SIDE_LENGTH-1)
            {
                DrawRubikSide3D(cube[side],rot_top_left,rot_delta_row,rot_delta_col,line_offset,true);
                break;
            }
        default:
        case Animation::None:
            DrawRubikSide3D(cube[side],top_left,delta_row,delta_col,line_offset,true);
            break;

        case Animation::Pitch:
            {
                Tile trans_where = anim.where;
                if (trans_where.side == Back)
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.column != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_col.x,
                                top_left.y+i*delta_col.y,
                                top_left.z+i*delta_col.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, true
                        );
                    if (i != 0)
                        DrawLine3D
                        (
                            {
                                top_left.x+i*delta_col.x+line_offset.x,
                                top_left.y+i*delta_col.y+line_offset.y,
                                top_left.z+i*delta_col.z+line_offset.z
                            },
                            {
                                top_left.x+i*delta_col.x+SIDE_LENGTH*delta_row.x+line_offset.x,
                                top_left.y+i*delta_col.y+SIDE_LENGTH*delta_row.y+line_offset.y,
                                top_left.z+i*delta_col.z+SIDE_LENGTH*delta_row.z+line_offset.z
                            },
                            LINE_COLOR
                        );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.column*delta_col.x,
                        rot_top_left.y+trans_where.column*delta_col.y,
                        rot_top_left.z+trans_where.column*delta_col.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.column, true
                );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column)*delta_col.x,
                            top_left.y+(trans_where.column)*delta_col.y,
                            top_left.z+(trans_where.column)*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.column)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column+1)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column+1)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x,
                            top_left.y+(trans_where.column+1)*delta_col.y,
                            top_left.z+(trans_where.column+1)*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                break;
            }

        case Animation::Roll:
            {
                Tile trans_where = anim.where;
                switch (trans_where.side)
                {
                default:
                    break;

                case Top:
                    trans_where.row = SIDE_LENGTH-trans_where.row-1;
                    break;

                case Left:
                    trans_where.row = SIDE_LENGTH-trans_where.column-1;
                    break;

                case Right:
                    trans_where.row = trans_where.column;
                    break;
                }
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.row != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_row.x,
                                top_left.y+i*delta_row.y,
                                top_left.z+i*delta_row.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, false
                        );
                    if (i != 0)
                    DrawLine3D
                    (
                        {
                            top_left.x+i*delta_row.x+line_offset.x,
                            top_left.y+i*delta_row.y+line_offset.y,
                            top_left.z+i*delta_row.z+line_offset.z
                        },
                        {
                            top_left.x+i*delta_row.x+SIDE_LENGTH*delta_col.x+line_offset.x,
                            top_left.y+i*delta_row.y+SIDE_LENGTH*delta_col.y+line_offset.y,
                            top_left.z+i*delta_row.z+SIDE_LENGTH*delta_col.z+line_offset.z
                        },
                        LINE_COLOR
                    );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.row*delta_row.x,
                        rot_top_left.y+trans_where.row*delta_row.y,
                        rot_top_left.z+trans_where.row*delta_row.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.row, false
                );
                if (trans_where.row > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.row)*rot_delta_row.x,
                            rot_top_left.y+(trans_where.row)*rot_delta_row.y,
                            rot_top_left.z+(trans_where.row)*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.row)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                            rot_top_left.y+(trans_where.row)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                            rot_top_left.z+(trans_where.row)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.row < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.row+1)*delta_row.x,
                            top_left.y+(trans_where.row+1)*delta_row.y,
                            top_left.z+(trans_where.row+1)*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.row+1)*delta_row.x+SIDE_LENGTH*delta_col.x,
                            top_left.y+(trans_where.row+1)*delta_row.y+SIDE_LENGTH*delta_col.y,
                            top_left.z+(trans_where.row+1)*delta_row.z+SIDE_LENGTH*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row+1)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row+1)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row+1)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.row < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.row+1)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                            rot_top_left.y+(trans_where.row+1)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                            rot_top_left.z+(trans_where.row+1)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.row+1)*rot_delta_row.x,
                            rot_top_left.y+(trans_where.row+1)*rot_delta_row.y,
                            rot_top_left.z+(trans_where.row+1)*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row+1)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row+1)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row+1)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.row > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.row)*delta_row.x+SIDE_LENGTH*delta_col.x,
                            top_left.y+(trans_where.row)*delta_row.y+SIDE_LENGTH*delta_col.y,
                            top_left.z+(trans_where.row)*delta_row.z+SIDE_LENGTH*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.row)*delta_row.x,
                            top_left.y+(trans_where.row)*delta_row.y,
                            top_left.z+(trans_where.row)*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_row.x/2+(trans_where.row)*delta_row.x,
                            -SIDE_LENGTH*delta_row.y/2+(trans_where.row)*delta_row.y,
                            -SIDE_LENGTH*delta_row.z/2+(trans_where.row)*delta_row.z
                        },
                        SIDE_COLOR
                    );
                break;
            }
        }
    }
    
    // Draw Right
    if (true)
    {
        Sides side = Right;
        Vector3 top_left = {CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2};
        Vector3 delta_row = {0,-TILE_SIZE,0};
        Vector3 delta_col = {0,0,-TILE_SIZE};
        Vector3 line_offset = {THICK, 0, 0};
        Vector3 rot_top_left = Transform3D(anim,top_left);
        Vector3 rot_delta_row = Transform3D(anim,delta_row);
        Vector3 rot_delta_col = Transform3D(anim,delta_col);
        switch (anim.type)
        {
        case Animation::Pitch:
            if ((anim.where.column == SIDE_LENGTH-1 && anim.where.side != Back)
                || (anim.where.column == 0 && anim.where.side == Back))
            {
                DrawRubikSide3D(cube[side],rot_top_left,rot_delta_row,rot_delta_col,line_offset,true);
                break;
            }
        default:
        case Animation::None:
            DrawRubikSide3D(cube[side],top_left,delta_row,delta_col,line_offset,true);
            break;

        case Animation::Yaw:
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                if (anim.where.row != i)
                    DrawRubikLine3D
                    (
                        cube[side],
                        {
                            top_left.x+i*delta_row.x,
                            top_left.y+i*delta_row.y,
                            top_left.z+i*delta_row.z
                        },
                        delta_row, delta_col,
                        line_offset,
                        i, false
                    );
                if (i != 0)
                DrawLine3D
                (
                    {
                        top_left.x+i*delta_row.x+line_offset.x,
                        top_left.y+i*delta_row.y+line_offset.y,
                        top_left.z+i*delta_row.z+line_offset.z
                    },
                    {
                        top_left.x+i*delta_row.x+SIDE_LENGTH*delta_col.x+line_offset.x,
                        top_left.y+i*delta_row.y+SIDE_LENGTH*delta_col.y+line_offset.y,
                        top_left.z+i*delta_row.z+SIDE_LENGTH*delta_col.z+line_offset.z
                    },
                    LINE_COLOR
                );
            }
            DrawRubikLine3D
            (
                cube[side],
                {
                    rot_top_left.x+anim.where.row*delta_row.x,
                    rot_top_left.y+anim.where.row*delta_row.y,
                    rot_top_left.z+anim.where.row*delta_row.z
                },
                rot_delta_row, rot_delta_col,
                line_offset,
                anim.where.row, false
            );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z
                    },
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x,
                        top_left.y+(anim.where.row+1)*delta_row.y,
                        top_left.z+(anim.where.row+1)*delta_row.z
                    },
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row+1)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row+1)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        top_left.x+(anim.where.row)*delta_row.x,
                        top_left.y+(anim.where.row)*delta_row.y,
                        top_left.z+(anim.where.row)*delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            break;

        case Animation::Roll:
            {
                Tile trans_where = anim.where;
                switch (trans_where.side)
                {
                default:
                    break;

                case Bottom:
                    trans_where.column = trans_where.row;
                    break;

                case Left:
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                    break;

                case Top:
                    trans_where.column = SIDE_LENGTH-trans_where.row-1;
                    break;
                }
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.column != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_col.x,
                                top_left.y+i*delta_col.y,
                                top_left.z+i*delta_col.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, true
                        );
                    if (i != 0)
                        DrawLine3D
                        (
                            {
                                top_left.x+i*delta_col.x+line_offset.x,
                                top_left.y+i*delta_col.y+line_offset.y,
                                top_left.z+i*delta_col.z+line_offset.z
                            },
                            {
                                top_left.x+i*delta_col.x+SIDE_LENGTH*delta_row.x+line_offset.x,
                                top_left.y+i*delta_col.y+SIDE_LENGTH*delta_row.y+line_offset.y,
                                top_left.z+i*delta_col.z+SIDE_LENGTH*delta_row.z+line_offset.z
                            },
                            LINE_COLOR
                        );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.column*delta_col.x,
                        rot_top_left.y+trans_where.column*delta_col.y,
                        rot_top_left.z+trans_where.column*delta_col.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.column, true
                );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column+1)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column+1)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x,
                            top_left.y+(trans_where.column+1)*delta_col.y,
                            top_left.z+(trans_where.column+1)*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column)*delta_col.x,
                            top_left.y+(trans_where.column)*delta_col.y,
                            top_left.z+(trans_where.column)*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.column)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                break;
            }
        }
    }
    
    // Draw Left
    if (true)
    {
        Sides side = Left;
        Vector3 top_left = {-CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2};
        Vector3 delta_row = {0,-TILE_SIZE,0};
        Vector3 delta_col = {0,0,TILE_SIZE};
        Vector3 line_offset = {-THICK, 0, 0};
        Vector3 rot_top_left = Transform3D(anim,top_left);
        Vector3 rot_delta_row = Transform3D(anim,delta_row);
        Vector3 rot_delta_col = Transform3D(anim,delta_col);
        switch (anim.type)
        {
        case Animation::Pitch:
            if ((anim.where.column == 0 && anim.where.side != Back)
                || (anim.where.column == SIDE_LENGTH-1 && anim.where.side == Back))
            {
                DrawRubikSide3D(cube[side],rot_top_left,rot_delta_row,rot_delta_col,line_offset,true);
                break;
            }
        default:
        case Animation::None:
            DrawRubikSide3D(cube[side],top_left,delta_row,delta_col,line_offset,true);
            break;

        case Animation::Yaw:
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                if (anim.where.row != i)
                    DrawRubikLine3D
                    (
                        cube[side],
                        {
                            top_left.x+i*delta_row.x,
                            top_left.y+i*delta_row.y,
                            top_left.z+i*delta_row.z
                        },
                        delta_row, delta_col,
                        line_offset,
                        i, false
                    );
                if (i != 0)
                DrawLine3D
                (
                    {
                        top_left.x+i*delta_row.x+line_offset.x,
                        top_left.y+i*delta_row.y+line_offset.y,
                        top_left.z+i*delta_row.z+line_offset.z
                    },
                    {
                        top_left.x+i*delta_row.x+SIDE_LENGTH*delta_col.x+line_offset.x,
                        top_left.y+i*delta_row.y+SIDE_LENGTH*delta_col.y+line_offset.y,
                        top_left.z+i*delta_row.z+SIDE_LENGTH*delta_col.z+line_offset.z
                    },
                    LINE_COLOR
                );
            }
            DrawRubikLine3D
            (
                cube[side],
                {
                    rot_top_left.x+anim.where.row*delta_row.x,
                    rot_top_left.y+anim.where.row*delta_row.y,
                    rot_top_left.z+anim.where.row*delta_row.z
                },
                rot_delta_row, rot_delta_col,
                line_offset,
                anim.where.row, false
            );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z
                    },
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x,
                        top_left.y+(anim.where.row+1)*delta_row.y,
                        top_left.z+(anim.where.row+1)*delta_row.z
                    },
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row+1)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row+1)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        top_left.x+(anim.where.row)*delta_row.x,
                        top_left.y+(anim.where.row)*delta_row.y,
                        top_left.z+(anim.where.row)*delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            break;

        case Animation::Roll:
            {
                Tile trans_where = anim.where;
                switch (trans_where.side)
                {
                default:
                    break;

                case Bottom:
                    trans_where.column = SIDE_LENGTH-trans_where.row-1;
                    break;

                case Right:
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                    break;

                case Top:
                    trans_where.column = trans_where.row;
                    break;
                }
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.column != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_col.x,
                                top_left.y+i*delta_col.y,
                                top_left.z+i*delta_col.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, true
                        );
                    if (i != 0)
                        DrawLine3D
                        (
                            {
                                top_left.x+i*delta_col.x+line_offset.x,
                                top_left.y+i*delta_col.y+line_offset.y,
                                top_left.z+i*delta_col.z+line_offset.z
                            },
                            {
                                top_left.x+i*delta_col.x+SIDE_LENGTH*delta_row.x+line_offset.x,
                                top_left.y+i*delta_col.y+SIDE_LENGTH*delta_row.y+line_offset.y,
                                top_left.z+i*delta_col.z+SIDE_LENGTH*delta_row.z+line_offset.z
                            },
                            LINE_COLOR
                        );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.column*delta_col.x,
                        rot_top_left.y+trans_where.column*delta_col.y,
                        rot_top_left.z+trans_where.column*delta_col.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.column, true
                );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column)*delta_col.x,
                            top_left.y+(trans_where.column)*delta_col.y,
                            top_left.z+(trans_where.column)*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.column)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column+1)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column+1)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x,
                            top_left.y+(trans_where.column+1)*delta_col.y,
                            top_left.z+(trans_where.column+1)*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                break;
            }
        }
    }

    // Draw Front
    if (true)
    {
        Sides side = Front;
        Vector3 top_left = {-CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2};
        Vector3 delta_row = {0,-TILE_SIZE,0};
        Vector3 delta_col = {TILE_SIZE,0,0};
        Vector3 line_offset = {0, 0, THICK};
        Vector3 rot_top_left = Transform3D(anim,top_left);
        Vector3 rot_delta_row = Transform3D(anim,delta_row);
        Vector3 rot_delta_col = Transform3D(anim,delta_col);
        switch (anim.type)
        {
        case Animation::Roll:
            {
                Tile trans_where = anim.where;
                switch (trans_where.side)
                {
                default:
                    break;

                case Bottom:
                    trans_where.column = trans_where.row;
                    break;

                case Left:
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                    break;

                case Top:
                    trans_where.column = SIDE_LENGTH-trans_where.row-1;
                    break;
                }
                if (trans_where.column == 0)
                {
                    DrawRubikSide3D(cube[side],rot_top_left,rot_delta_row,rot_delta_col,line_offset,true);
                    break;
                }
            }
        default:
        case Animation::None:
            DrawRubikSide3D(cube[side],top_left,delta_row,delta_col,line_offset,true);
            break;

        case Animation::Yaw:
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                if (anim.where.row != i)
                    DrawRubikLine3D
                    (
                        cube[side],
                        {
                            top_left.x+i*delta_row.x,
                            top_left.y+i*delta_row.y,
                            top_left.z+i*delta_row.z
                        },
                        delta_row, delta_col,
                        line_offset,
                        i, false
                    );
                if (i != 0)
                DrawLine3D
                (
                    {
                        top_left.x+i*delta_row.x+line_offset.x,
                        top_left.y+i*delta_row.y+line_offset.y,
                        top_left.z+i*delta_row.z+line_offset.z
                    },
                    {
                        top_left.x+i*delta_row.x+SIDE_LENGTH*delta_col.x+line_offset.x,
                        top_left.y+i*delta_row.y+SIDE_LENGTH*delta_col.y+line_offset.y,
                        top_left.z+i*delta_row.z+SIDE_LENGTH*delta_col.z+line_offset.z
                    },
                    LINE_COLOR
                );
            }
            DrawRubikLine3D
            (
                cube[side],
                {
                    rot_top_left.x+anim.where.row*delta_row.x,
                    rot_top_left.y+anim.where.row*delta_row.y,
                    rot_top_left.z+anim.where.row*delta_row.z
                },
                rot_delta_row, rot_delta_col,
                line_offset,
                anim.where.row, false
            );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z
                    },
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x,
                        top_left.y+(anim.where.row+1)*delta_row.y,
                        top_left.z+(anim.where.row+1)*delta_row.z
                    },
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row+1)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row+1)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        top_left.x+(anim.where.row)*delta_row.x,
                        top_left.y+(anim.where.row)*delta_row.y,
                        top_left.z+(anim.where.row)*delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            break;

        case Animation::Pitch:
            {
                Tile trans_where = anim.where;
                if (trans_where.side == Back)
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.column != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_col.x,
                                top_left.y+i*delta_col.y,
                                top_left.z+i*delta_col.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, true
                        );
                    if (i != 0)
                        DrawLine3D
                        (
                            {
                                top_left.x+i*delta_col.x+line_offset.x,
                                top_left.y+i*delta_col.y+line_offset.y,
                                top_left.z+i*delta_col.z+line_offset.z
                            },
                            {
                                top_left.x+i*delta_col.x+SIDE_LENGTH*delta_row.x+line_offset.x,
                                top_left.y+i*delta_col.y+SIDE_LENGTH*delta_row.y+line_offset.y,
                                top_left.z+i*delta_col.z+SIDE_LENGTH*delta_row.z+line_offset.z
                            },
                            LINE_COLOR
                        );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.column*delta_col.x,
                        rot_top_left.y+trans_where.column*delta_col.y,
                        rot_top_left.z+trans_where.column*delta_col.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.column, true
                );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column)*delta_col.x,
                            top_left.y+(trans_where.column)*delta_col.y,
                            top_left.z+(trans_where.column)*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.column)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column+1)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column+1)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x,
                            top_left.y+(trans_where.column+1)*delta_col.y,
                            top_left.z+(trans_where.column+1)*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                break;
            }
        }
    }

    // Draw Back
    if (true)
    {
        Sides side = Back;
        Vector3 top_left = {CUBE_SIZE/2,CUBE_SIZE/2,-CUBE_SIZE/2};
        Vector3 delta_row = {0,-TILE_SIZE,0};
        Vector3 delta_col = {-TILE_SIZE,0,0};
        Vector3 line_offset = {0, 0, -THICK};
        Vector3 rot_top_left = Transform3D(anim,top_left);
        Vector3 rot_delta_row = Transform3D(anim,delta_row);
        Vector3 rot_delta_col = Transform3D(anim,delta_col);
        switch (anim.type)
        {
        case Animation::Roll:
            {
                Tile trans_where = anim.where;
                switch (trans_where.side)
                {
                default:
                    break;

                case Bottom:
                    trans_where.column = trans_where.row;
                    break;

                case Left:
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                    break;

                case Top:
                    trans_where.column = SIDE_LENGTH-trans_where.row-1;
                    break;
                }
                if (trans_where.column == SIDE_LENGTH-1)
                {
                    DrawRubikSide3D(cube[side],rot_top_left,rot_delta_row,rot_delta_col,line_offset,true);
                    break;
                }
            }
        default:
        case Animation::None:
            DrawRubikSide3D(cube[side],top_left,delta_row,delta_col,line_offset,true);
            break;

        case Animation::Yaw:
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                if (anim.where.row != i)
                    DrawRubikLine3D
                    (
                        cube[side],
                        {
                            top_left.x+i*delta_row.x,
                            top_left.y+i*delta_row.y,
                            top_left.z+i*delta_row.z
                        },
                        delta_row, delta_col,
                        line_offset,
                        i, false
                    );
                if (i != 0)
                DrawLine3D
                (
                    {
                        top_left.x+i*delta_row.x+line_offset.x,
                        top_left.y+i*delta_row.y+line_offset.y,
                        top_left.z+i*delta_row.z+line_offset.z
                    },
                    {
                        top_left.x+i*delta_row.x+SIDE_LENGTH*delta_col.x+line_offset.x,
                        top_left.y+i*delta_row.y+SIDE_LENGTH*delta_col.y+line_offset.y,
                        top_left.z+i*delta_row.z+SIDE_LENGTH*delta_col.z+line_offset.z
                    },
                    LINE_COLOR
                );
            }
            DrawRubikLine3D
            (
                cube[side],
                {
                    rot_top_left.x+anim.where.row*delta_row.x,
                    rot_top_left.y+anim.where.row*delta_row.y,
                    rot_top_left.z+anim.where.row*delta_row.z
                },
                rot_delta_row, rot_delta_col,
                line_offset,
                anim.where.row, false
            );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z
                    },
                    {
                        rot_top_left.x+(anim.where.row)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x,
                        top_left.y+(anim.where.row+1)*delta_row.y,
                        top_left.z+(anim.where.row+1)*delta_row.z
                    },
                    {
                        top_left.x+(anim.where.row+1)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row+1)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row+1)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row < SIDE_LENGTH-1)
                DrawTriangle3D
                (
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x+SIDE_LENGTH*rot_delta_col.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y+SIDE_LENGTH*rot_delta_col.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z+SIDE_LENGTH*rot_delta_col.z
                    },
                    {
                        rot_top_left.x+(anim.where.row+1)*rot_delta_row.x,
                        rot_top_left.y+(anim.where.row+1)*rot_delta_row.y,
                        rot_top_left.z+(anim.where.row+1)*rot_delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row+1)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row+1)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row+1)*delta_row.z
                    },
                    SIDE_COLOR
                );
            if (anim.where.row > 0)
                DrawTriangle3D
                (
                    {
                        top_left.x+(anim.where.row)*delta_row.x+SIDE_LENGTH*delta_col.x,
                        top_left.y+(anim.where.row)*delta_row.y+SIDE_LENGTH*delta_col.y,
                        top_left.z+(anim.where.row)*delta_row.z+SIDE_LENGTH*delta_col.z
                    },
                    {
                        top_left.x+(anim.where.row)*delta_row.x,
                        top_left.y+(anim.where.row)*delta_row.y,
                        top_left.z+(anim.where.row)*delta_row.z
                    },
                    {
                        -SIDE_LENGTH*delta_row.x/2+(anim.where.row)*delta_row.x,
                        -SIDE_LENGTH*delta_row.y/2+(anim.where.row)*delta_row.y,
                        -SIDE_LENGTH*delta_row.z/2+(anim.where.row)*delta_row.z
                    },
                    SIDE_COLOR
                );
            break;

        case Animation::Pitch:
            {
                Tile trans_where = anim.where;
                if (trans_where.side != Back)
                    trans_where.column = SIDE_LENGTH-trans_where.column-1;
                for (unsigned i=0; i<SIDE_LENGTH; ++i)
                {
                    if (trans_where.column != i)
                        DrawRubikLine3D
                        (
                            cube[side],
                            {
                                top_left.x+i*delta_col.x,
                                top_left.y+i*delta_col.y,
                                top_left.z+i*delta_col.z
                            },
                            delta_row, delta_col,
                            line_offset,
                            i, true
                        );
                    if (i != 0)
                        DrawLine3D
                        (
                            {
                                top_left.x+i*delta_col.x+line_offset.x,
                                top_left.y+i*delta_col.y+line_offset.y,
                                top_left.z+i*delta_col.z+line_offset.z
                            },
                            {
                                top_left.x+i*delta_col.x+SIDE_LENGTH*delta_row.x+line_offset.x,
                                top_left.y+i*delta_col.y+SIDE_LENGTH*delta_row.y+line_offset.y,
                                top_left.z+i*delta_col.z+SIDE_LENGTH*delta_row.z+line_offset.z
                            },
                            LINE_COLOR
                        );
                }
                DrawRubikLine3D
                (
                    cube[side],
                    {
                        rot_top_left.x+trans_where.column*delta_col.x,
                        rot_top_left.y+trans_where.column*delta_col.y,
                        rot_top_left.z+trans_where.column*delta_col.z
                    },
                    rot_delta_row, rot_delta_col,
                    line_offset,
                    trans_where.column, true
                );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            rot_top_left.x+(trans_where.column)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column)*rot_delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column+1)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column+1)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            top_left.x+(trans_where.column+1)*delta_col.x,
                            top_left.y+(trans_where.column+1)*delta_col.y,
                            top_left.z+(trans_where.column+1)*delta_col.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column < SIDE_LENGTH-1)
                    DrawTriangle3D
                    (
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z
                        },
                        {
                            rot_top_left.x+(trans_where.column+1)*rot_delta_col.x+SIDE_LENGTH*rot_delta_row.x,
                            rot_top_left.y+(trans_where.column+1)*rot_delta_col.y+SIDE_LENGTH*rot_delta_row.y,
                            rot_top_left.z+(trans_where.column+1)*rot_delta_col.z+SIDE_LENGTH*rot_delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column+1)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column+1)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column+1)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                if (trans_where.column > 0)
                    DrawTriangle3D
                    (
                        {
                            top_left.x+(trans_where.column)*delta_col.x,
                            top_left.y+(trans_where.column)*delta_col.y,
                            top_left.z+(trans_where.column)*delta_col.z
                        },
                        {
                            top_left.x+(trans_where.column)*delta_col.x+SIDE_LENGTH*delta_row.x,
                            top_left.y+(trans_where.column)*delta_col.y+SIDE_LENGTH*delta_row.y,
                            top_left.z+(trans_where.column)*delta_col.z+SIDE_LENGTH*delta_row.z
                        },
                        {
                            -SIDE_LENGTH*delta_col.x/2+(trans_where.column)*delta_col.x,
                            -SIDE_LENGTH*delta_col.y/2+(trans_where.column)*delta_col.y,
                            -SIDE_LENGTH*delta_col.z/2+(trans_where.column)*delta_col.z
                        },
                        SIDE_COLOR
                    );
                break;
            }
        }
    }
}

#pragma GCC diagnostic pop

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

Tile FindMouse3D(const CameraProperties& camera)
{
    Ray ray = GetMouseRay(GetMousePosition(),camera.cam);
    RayCollision collision =
        GetRayCollisionBox(ray,{{-CUBE_SIZE/2,-CUBE_SIZE/2,-CUBE_SIZE/2},{CUBE_SIZE/2,CUBE_SIZE/2,CUBE_SIZE/2}});
    if (!collision.hit)
        return Tile::invalid;

    if (camera.rad.x > VIS_THRESHOLD)
    {
        if (camera.cam.position.x > 0)
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
    }

    if (camera.rad.y > VIS_THRESHOLD)
    {
        if (camera.cam.position.y > 0)
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
    }

    if (camera.rad.z > VIS_THRESHOLD)
    {
        if (camera.cam.position.z > 0)
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
    }

    // Shouldn't happen
    return Tile::invalid;
}

void MoveCamera(CameraProperties& camera, Vector2 angle)
{
    camera.deg.x += angle.x;
    camera.deg.y += angle.y;

    if (camera.deg.x > 180)
        camera.deg.x -= 360;
    if (camera.deg.x < -180)
        camera.deg.x += 360;
    if (camera.deg.y > 89)
        camera.deg.y = 89;
    if (camera.deg.y < -89)
        camera.deg.y = -89;
    
    camera.cam.position =
    {
        sinf(DEG2RAD*camera.deg.x) * cosf(DEG2RAD*camera.deg.y) * CAMERA_DISTANCE,
        sinf(DEG2RAD*camera.deg.y) * CAMERA_DISTANCE,
        cosf(DEG2RAD*camera.deg.x) * cosf(DEG2RAD*camera.deg.y) * CAMERA_DISTANCE
    };

    camera.rad =
    {
        fabsf(asinf(camera.cam.position.x/CAMERA_DISTANCE)),
        fabsf(asinf(camera.cam.position.y/CAMERA_DISTANCE)),
        fabsf(asinf(camera.cam.position.z/CAMERA_DISTANCE))
    };
}

bool ProgressAnimation(Cube& cube, Animation& anim, float angle)
{
    anim.angle += angle;

    if (anim.angle > 45)
    {
        anim.angle -= 90;
        switch (anim.type)
        {
        default:
        case Animation::None:
            break;

        case Animation::Pitch:
            if (anim.where.side == Back)
                MovePitch(cube,anim.where,false);
            else
                MovePitch(cube,anim.where,true);
            break;
            
        case Animation::Yaw:
            MoveYaw(cube,anim.where,false);
            break;

        case Animation::Roll:
            if (anim.where.side == Top)
                MoveYaw(cube,anim.where,false);
            else if (anim.where.side == Bottom)
                MoveYaw(cube,anim.where,true);
            else if (anim.where.side == Left)
                MovePitch(cube,anim.where,true);
            else
                MovePitch(cube,anim.where,false);
            break;
        }
        return true;
    }
    if (anim.angle <-45)
    {
        anim.angle += 90;
        switch (anim.type)
        {
        case Animation::Pitch:
            if (anim.where.side == Back)
                MovePitch(cube,anim.where,true);
            else
                MovePitch(cube,anim.where,false);
            break;
            
        case Animation::Yaw:
            MoveYaw(cube,anim.where,true);
            break;

        case Animation::Roll:
            if (anim.where.side == Top)
                MoveYaw(cube,anim.where,true);
            else if (anim.where.side == Bottom)
                MoveYaw(cube,anim.where,false);
            else if (anim.where.side == Left)
                MovePitch(cube,anim.where,false);
            else
                MovePitch(cube,anim.where,true);
            break;

        default:
        case Animation::None:
            break;
        }
        return true;
    }
    return false;
}

void RAYLIB_IO(Cube& cube, Tile& mouse)
{
    char buffer[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bool enable_keybinds = true;
    bool anim_manual = false;

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WIND_WIDTH, WIND_HEIGHT, "Rubik!");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    CameraProperties camera;
    camera.cam.projection = CAMERA_PERSPECTIVE;
    camera.cam.target = {0, 0, 0};
    camera.cam.up = {0, 1, 0};
    camera.cam.fovy = 60;
    camera.deg = {45, 30};
    MoveCamera(camera,{0,0});

    std::queue<Animation> moves;
    Animation animation;

    SetCameraMode(camera.cam,CAMERA_CUSTOM);

    while (!WindowShouldClose())
    {
        mouse = FindMouse3D(camera);

        if (!anim_manual)
        {
            if (moves.empty() || (animation.where != Tile::invalid &&
                (moves.front().where != animation.where || moves.front().type != animation.type)))
            {
                if (animation.type != Animation::None)
                {
                    if (animation.angle > ANIM_AUTO_SPEED)
                        animation.angle -= ANIM_AUTO_SPEED;
                    else if (animation.angle < -ANIM_AUTO_SPEED)
                        animation.angle += ANIM_AUTO_SPEED;
                    else
                    {
                        animation.type = Animation::None;
                        animation.where = Tile::invalid;
                        animation.angle = 0;
                    }
                }
            }
            else
            {
                if (animation.type == Animation::None)
                {
                    animation = moves.front();
                    animation.angle = 0;
                }
                if (ProgressAnimation(cube,animation,moves.front().angle*ANIM_AUTO_SPEED))
                    moves.pop();
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && animation.type == Animation::None) //grab cube
        {
            enable_keybinds = false;
            anim_manual = true;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && anim_manual) //rotate part of cube
        {
            switch (animation.type)
            {
            default:
            case Animation::None:
                break;
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) //release cube
        {
            enable_keybinds = true;
            anim_manual = false;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) //grab camera
        {
            enable_keybinds = false;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) //move camera
        {
            auto move = GetMouseDelta();
            MoveCamera(camera,{-move.x*CAMERA_MOUSE_SPEED,move.y*CAMERA_MOUSE_SPEED});
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) //release camera
        {
            enable_keybinds = true;
        }

        if (enable_keybinds && moves.size() < 4)
        {
            if (IsKeyPressed(KEY_Q)) // rotate counterclockwise
            {
                switch (mouse.side)
                {
                case Top:
                    moves.push(Animation(Animation::Yaw,Tile(Front,0,mouse.column),1));
                    break;

                case Bottom:
                    moves.push(Animation(Animation::Yaw,Tile(Front,2,mouse.column),-1));
                    break;

                case Left:
                    moves.push(Animation(Animation::Pitch,Tile(Front,mouse.row,0),1));
                    break;

                case Right:
                    moves.push(Animation(Animation::Pitch,Tile(Front,mouse.row,2),-1));
                    break;

                case Front:
                    moves.push(Animation(Animation::Roll,Tile(Top,2,mouse.column),-1));
                    break;

                case Back:
                    moves.push(Animation(Animation::Roll,Tile(Top,0,SIDE_LENGTH-mouse.column-1),1));
                    break;

                default:
                    break;
                }
            }

            if (IsKeyPressed(KEY_W)) // pitch up
            {
                switch (mouse.side)
                {
                case Top:
                case Bottom:
                    if (camera.deg.x > -45 && camera.deg.x < 45)
                        moves.push(Animation(Animation::Pitch,mouse,1));
                    else if (camera.deg.x > 45 && camera.deg.x < 135)
                        moves.push(Animation(Animation::Roll,mouse,-1));
                    else if (camera.deg.x > 135 || camera.deg.x < -135)
                        moves.push(Animation(Animation::Pitch,mouse,-1));
                    else if (camera.deg.x > -135 && camera.deg.x < -45)
                        moves.push(Animation(Animation::Roll,mouse,1));
                    break;

                case Left:
                    moves.push(Animation(Animation::Roll,mouse,1));
                    break;

                case Right:
                    moves.push(Animation(Animation::Roll,mouse,-1));
                    break;

                case Front:
                    moves.push(Animation(Animation::Pitch,mouse,1));
                    break;

                case Back:
                    moves.push(Animation(Animation::Pitch,mouse,-1));
                    break;

                default:
                    break;
                }
            }

            if (IsKeyPressed(KEY_E)) // rotate clockwise
            {
                switch (mouse.side)
                {
                case Top:
                    moves.push(Animation(Animation::Yaw,Tile(Front,0,mouse.column),-1));
                    break;

                case Bottom:
                    moves.push(Animation(Animation::Yaw,Tile(Front,2,mouse.column),1));
                    break;

                case Left:
                    moves.push(Animation(Animation::Pitch,Tile(Front,mouse.row,0),-1));
                    break;

                case Right:
                    moves.push(Animation(Animation::Pitch,Tile(Front,mouse.row,2),1));
                    break;

                case Front:
                    moves.push(Animation(Animation::Roll,Tile(Top,2,mouse.column),1));
                    break;

                case Back:
                    moves.push(Animation(Animation::Roll,Tile(Top,0,SIDE_LENGTH-mouse.column-1),-1));
                    break;

                default:
                    break;
                }
            }

            if (IsKeyPressed(KEY_A)) // yaw left
            {
                switch (mouse.side)
                {
                case Top:
                    if (camera.deg.x > -45 && camera.deg.x < 45)
                        moves.push(Animation(Animation::Roll,mouse,-1));
                    else if (camera.deg.x > 45 && camera.deg.x < 135)
                        moves.push(Animation(Animation::Pitch,mouse,-1));
                    else if (camera.deg.x > 135 || camera.deg.x < -135)
                        moves.push(Animation(Animation::Roll,mouse,1));
                    else if (camera.deg.x > -135 && camera.deg.x < -45)
                        moves.push(Animation(Animation::Pitch,mouse,1));
                    break;

                case Bottom:
                    if (camera.deg.x > -45 && camera.deg.x < 45)
                        moves.push(Animation(Animation::Roll,mouse,1));
                    else if (camera.deg.x > 45 && camera.deg.x < 135)
                        moves.push(Animation(Animation::Pitch,mouse,1));
                    else if (camera.deg.x > 135 || camera.deg.x < -135)
                        moves.push(Animation(Animation::Roll,mouse,-1));
                    else if (camera.deg.x > -135 && camera.deg.x < -45)
                        moves.push(Animation(Animation::Pitch,mouse,-1));
                    break;

                case Left:
                case Right:
                case Front:
                case Back:
                    moves.push(Animation(Animation::Yaw,mouse,-1));
                    break;

                default:
                    break;
                }
            }

            if (IsKeyPressed(KEY_S)) // pitch down
            {
                switch (mouse.side)
                {
                case Top:
                case Bottom:
                    if (camera.deg.x > -45 && camera.deg.x < 45)
                        moves.push(Animation(Animation::Pitch,mouse,-1));
                    else if (camera.deg.x > 45 && camera.deg.x < 135)
                        moves.push(Animation(Animation::Roll,mouse,1));
                    else if (camera.deg.x > 135 || camera.deg.x < -135)
                        moves.push(Animation(Animation::Pitch,mouse,1));
                    else if (camera.deg.x > -135 && camera.deg.x < -45)
                        moves.push(Animation(Animation::Roll,mouse,-1));
                    break;

                case Left:
                    moves.push(Animation(Animation::Roll,mouse,-1));
                    break;

                case Right:
                    moves.push(Animation(Animation::Roll,mouse,1));
                    break;

                case Front:
                    moves.push(Animation(Animation::Pitch,mouse,-1));
                    break;

                case Back:
                    moves.push(Animation(Animation::Pitch,mouse,1));
                    break;

                default:
                    break;
                }
            }

            if (IsKeyPressed(KEY_D)) // yaw right
            {
                switch (mouse.side)
                {
                case Top:
                    if (camera.deg.x > -45 && camera.deg.x < 45)
                        moves.push(Animation(Animation::Roll,mouse,1));
                    else if (camera.deg.x > 45 && camera.deg.x < 135)
                        moves.push(Animation(Animation::Pitch,mouse,1));
                    else if (camera.deg.x > 135 || camera.deg.x < -135)
                        moves.push(Animation(Animation::Roll,mouse,-1));
                    else if (camera.deg.x > -135 && camera.deg.x < -45)
                        moves.push(Animation(Animation::Pitch,mouse,-1));
                    break;

                case Bottom:
                    if (camera.deg.x > -45 && camera.deg.x < 45)
                        moves.push(Animation(Animation::Roll,mouse,-1));
                    else if (camera.deg.x > 45 && camera.deg.x < 135)
                        moves.push(Animation(Animation::Pitch,mouse,-1));
                    else if (camera.deg.x > 135 || camera.deg.x < -135)
                        moves.push(Animation(Animation::Roll,mouse,1));
                    else if (camera.deg.x > -135 && camera.deg.x < -45)
                        moves.push(Animation(Animation::Pitch,mouse,1));
                    break;

                case Left:
                case Right:
                case Front:
                case Back:
                    moves.push(Animation(Animation::Yaw,mouse,1));
                    break;

                default:
                    break;
                }
            }

            if (IsKeyPressed(KEY_SPACE)) // shuffle
            {
                Shuffle(cube);
            }

            if (IsKeyDown(KEY_I)) // camera up
            {
                MoveCamera(camera,{0,CAMERA_KEYBOARD_SPEED});
            }

            if (IsKeyDown(KEY_J)) // camera left
            {
                MoveCamera(camera,{-CAMERA_KEYBOARD_SPEED,0});
            }

            if (IsKeyDown(KEY_K)) // camera down
            {
                MoveCamera(camera,{0,-CAMERA_KEYBOARD_SPEED});
            }

            if (IsKeyDown(KEY_L)) // camera right
            {
                MoveCamera(camera,{CAMERA_KEYBOARD_SPEED,0});
            }
        }

        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode3D(camera.cam);

                DrawRubikCube3D(cube,camera,animation);

            EndMode3D();

            DrawText("Press Space to shuffle!", (WIND_WIDTH-420)/2, WIND_HEIGHT-48, 32, LIGHTGRAY);
            DrawText(itoa((int)(camera.deg.x),buffer,10),WIND_WIDTH-144,16,32,LIGHTGRAY);
            DrawText(itoa((int)(camera.deg.y),buffer,10),WIND_WIDTH-64,16,32,LIGHTGRAY);
            DrawText(itoa(mouse.side,buffer,10),16,16,32,LIGHTGRAY);
            DrawText(itoa(mouse.row,buffer,10),48,16,32,LIGHTGRAY);
            DrawText(itoa(mouse.column,buffer,10),80,16,32,LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
}

#endif // USE_SFML
