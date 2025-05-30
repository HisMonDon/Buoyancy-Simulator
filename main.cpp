#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
//#include<imgui.h>
//#include <imgui-sfml/imgui-SFML.h>
using namespace std;

sf::Font arial("arial.ttf");
float ybelow(float bally, float waterline) {
    if (bally - waterline >= 0) {
        return bally-waterline;
    }
    return 0.0;
}

float integratecircle(float r, float h) {
    return(pow(r,2)*acos((h-r)/r)+ (r - h)*sqrt(2*h*r - pow(h, 2)));
}
float getareaofcircle (float radius, float ybelow) {
    float x1 = sqrt(pow(radius, 2)- pow(ybelow, 2));
    float x2 = -x1;
    //integrate x2, subtract that from integral of x1;
    // r^2 = y^2 + x^2
    //integrate (y = root(900-x^2)) -> i got -1/3 *(r^2 - x^2)^3/2
    return integratecircle(radius, x2) - integratecircle(radius, x1);
}
int main() {
    //Variables
    unsigned int width = 1400;
    unsigned int height = 960;
    //int x = 0;
    float ballx = width/2 - 70;
    float bally = 200;
    float ballradius = 60;
    float ballmass = 100;
    //water
    int waterline = 500;
    float waterx = 300.f;
    float watery = 0.f;
    float densitywater = 2.0;
    float gravity = -9.81;
    float forceGravity = gravity * ballmass;
    float buoyancyForce = 0;
    float submergedArea;
    float ballarea = M_PI*pow(ballradius, 2);
    float netForce = gravity;
    float ballvelocityy = 0;
    int tickcounter = 0;

    bool dragging = false;
    ///////////////////////////////////////////////////////////////
    sf::RenderWindow window(sf::VideoMode({width, height}), "Buoyancy simulator");

    sf::CircleShape ball(ballradius);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition({ballx, bally});

    sf::RectangleShape water({float(1500), float(600)});
    water.setFillColor(sf::Color::Blue);
    water.setPosition({0.0, float(waterline)});
    sf::Text text(arial, to_string(getareaofcircle(ballradius, ybelow(bally, waterline))));
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);
    text.setPosition({10, 10});

    //////////////////////////////////////////////////////////////////
    while (window.isOpen()) {
        /*        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    ballx = mouse_pos.x - ballradius;
                    bally = mouse_pos.y - ballradius;
                    ball.setPosition({mouse_pos.x - ballradius, mouse_pos.y - ballradius}); */
        tickcounter++;
        if (tickcounter >= 5) {
            ballvelocityy += netForce/50;
            tickcounter = 0;
        }

        netForce = buoyancyForce + forceGravity;
        if (bally + 2*ballradius < waterline) {
            text.setString("Submerged area: " + to_string(0) +
                           "\nBall Y: " + to_string(int(bally)) +
                           "\nBuoyant Force: " + to_string(buoyancyForce)+
                           "\nNet Force: "+ to_string(int(netForce)) + " N");


            submergedArea = 0;

        } else if (bally > waterline) {
            submergedArea = (M_PI*pow(ballradius, 2))/100;
            text.setString("Submerged area: " + to_string(submergedArea) +
                           "\nBall Y: " + to_string(int(bally)) +
                           "\nBuoyant Force: " + to_string(buoyancyForce) +
                           "\nNet Force: "+ to_string(int(netForce)) +" N");


        } else {
            submergedArea = (ballarea - (ballarea - integratecircle(ballradius, waterline - bally)))/100;
            text.setString("Submerged area: " + to_string(submergedArea) +
                           "\nBall Y: " + to_string(int(bally)) +
                           "\nBuoyant Force: " + to_string(buoyancyForce)+
                           "\nNet Force: "+ to_string(int(netForce)) +" N");

        }
        buoyancyForce = abs(densitywater*submergedArea*gravity);

        if (auto event = window.pollEvent()) {
            if (event-> is<sf::Event::Closed>()) {
                window.close();
            }
            //std::cout<<x<<endl;
            //x++;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            bally += 0.5;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            bally -=0.5;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            cout<<getareaofcircle(ballradius, ybelow(bally, waterline))<<endl;
        }
        else{bally -= ballvelocityy/1000;}
        window.clear();
        window.draw(water);
        ball.setPosition({ballx, bally});
        window.draw(ball);
        window.draw(text);
        window.display();
    }
    }



