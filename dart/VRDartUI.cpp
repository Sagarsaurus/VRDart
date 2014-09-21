#include <SFML/Graphics.hpp>
#include <iostream>

void drawTarget(int percent, int width, int height, int x, int y);

int main() {
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.draw(outerMost);
        window.draw(firstInner);
        window.draw(secondInner);
        window.draw(almostMiddle);
        window.draw(center);
        window.display();
    }
    drawTarget(20, 200, 200, 80, 90);
}

//value for percent is what percentage of the diameter is taken up by the outermost circle
//height and width in pixels
//x and y are in pixels
void drawTarget(int percent, int width, int height, int x, int y) {
    sf::RenderWindow window(sf::VideoMode(width, height), "VRDarts");
    sf::Vector2u v = window.getSize();
    sf::CircleShape outerMost(percent);
    sf::CircleShape firstInner(.8*percent);
    sf::CircleShape secondInner(.6*percent);
    sf::CircleShape almostMiddle(.4*percent);
    sf::CircleShape center(.2*percent);
    outerMost.setFillColor(sf::Color::Magenta);
    firstInner.setFillColor(sf::Color::White);
    secondInner.setFillColor(sf::Color::Magenta);
    almostMiddle.setFillColor(sf::Color::White);
    center.setFillColor(sf::Color::Magenta);
    outerMost.setPosition(x/2-outerMost.getRadius(), y/2-outerMost.getRadius());
    firstInner.setPosition(x/2-firstInner.getRadius(), y/2-firstInner.getRadius());
    secondInner.setPosition(x/2-secondInner.getRadius(), y/2-secondInner.getRadius());
    almostMiddle.setPosition(x/2-almostMiddle.getRadius(), y/2-almostMiddle.getRadius());
    center.setPosition(x/2-center.getRadius(), y/2-center.getRadius());
    

    
}
