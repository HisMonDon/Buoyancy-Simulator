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
    float friction = 25;
    int tickcounter = 0;
    bool ismoving = true;
    bool ismoving2 = true;
    bool ismoving3 = true;
    bool ismovingoverall = true;
    int counter = 0;


    bool dragging = false;
    ///////////////////////////////////////////////////////////////
    sf::RenderWindow window(sf::VideoMode({width, height}), "Buoyancy simulator");
    window.setFramerateLimit(80);
    sf::CircleShape ball(ballradius);
    ball.setFillColor(sf::Color::Red);
    ball.setPosition({ballx, bally});
    //Draw shapes
    sf::RectangleShape increaseDensity({float(40), float(40)});
    sf::RectangleShape decreaseDensity({float(40), float(40)});
    sf::RectangleShape water({float(1500), float(600)});
    water.setFillColor(sf::Color::Blue);
    water.setPosition({0.0, float(waterline)});
    increaseDensity.setFillColor(sf::Color::Cyan);
    decreaseDensity.setFillColor(sf::Color::Cyan);
    increaseDensity.setPosition({float(width)- 90, 10});
    decreaseDensity.setPosition({float(width) - 530, 10});
    //////////////////////

    sf::Text text(arial, to_string(getareaofcircle(ballradius, ybelow(bally, waterline))));
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);
    text.setPosition({10, 10});
    sf::Text constantText(arial, ("Fluid Density: 1.0 g/mL"));
    constantText.setStyle(sf::Text::Bold);
    constantText.setFillColor(sf::Color::Cyan);
    constantText.setPosition({float(width) - 450, 10});

    //////////////////////////////////////////////////////////////////
    while (window.isOpen()) {
        /*        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    ballx = mouse_pos.x - ballradius;
                    bally = mouse_pos.y - ballradius;
                    ball.setPosition({mouse_pos.x - ballradius, mouse_pos.y - ballradius}); */
        counter ++;
        netForce = buoyancyForce + forceGravity;
        ballvelocityy += netForce/10;
        if (abs(netForce) < 80) {
            netForce = 0;
            buoyancyForce = forceGravity;
            if (ismoving) {
                ismoving = false;
                cout<<"ismoving"<<counter<<endl;
            } else if (not ismoving && ismoving2 && counter == 2) {
                ismoving2 = false;
                cout<<"ismoving2"<<endl;
            } else if (not ismoving2 && ismoving3 && counter <= 3) {
                ismoving3 = false;
                cout<<"ismoving3"<<endl;
            } else if (not ismoving3 && counter >=4) {
                ismovingoverall = false;
                submergedArea = (buoyancyForce/densitywater)/gravity;
                cout <<"not moving"<<endl;
                counter = 20;
            } else {
                counter = 0;
                ismoving = true;
                ismoving2 = true;
                ismoving3 = true;

            }
        }

        if (bally + 2*ballradius < waterline) {
            text.setString("Submerged area: " + to_string(0) +
                           "\nBall Y: " + to_string(height - int(bally)) +
                           "\nBuoyant Force: " + to_string(buoyancyForce)+ " N"
                           "\nNet Force: "+ to_string(int(netForce)) + " N");


            submergedArea = 0;

        } else if (bally > waterline) {
            submergedArea = (M_PI*pow(ballradius, 2))/100;
            text.setString("Submerged area: " + to_string(submergedArea) +
                           "\nBall Y: " + to_string(height - int(bally)) +
                           "\nBuoyant Force: " + to_string(buoyancyForce) + " N"+
                           "\nNet Force: "+ to_string(int(netForce)) +" N");


        } else {
            if (ismovingoverall) {
                submergedArea = (ballarea - (ballarea - integratecircle(ballradius, waterline - bally)))/100;
            }
            text.setString("Submerged area: " + to_string(submergedArea) +
                           "\nBall Y: " + to_string(height - int(bally)) +
                           "\nBuoyant Force: " + to_string(buoyancyForce)+ " N" +
                           "\nNet Force: "+ to_string(int(netForce)) +" N");

        }

        buoyancyForce = abs(densitywater*submergedArea*gravity);
//MAIN GAME CONTROL
/////////////////////////////////////////////////////////////////////////////////////////////
        if (auto event = window.pollEvent()) {
            if (event-> is<sf::Event::Closed>()) {
                window.close();
            }
            //std::cout<<x<<endl;
            //x++;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            bally += 4;
            ballvelocityy = 0;
            ismovingoverall = true;
            counter = 0;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            bally -=4;
            ballvelocityy = 0;
            ismovingoverall = true;
            counter = 0;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            cout<<getareaofcircle(ballradius, ybelow(bally, waterline))<<endl;
            ballvelocityy = 0;
        } else{bally -= ballvelocityy/1000;}
        if (ballvelocityy > 0) {
            ballvelocityy -= friction;
        } else {
            ballvelocityy += friction;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        
        //Drawings
        window.clear();
        window.draw(water);
        ball.setPosition({ballx, bally});
        window.draw(ball);
        window.draw(text);
        window.draw(constantText);
        window.draw(increaseDensity);
        window.draw(decreaseDensity);
        window.display();
        
        /////////////////////////////////////////////////////////////////////////////////////////////
    }
}
