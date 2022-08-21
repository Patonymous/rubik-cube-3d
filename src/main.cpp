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

void DrawFlatCube(sf::RenderTarget& rt, const Cube& cube, float x, float y, float width, float height)
{
    const float side_width = width/4;
    const float side_heigth = height/3;
    DrawFlatSide(rt, cube[Top], x+side_width, y, side_width, side_heigth);
    DrawFlatSide(rt, cube[Left], x, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Front], x+side_width, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Right], x+2*side_width, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Back], x+3*side_width, y+side_width, side_width, side_heigth);
    DrawFlatSide(rt, cube[Bottom], x+side_width, y+2*side_width, side_width, side_heigth);
}

int main()
{
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

    sf::RenderWindow window(sf::VideoMode(WIND_WIDTH, WIND_HEIGHT), "Rubik!");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        DrawFlatCube(window, cube, 0, 0, WIND_WIDTH, WIND_HEIGHT);
        window.display();
    }

    return EXIT_SUCCESS;
}
