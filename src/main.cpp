#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>

#define WIND_WIDTH 800
#define WIND_HEIGHT 600
#define SIDE_LENGTH 3

using namespace std;

enum Sides {Top, Left, Front, Right, Back, Bottom, SideCount};
enum Colors {White, Magenta, Green, Red, Blue, Yellow, ColorCount};
using Side = array<array<Colors,SIDE_LENGTH>,SIDE_LENGTH>;
using Cube = array<Side,SideCount>;
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
const Tile Tile::invalid(SideCount, SIDE_LENGTH, SIDE_LENGTH);

const array<sf::Color,ColorCount> RGB = {sf::Color::White, sf::Color::Magenta, sf::Color::Green, sf::Color::Red, sf::Color::Blue, sf::Color::Yellow};

void DrawFlatSide(sf::RenderTarget& rt, const Side& side, float x, float y, float width, float height)
{
    sf::RectangleShape square(sf::Vector2f(width/SIDE_LENGTH, height/SIDE_LENGTH));
    square.setOutlineThickness(-1);
    square.setOutlineColor(sf::Color::Black);
    for (unsigned i=0; i<SIDE_LENGTH; ++i)
    {
        for (unsigned j=0; j<SIDE_LENGTH; ++j)
        {
            square.setFillColor(RGB[side[i][j]]);
            square.setPosition(x+j*square.getSize().x, y+i*square.getSize().y);
            rt.draw(square);
        }
    }
}

void DrawFlatCube(sf::RenderTarget& rt, const Cube& cube, float x, float y, float width, float height, Tile mouse)
{
    const float side_width = width/4;
    const float side_heigth = height/3;
    DrawFlatSide(rt, cube[Top], x+side_width, y, side_width, side_heigth);
    DrawFlatSide(rt, cube[Left], x, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Front], x+side_width, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Right], x+2*side_width, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Back], x+3*side_width, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Bottom], x+side_width, y+2*side_width, side_width, side_heigth);
    sf::RectangleShape line_horizontal(sf::Vector2f(side_width,10)), line_vertical(sf::Vector2f(10,side_heigth));
    line_horizontal.setFillColor(sf::Color(128, 128, 128, 128));
    line_vertical.setFillColor(sf::Color(128, 128, 128, 128));
    line_horizontal.setOrigin(0, -side_heigth/6);
    line_vertical.setOrigin(-side_width/6, 0);
    switch (mouse.side)
    {
    case Top:
    case Bottom:
        if (mouse.side == Bottom)
            mouse.row = SIDE_LENGTH-mouse.row-1;
        //Top
        line_horizontal.setPosition(side_width, mouse.row*side_heigth/SIDE_LENGTH);
        line_vertical.setPosition(side_width+mouse.column*side_width/SIDE_LENGTH, 0);
        rt.draw(line_horizontal);
        rt.draw(line_vertical);
        //Left
        line_vertical.setPosition(mouse.row*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_vertical);
        //Front
        line_vertical.setPosition(side_width+mouse.column*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_vertical);
        //Right
        line_vertical.setPosition(2*side_width+(SIDE_LENGTH-mouse.row-1)*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_vertical);
        //Back
        line_vertical.setPosition(3*side_width+(SIDE_LENGTH-mouse.column-1)*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_vertical);
        //Bottom
        line_horizontal.setPosition(side_width, 2*side_heigth+(SIDE_LENGTH-mouse.row-1)*side_heigth/SIDE_LENGTH);
        line_vertical.setPosition(side_width+mouse.column*side_width/SIDE_LENGTH, 2*side_heigth);
        rt.draw(line_horizontal);
        rt.draw(line_vertical);
        break;

    case Front:
    case Back:
        if (mouse.side == Back)
            mouse.column = SIDE_LENGTH-mouse.column-1;
        //Top
        line_vertical.setPosition(side_width+mouse.column*side_width/SIDE_LENGTH, 0);
        rt.draw(line_vertical);
        //Left
        line_horizontal.setPosition(0, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        rt.draw(line_horizontal);
        //Front
        line_horizontal.setPosition(side_width, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        line_vertical.setPosition(side_width+mouse.column*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_horizontal);
        rt.draw(line_vertical);
        //Right
        line_horizontal.setPosition(2*side_width, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        rt.draw(line_horizontal);
        //Back
        line_horizontal.setPosition(3*side_width, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        line_vertical.setPosition(3*side_width+(SIDE_LENGTH-mouse.column-1)*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_horizontal);
        rt.draw(line_vertical);
        //Bottom
        line_vertical.setPosition(side_width+mouse.column*side_width/SIDE_LENGTH, 2*side_heigth);
        rt.draw(line_vertical);
        break;

    case Left:
    case Right:
        if (mouse.side == Right)
            mouse.column = SIDE_LENGTH-mouse.column-1;
        //Top
        line_horizontal.setPosition(side_width, mouse.column*side_heigth/SIDE_LENGTH);
        rt.draw(line_horizontal);
        //Left
        line_horizontal.setPosition(0, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        line_vertical.setPosition(mouse.column*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_horizontal);
        rt.draw(line_vertical);
        //Front
        line_horizontal.setPosition(side_width, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        rt.draw(line_horizontal);
        //Right
        line_horizontal.setPosition(2*side_width, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        line_vertical.setPosition(2*side_width+(SIDE_LENGTH-mouse.column-1)*side_width/SIDE_LENGTH, side_heigth);
        rt.draw(line_horizontal);
        rt.draw(line_vertical);
        //Back
        line_horizontal.setPosition(3*side_width, side_heigth+mouse.row*side_heigth/SIDE_LENGTH);
        rt.draw(line_horizontal);
        //Bottom
        line_horizontal.setPosition(side_width, 2*side_heigth+(SIDE_LENGTH-mouse.column-1)*side_heigth/SIDE_LENGTH);
        rt.draw(line_horizontal);
        break;

    default:
        break;
    }
}

Tile FindFlatMouse(float mouse_x, float mouse_y, float x, float y, float width, float height)
{
    const float relative_x = mouse_x-x;
    const float relative_y = mouse_y-y;
    if (relative_x <= 0 || relative_x >= width || relative_y <= 0 || relative_y >= height)
        return Tile::invalid;

    const float side_width = width/4;
    const float side_heigth = height/3;
    if ((relative_x <= side_width || relative_x >= 2*side_width) && (relative_y <= side_heigth || relative_y >= 2*side_heigth))
        return Tile::invalid;
    
    if (relative_y <= side_heigth)
        return Tile(Top, relative_y/side_heigth*SIDE_LENGTH, (relative_x/side_width-1)*SIDE_LENGTH);
    if (relative_y >= 2*side_heigth)
        return Tile(Bottom, (relative_y/side_heigth-2)*SIDE_LENGTH, (relative_x/side_width-1)*SIDE_LENGTH);

    if (relative_x <= side_width)
        return Tile(Left, (relative_y/side_heigth-1)*SIDE_LENGTH, relative_x/side_width*SIDE_LENGTH);
    if (relative_x <= 2*side_width)
        return Tile(Front, (relative_y/side_heigth-1)*SIDE_LENGTH, (relative_x/side_width-1)*SIDE_LENGTH);
    if (relative_x <= 3*side_width)
        return Tile(Right, (relative_y/side_heigth-1)*SIDE_LENGTH, (relative_x/side_width-2)*SIDE_LENGTH);
    else
        return Tile(Back, (relative_y/side_heigth-1)*SIDE_LENGTH, (relative_x/side_width-3)*SIDE_LENGTH);
}

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

    sf::Font font;
    sf::Text label;
    font.loadFromFile("C:/Windows/Fonts/courbd.ttf");
    label.setFillColor(sf::Color::White);
    label.setCharacterSize(32);
    label.setFont(font);
    label.setString("Press Space\nto shuffle!");

    sf::RenderWindow window(sf::VideoMode(WIND_WIDTH, WIND_HEIGHT), "Rubik!");
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            
            case sf::Event::MouseMoved:
                mouse = FindFlatMouse(event.mouseMove.x, event.mouseMove.y, 0, 0, WIND_WIDTH, WIND_HEIGHT);
                // cout << mouse.side << ": " << mouse.row << ", " << mouse.column << endl;
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Q:
                    MoveRoll(cube, mouse, false);
                    break;

                case sf::Keyboard::W:
                    MovePitch(cube, mouse, true);
                    break;

                case sf::Keyboard::E:
                    MoveRoll(cube, mouse, true);
                    break;
                    
                case sf::Keyboard::A:
                    MoveYaw(cube, mouse, true);
                    break;

                case sf::Keyboard::S:
                    MovePitch(cube, mouse, false);
                    break;

                case sf::Keyboard::D:
                    MoveYaw(cube, mouse, false);
                    break;

                case sf::Keyboard::Space:
                    Shuffle(cube);
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        window.clear();
        label.setPosition(WIND_WIDTH/2+(WIND_WIDTH/2-label.getGlobalBounds().width)/2, (WIND_HEIGHT/3-label.getGlobalBounds().height)/2-label.getLocalBounds().top);
        window.draw(label);
        DrawFlatCube(window, cube, 0, 0, WIND_WIDTH, WIND_HEIGHT, mouse);
        window.display();
    }

    return EXIT_SUCCESS;
}
