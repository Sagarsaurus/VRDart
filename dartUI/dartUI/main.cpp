#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

sf::Font font;
int sizex = 1280, sizey = 720;
int grid = sizey/10;

class Target : public sf::Drawable {
    sf::CircleShape outerMost;
    sf::CircleShape firstInner;
    sf::CircleShape secondInner;
    sf::CircleShape almostMiddle;
    sf::CircleShape center;
    sf::Vector2f pos;
    sf::Vector2f speed;
    float radius;
    int score;
    
public:
    Target(float x, float y, float radius, float speedx, float speedy, int score){
        outerMost = sf::CircleShape();
        firstInner = sf::CircleShape();
        secondInner = sf::CircleShape();
        almostMiddle = sf::CircleShape();
        center = sf::CircleShape();
        
        outerMost.setFillColor(sf::Color::Magenta);
        firstInner.setFillColor(sf::Color::White);
        secondInner.setFillColor(sf::Color::Magenta);
        almostMiddle.setFillColor(sf::Color::White);
        center.setFillColor(sf::Color::Magenta);
        
        set(x, y, radius);
        pos = sf::Vector2f(x, y);
        speed = sf::Vector2f(speedx, speedy);
        this->radius = radius;
        this->score = score;
    }
    
    void set(float x, float y, float radius){
        outerMost.setRadius(radius);
        firstInner.setRadius(.8*radius);
        secondInner.setRadius(.6*radius);
        almostMiddle.setRadius(.4*radius);
        center.setRadius(.2*radius);
        
        outerMost.setPosition(x-outerMost.getRadius(), y-outerMost.getRadius());
        firstInner.setPosition(x-firstInner.getRadius(), y-firstInner.getRadius());
        secondInner.setPosition(x-secondInner.getRadius(), y-secondInner.getRadius());
        almostMiddle.setPosition(x-almostMiddle.getRadius(), y-almostMiddle.getRadius());
        center.setPosition(x-center.getRadius(), y-center.getRadius());
    }
    
    void setSpeed(float x, float y){
        speed.x = x;
        speed.y = y;
    }
    
    int getScore(){
        return score;
    }
    
    bool hit(int x, int y){
        return false;
    }
    
    void update(){
        pos += speed;
        set(pos.x, pos.y, radius);
        
        if (pos.x > sizex + grid*3 || pos.x < -grid*4 - abs(speed.x)) {
            speed.x = -speed.x;
        }
    }

    
protected:
    void draw (sf::RenderTarget &target, sf::RenderStates states) const {
        target.draw(outerMost, states);
        target.draw(firstInner, states);
        target.draw(secondInner, states);
        target.draw(almostMiddle, states);
        target.draw(center, states);
    }
};

class GameScreen {
    std::vector<Target> targets;
    sf::CircleShape ref;
    sf::Text text;
    int score;
public:
    GameScreen(int sizex, int sizey){
        score = 0;
        
        ref = sf::CircleShape(0.5*grid);
        ref.setPosition(0.25*grid, 0.25*grid);
        ref.setFillColor(sf::Color::Black);
        
        text = sf::Text("0", font, grid * .5);
        text.setColor(sf::Color::Red);
        text.setPosition(sizex-1.5*grid, grid*.25);
        
        // Load a sprite to display
        sf::Texture texture;
        if (!texture.loadFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/cute_image.jpg")) {
            std::cout<< "Coud not open cute_image.jpg";
        }
        sf::Sprite sprite(texture);
        
        targets.push_back(Target(-grid*4, grid*2.5, grid, 10, 0, 10));
        targets.push_back(Target(-grid*4, grid*5.5, grid, 15, 0, 15));
        targets.push_back(Target(-grid*4, grid*8.5, grid, 5, 0, 5));
    }
    
    void hit(int x, int y){
        for (Target &target: targets){
            if (target.hit(x, y)) {
                score += target.getScore();
            }
        }
    }
    
    void update(){
        for (Target &target: targets){
            target.update();
        }
        text.setString(std::to_string(score));
    }
    
    void draw(sf::RenderWindow &window){
        window.draw(ref);
        for (Target target: targets){
            window.draw(target);
        }
        window.draw(text);
    }
};

int main(int argc, char const** argv)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(sizex, sizey), "Darts!");
    window.setVerticalSyncEnabled(true);

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load Font
    if (!font.loadFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/sansation.ttf")) {
        return EXIT_FAILURE;
    }

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/nice_music.ogg")) {
        return EXIT_FAILURE;
    }
    
    GameScreen game = GameScreen(sizex, sizey);

    // Play the music
//    music.play();

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window : exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed : exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }

        // Clear screen
        window.clear(sf::Color::White);
        
        game.hit(grid*6, grid*6);

        game.update();
        
        game.draw(window);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
