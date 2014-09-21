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
                window.display();
    }
    drawTarget(20, 200, 200, 80, 90);
}

//value for percent is what percentage of the diameter is taken up by the outermost circle
//height and width in pixels
//x and y are in pixels
void drawTarget(int percent, int width, int height, int x, int y) {


    
}
