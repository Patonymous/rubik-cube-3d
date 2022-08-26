#include "SFML_version.hpp"

static const std::array<sf::Color,ColorCount> RGB = {sf::Color::White, sf::Color::Magenta, sf::Color::Green, sf::Color::Red, sf::Color::Blue, sf::Color::Yellow};

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

void SFML_IO(Cube& cube, Tile& mouse)
{
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
}
