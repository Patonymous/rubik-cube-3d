#include "main.hpp"
const Tile Tile::invalid(SideCount, SIDE_LENGTH, SIDE_LENGTH);

#include <iostream>

#include "SFML_version.hpp"
#include "RAYLIB_version.hpp"

using namespace std;

inline void Rotate(Cube& cube, Sides side, bool clockwise)
{
    if (clockwise)
    {
        auto temp = cube[side][0][0];
        cube[side][0][0] = cube[side][2][0];
        cube[side][2][0] = cube[side][2][2];
        cube[side][2][2] = cube[side][0][2];
        cube[side][0][2] = temp;
        temp = cube[side][1][0];
        cube[side][1][0] = cube[side][2][1];
        cube[side][2][1] = cube[side][1][2];
        cube[side][1][2] = cube[side][0][1];
        cube[side][0][1] = temp;
    }
    else
    {
        auto temp = cube[side][0][0];
        cube[side][0][0] = cube[side][0][2];
        cube[side][0][2] = cube[side][2][2];
        cube[side][2][2] = cube[side][2][0];
        cube[side][2][0] = temp;
        temp = cube[side][0][1];
        cube[side][0][1] = cube[side][1][2];
        cube[side][1][2] = cube[side][2][1];
        cube[side][2][1] = cube[side][1][0];
        cube[side][1][0] = temp;
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="

void MovePitch(Cube& cube, Tile where, bool up)
{
    switch (where.side)
    {
    case Back:
        where.column = SIDE_LENGTH-where.column-1;
        up = !up;
    case Top:
    case Front:
    case Bottom:
        if (up)
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Top][i][where.column];
                cube[Top][i][where.column] = cube[Front][i][where.column];
                cube[Front][i][where.column] = cube[Bottom][i][where.column];
                cube[Bottom][i][where.column] = cube[Back][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1];
                cube[Back][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1] = temp;
            }
            if (where.column == 0)
                Rotate(cube,Left,false);
            if (where.column == SIDE_LENGTH-1)
                Rotate(cube,Right,true);
        }
        else
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Top][i][where.column];
                cube[Top][i][where.column] = cube[Back][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1];
                cube[Back][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1] = cube[Bottom][i][where.column];
                cube[Bottom][i][where.column] = cube[Front][i][where.column];
                cube[Front][i][where.column] = temp;
            }
            if (where.column == 0)
                Rotate(cube,Left,true);
            if (where.column == SIDE_LENGTH-1)
                Rotate(cube,Right,false);
        }
        break;

    case Right:
        where.column = SIDE_LENGTH-where.column-1;
        up = !up;
    case Left:
        if (up)
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Top][where.column][SIDE_LENGTH-i-1];
                cube[Top][where.column][SIDE_LENGTH-i-1] = cube[Left][i][where.column];
                cube[Left][i][where.column] = cube[Bottom][SIDE_LENGTH-where.column-1][i];
                cube[Bottom][SIDE_LENGTH-where.column-1][i] = cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1];
                cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1] = temp;
            }
            if (where.column == 0)
                Rotate(cube,Back,false);
            if (where.column == SIDE_LENGTH-1)
                Rotate(cube,Front,true);
        }
        else
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Top][where.column][SIDE_LENGTH-i-1];
                cube[Top][where.column][SIDE_LENGTH-i-1] = cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1];
                cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.column-1] = cube[Bottom][SIDE_LENGTH-where.column-1][i];
                cube[Bottom][SIDE_LENGTH-where.column-1][i] = cube[Left][i][where.column];
                cube[Left][i][where.column] = temp;
            }
            if (where.column == 0)
                Rotate(cube,Back,true);
            if (where.column == SIDE_LENGTH-1)
                Rotate(cube,Front,false);
        }
        break;

    default:
        break;
    }
}

void MoveYaw(Cube& cube, Tile where, bool left)
{
    switch (where.side)
    {
    case Left:
    case Front:
    case Right:
    case Back:
        if (left)
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Left][where.row][i];
                cube[Left][where.row][i] = cube[Front][where.row][i];
                cube[Front][where.row][i] = cube[Right][where.row][i];
                cube[Right][where.row][i] = cube[Back][where.row][i];
                cube[Back][where.row][i] = temp;
            }
            if (where.row == 0)
                Rotate(cube,Top,true);
            if (where.row == SIDE_LENGTH-1)
                Rotate(cube,Bottom,false);
        }
        else
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Left][where.row][i];
                cube[Left][where.row][i] = cube[Back][where.row][i];
                cube[Back][where.row][i] = cube[Right][where.row][i];
                cube[Right][where.row][i] = cube[Front][where.row][i];
                cube[Front][where.row][i] = temp;
            }
            if (where.row == 0)
                Rotate(cube,Top,false);
            if (where.row == SIDE_LENGTH-1)
                Rotate(cube,Bottom,true);
        }
        break;

    case Bottom:
        where.row = SIDE_LENGTH-where.row-1;
        left = !left;
    case Top:
        if (left)
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Top][where.row][SIDE_LENGTH-i-1];
                cube[Top][where.row][SIDE_LENGTH-i-1] = cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.row-1];
                cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.row-1] = cube[Bottom][SIDE_LENGTH-where.row-1][i];
                cube[Bottom][SIDE_LENGTH-where.row-1][i] = cube[Left][i][where.row];
                cube[Left][i][where.row] = temp;
            }
            if (where.row == 0)
                Rotate(cube,Back,true);
            if (where.row == SIDE_LENGTH-1)
                Rotate(cube,Front,false);
        }
        else
        {
            for (unsigned i=0; i<SIDE_LENGTH; ++i)
            {
                auto temp = cube[Top][where.row][SIDE_LENGTH-i-1];
                cube[Top][where.row][SIDE_LENGTH-i-1] = cube[Left][i][where.row];
                cube[Left][i][where.row] = cube[Bottom][SIDE_LENGTH-where.row-1][i];
                cube[Bottom][SIDE_LENGTH-where.row-1][i] = cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.row-1];
                cube[Right][SIDE_LENGTH-i-1][SIDE_LENGTH-where.row-1] = temp;
            }
            if (where.row == 0)
                Rotate(cube,Back,false);
            if (where.row == SIDE_LENGTH-1)
                Rotate(cube,Front,true);
        }
        break;

    default:
        break;
    }
}

#pragma GCC diagnostic pop

void MoveRoll(Cube& cube, Tile where, bool clockwise)
{
    switch (where.side)
    {
    case Top:
        where.side = Front;
        where.row = 0;
        where.column = 0;
        MoveYaw(cube, where, clockwise);
        break;

    case Bottom:
        where.side = Front;
        where.row = SIDE_LENGTH-1;
        where.column = 0;
        MoveYaw(cube, where, !clockwise);
        break;

    case Left:
        where.side = Top;
        where.row = 0;
        where.column = 0;
        MovePitch(cube, where, !clockwise);
        break;

    case Right:
        where.side = Top;
        where.row = 0;
        where.column = SIDE_LENGTH-1;
        MovePitch(cube, where, clockwise);
        break;

    case Front:
        where.side = Left;
        where.row = 0;
        where.column = SIDE_LENGTH-1;
        MovePitch(cube, where, clockwise);
        break;

    case Back:
        where.side = Left;
        where.row = 0;
        where.column = 0;
        MovePitch(cube, where, !clockwise);
        break;

    default:
        break;
    }
}

void Shuffle(Cube& cube)
{
    for (unsigned i=0; i<100; ++i)
    {
        const unsigned move = rand()%3;
        const unsigned position = rand()%3;
        const bool direction = rand()%2;
        switch (move)
        {
        case 0: //Pitch
            MovePitch(cube, Tile(Top,0,position), direction);
            break;
        
        case 1: //Yaw
            MoveYaw(cube, Tile(Top,position,0), direction);
            break;

        case 2: //Roll
            MoveYaw(cube, Tile(Front,position,0), direction);
            break;

        default:
            break;
        }
    }
}

int main()
{
    srand(time(0));

    Cube cube;
    for (unsigned i=0; i<SIDE_LENGTH; ++i)
    {
        for (unsigned j=0; j<SIDE_LENGTH; ++j)
        {
            cube[Top][i][j] = White;
            cube[Left][i][j] = Magenta;
            cube[Front][i][j] = Green;
            cube[Right][i][j] = Red;
            cube[Back][i][j] = Blue;
            cube[Bottom][i][j] = Yellow;
        }
    }
    Tile mouse = Tile::invalid;

    RAYLIB_IO(cube, mouse);

    return EXIT_SUCCESS;
}
