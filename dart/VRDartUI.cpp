#include <SFML/Graphics.hpp>
#include <iostream>
#include <pthread.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void drawTarget(int percent, int width, int height, int x, int y);

pthread_mutex_t netMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t netThread;

int hX;
int hY;

int initNetworking(char* address, int port)
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
        std::cout << hX << std::endl;
        std::cout << hY << std::endl;
        pthread_mutex_unlock( &netMutex ); 
    }
}

int main(int argc, char** argv)
{
  int s = initNetworking(argv[1], 56465);
  int net1 = pthread_create(&netThread, NULL, cv_comm, (void*)&s);
  
  drawTarget(20, 200, 200, 80, 90);
  
  pthread_join(net1, NULL);
  close(s);
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
}
