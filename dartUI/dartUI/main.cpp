#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

sf::Font font;
int sizex = 1280, sizey = 720;
int grid = sizey/10;

pthread_mutex_t netMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t netThread;

int hX;
int hY;
char newData = 0;

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
        std::cout << sqrt(pow(x - pos.x + radius, 2) + pow(y - pos.y + radius, 2)) << std::endl;
        return sqrt(pow(x - pos.x, 2) + pow(y - pos.y, 2)) < radius;
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
    sf::CircleShape ref1, ref2;
    sf::Text text;
    int score;
public:
    GameScreen(int sizex, int sizey){
        score = 0;
        
        ref1 = sf::CircleShape(0.5*grid);
        ref1.setPosition(0.25*grid, 0.25*grid);
        ref1.setFillColor(sf::Color::Black);
        
        ref2 = sf::CircleShape(0.5*grid);
        ref2.setPosition(sizex - 0.75*grid, sizey - 0.75*grid);
        ref2.setFillColor(sf::Color::Black);
        
        text = sf::Text("0", font, grid * .5);
        text.setColor(sf::Color::Red);
        text.setPosition(sizex-1.5*grid, grid*.25);
        
        // Load a sprite to display
        sf::Texture texture;
        if (!texture.loadFromFile("./cute_image.jpg")) {
            std::cout<< "Coud not open cute_image.jpg";
        }
        sf::Sprite sprite(texture);
        
        targets.push_back(Target(-grid*4, grid*2.5, grid, 1, 0, 1));
        targets.push_back(Target(-grid*4, grid*5.5, grid, 1, 0, 1));
        targets.push_back(Target(-grid*4, grid*8.5, grid, 1, 0, 1));
    }
    
    void hit(int x, int y){
        targets.push_back(Target(x, y, 30, 0, 0, 1));
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
        window.draw(ref1);
        window.draw(ref2);
        for (Target target: targets){
            window.draw(target);
        }
        window.draw(text);
    }
};

int initNetworking(const char* address, int port)
{
    int sockfd;
    struct sockaddr_in serverAddr;

    sockfd=socket(AF_INET,SOCK_STREAM,0);

    bzero(&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr=inet_addr(address);
    serverAddr.sin_port=htons(port);

    connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    return sockfd;
}

void *cv_comm(void *ptr)
{
    int buf[2];
    int s = *((int*)ptr);
    while(1)
    {
        recv(s,buf, sizeof(buf), 0);
        pthread_mutex_lock( &netMutex );
        hX = ntohl(buf[0]);
        hY = ntohl(buf[1]);
        newData = 1;
        pthread_mutex_unlock( &netMutex );
    }
}

int main(int argc, char const** argv)
{

    int s = initNetworking(argv[1], 56465);
    int net1 = pthread_create(&netThread, NULL, cv_comm, (void*)&s);
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(sizex, sizey), "Darts!");
    window.setVerticalSyncEnabled(true);

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("./icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load Font
    if (!font.loadFromFile("./sansation.ttf")) {
        return EXIT_FAILURE;
    }

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile("./nice_music.ogg")) {
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
        
        if(newData)
        {
            game.hit(grid * 2.5 + grid * 0.5 + hX, grid * 2.5 + grid * 0.5 + hY);
            newData = 0;
        }
        game.update();
        
        game.draw(window);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
