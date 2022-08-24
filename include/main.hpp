#ifndef MAIN_HPP_INCLUDED
#define MAIN_HPP_INCLUDED

#include <array>

#define WIND_WIDTH 800
#define WIND_HEIGHT 600
#define SIDE_LENGTH 3

enum Sides {Top, Left, Front, Right, Back, Bottom, SideCount};
enum Colors {White, Magenta, Green, Red, Blue, Yellow, ColorCount};
using Side = std::array<std::array<Colors,SIDE_LENGTH>,SIDE_LENGTH>;
using Cube = std::array<Side,SideCount>;
struct Tile
{
    Sides side;
    unsigned row;
    unsigned column;

    Tile(Sides s, unsigned r, unsigned c):side(s),row(r),column(c){}
    // bool operator==(const Tile& other)const{return side==other.side && row==other.row && column==other.column;}
    // bool operator!=(const Tile& other)const{return !(*this==other);}

    const static Tile invalid;
};

void MovePitch(Cube& cube, Tile where, bool up);
void MoveYaw(Cube& cube, Tile where, bool left);
void MoveRoll(Cube& cube, Tile where, bool clockwise);
void Shuffle(Cube& cube);

#endif // MAIN_HPP_INCLUDED
