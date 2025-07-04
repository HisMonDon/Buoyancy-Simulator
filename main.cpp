//Made by Chenyu Lu
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

sf::Font arial("arial.ttf");
int Random(int n1, int n2, int scale = 1) {
    return n1+(rand() % (n2 - n1 + 1) * scale) / scale;
}
float ybelow(float bally, float waterline) {
    if (bally - waterline >= 0) {
        return bally-waterline;
    }
    return 0.0;
}

double round_up(double value) {
    return("%2f", value);
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
class waterParticle {
    public:
    float lifetime = 0.f;
    float size;
    //status
    bool alive = true;
    float time = 0.f;
    sf::Vector2f position;
    sf::Vector2f velocity;
    void Update(float const timechange, float gravity = 4.f, float resistance = 0.f) {
        time += timechange;
        if (time >= lifetime) {
            alive = false;
        }
        position += velocity * timechange;
        velocity.y += gravity * timechange * 3;
        velocity -= velocity * resistance * timechange;
        cout<<"accessed"<<endl;
    }
    void Draw(sf::RenderWindow& window, sf::CircleShape circle, float waterline) {
        if (position.y > waterline) {
            return;
        }
        float radius = size/2 * (1 - time / lifetime);
        circle.setRadius (radius);
        circle.setOrigin(circle.getGeometricCenter());
        circle.setPosition(position);
        circle.setFillColor(sf::Color(29, 111, 163));
        window.draw(circle);
    }
};
int main() {
    //shader
    sf::Shader shader;
    sf::Shader ballshader;
    if (!ballshader.loadFromFile("/Users/ericlu/CLionProjects/BuoyancySimulator/shaders/ball.frag", sf::Shader::Type::Fragment)) {
        return 1;
    }
    if (!shader.loadFromFile("/Users/ericlu/CLionProjects/BuoyancySimulator/shaders/shader.frag", sf::Shader::Type::Fragment)) {
        return 1;
    }
    //Variables
    unsigned int fps = 60;
    unsigned int width = 1400;
    unsigned int height = 800;
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
    int counter = 0;
    bool isMousePressed = false;
    sf::Vector2i offset;
    int particle_amount = 5;
    float size_particle = 4.f;
    float max_lifetime_particle = 6;
    float particle_resistance = 0.1f * fps - 3;
    bool dragging = false;
    sf::CircleShape circle;
    std::vector<waterParticle> particles;

    ///////////////////////////////////////////////////////////////
    sf::RenderWindow window(sf::VideoMode({width, height}), "Buoyancy simulator");
    window.setFramerateLimit(fps);
    sf::CircleShape ball(ballradius);

    ball.setFillColor(sf::Color::Red);
    ball.setPosition({ballx, bally});
    //Draw shapes
    sf::RectangleShape increaseDensity({float(40), float(40)});
    sf::RectangleShape decreaseDensity({float(40), float(40)});
    sf::RectangleShape shadedBall({80.f, 80.f});
    shadedBall.setPosition({float(ballx), float(bally)});
    sf::RectangleShape water({float(1500), float(600)});
    water.setFillColor(sf::Color(76, 98, 228, 150));
    water.setPosition({0.0, float(waterline)});

    increaseDensity.setFillColor(sf::Color::Cyan);
    decreaseDensity.setFillColor(sf::Color::Cyan);
    increaseDensity.setPosition({float(width) - 70, 10});
    decreaseDensity.setPosition({float(width) - 500, 10});
    //////////////////////

    sf::Text text(arial, to_string(getareaofcircle(ballradius, ybelow(bally, waterline))));
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);
    text.setPosition({10, 10});
    sf::Text constantText(arial, ("Fluid Density: 1.0 g/mL"));
    constantText.setStyle(sf::Text::Bold);
    constantText.setFillColor(sf::Color::Cyan);
    constantText.setPosition({float(width) - 435, 10});
    //////////////////////////////////////////////////////////////////
    sf::Clock clock;
    clock.restart();
    sf::Clock shaderclock;
    shaderclock.restart();
    while (window.isOpen()) {
        //particle_amount = int(ballvelocityy)*10;
        //shader
        shader.setUniform("iTime", shaderclock.getElapsedTime().asSeconds());
        shader.setUniform("iResolution", sf::Vector3f(
            window.getSize().x,
            window.getSize().y,
            window.getSize().x / (float)window.getSize().y
        ));
        ballshader.setUniform("iTime", shaderclock.getElapsedTime().asSeconds());
        ballshader.setUniform("iResolution", sf::Vector3f(
            window.getSize().x,
            window.getSize().y,
            window.getSize().x / (float)window.getSize().y
        ));
        float timechange = clock.restart().asSeconds();

        counter ++;
        netForce = buoyancyForce + forceGravity;
        ballvelocityy += netForce/10;


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
            for (int x = 0; x < particle_amount; x++) {
                float lifetime = Random(0, max_lifetime_particle, 100);
                float velocity = Random(0, 600);
                float angle = Random(180, 360);
                waterParticle particle;
                particle.size = size_particle;
                particle.lifetime = lifetime;
                particle.position = sf:: Vector2f(ballx, float(waterline));
                particle.velocity = sf:: Vector2f(velocity, sf::degrees(angle));
                particles.push_back(particle);
            }
            submergedArea = (ballarea - (ballarea - integratecircle(ballradius, waterline - bally)))/100;
            text.setString("Submerged area: " + to_string(submergedArea) +
                           "\nBall Y: " + to_string(height - int(bally)) +
                           "\nBuoyant Force: " + to_string(buoyancyForce)+ " N" +
                           "\nNet Force: "+ to_string(int(netForce)) +" N");

        }

        buoyancyForce = abs(densitywater*submergedArea*gravity);
        //MAIN GAME CONTROL (Mouse button pressed, buttons, e.t.c)
        /////////////////////////////////////////////////////////////////////////////////////////////

        if (auto event = window.pollEvent()) {
            if (event-> is<sf::Event::Closed>()) {
                window.close();
            }
            //std::cout<<x<<endl;
            //x++;
        }
        sf::Vector2i mouseposition = sf::Mouse::getPosition(window);
        float mouse_x = sf::Mouse::getPosition(window).x;
        float mouse_y = sf::Mouse::getPosition(window).y;
        float dincreasex1 = increaseDensity.getPosition().x;
        float dincreasey1 = increaseDensity.getPosition().y;
        float dincreasex2 = increaseDensity.getSize().x;
        float dincreasey2 = increaseDensity.getSize().x;
        float ddecreasex1 = decreaseDensity.getPosition().x;
        float ddecreasey1 = decreaseDensity.getPosition().y;
        float ddecreasex2 = decreaseDensity.getSize().x;
        float ddecreasey2 = decreaseDensity.getSize().x;
        //increase
        ////
        if ((mouse_x >= dincreasex1) and (mouse_x <= dincreasex1 + dincreasex2) and
            (mouse_y >= dincreasey1) and (mouse_y <= dincreasey2 + dincreasey1)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                {
                    if (not isMousePressed) {
                        densitywater += 0.1;
                        increaseDensity.setFillColor(sf::Color(252, 4, 4));
                        netForce = 0;
                        isMousePressed = true;
                    } else {
                        isMousePressed = true;
                    }
                }

            } else {
                increaseDensity.setFillColor(sf::Color(252, 240, 4));
                isMousePressed = false;
            }
        } else {
            increaseDensity.setFillColor(sf::Color::Cyan);
            isMousePressed = false;
        }
        //Decrease
        if ((mouse_x >= ddecreasex1) and (mouse_x <= ddecreasex1 + ddecreasex2) and
            (mouse_y >= ddecreasey1) and (mouse_y <= ddecreasey2 + ddecreasey1)) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                    if (not isMousePressed) {
                        densitywater -= 0.1;
                        decreaseDensity.setFillColor(sf::Color(252, 4, 4));
                        netForce = 0;
                        isMousePressed = true;
                    } else {
                        isMousePressed = true;
                    }
            } else {
                decreaseDensity.setFillColor(sf::Color(252, 240, 4));
                isMousePressed = false;
            }
        } else {
            decreaseDensity.setFillColor(sf::Color::Cyan);
            isMousePressed = false;
        }

        // Print mouse position
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            cout<<"X position: " <<mouse_x<< ", Y Position: "<<mouse_y<<endl;
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mouse_x < ballx + ballradius && mouse_x > ballx - ballradius
            && mouse_y < ballx + ballradius && mouse_y > bally - ballradius) {
            ballx = mouse_x;
            bally = mouse_y;
            window.setMouseCursorVisible(false);
            ball.setPosition({mouse_x, mouse_y});
            ball.setFillColor(sf::Color::Yellow);
        } else {
            ball.setFillColor(sf::Color::Red);
            window.setMouseCursorVisible(true);
        }
        /*else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
            cout<<"not clicked"<<endl;
        }*/
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            bally += 4;
            ballvelocityy = 0;
            counter = 0;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            bally -=4;
            ballvelocityy = 0;
            counter = 0;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            //cout<<getareaofcircle(ballradius, ybelow(bally, waterline))<<endl;
            ballvelocityy = 0;
        } else{bally -= ballvelocityy/1000;}
        if (ballvelocityy > 0) {
            ballvelocityy -= friction;
        } else {
            ballvelocityy += friction;
        }

        std::ostringstream densityStream;
        densityStream << std::fixed << std::setprecision(2) << densitywater;
        constantText.setString("Fluid Density: " + densityStream.str() + " g/mL");
        /////////////////////////////////////////////////////////////////////////////////////////////
        //physical and game barriers to prevent breaking the laws of physics
        if (densitywater < 0.1) {
            densitywater = 0.1;
        } else if (densitywater > 5.0) {
            densitywater = 5.0;
        }
        if (bally + 2*ballradius > height) {
            bally = height - 2*ballradius;
            cout<<"dam"<<endl;
        }
        for (auto particle = particles.begin();  particle != particles.end();)
        {
            particle->Update(timechange, 200, particle_resistance);
            if (not particle->alive)
            {
                // remove particle
                particles.erase(particle);
            }

            else
            {
                // iterate to next particle
                particle++;
            }
        }
        //Drawings
        float time = shaderclock.getElapsedTime().asSeconds();
        shader.setUniform("iTime", time);
        ballshader.setUniform("iTime", time);
        window.clear();
        window.draw(ball);
        for (waterParticle particle : particles) {
            particle.Draw(window, circle, waterline);
        }
        //window.draw(water);
        window.draw(water, &shader);
        ball.setPosition({ballx - ballradius, bally});
        window.draw(ball, &ballshader);
        window.draw(text);
        window.draw(constantText);
        window.draw(increaseDensity);
        window.draw(decreaseDensity);

        window.display();

        /////////////////////////////////////////////////////////////////////////////////////////////
    }
}
