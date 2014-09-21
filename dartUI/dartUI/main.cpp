
//
// Disclamer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// In order to load the resources like cute_image.png, you have to set up
// your target scheme :
//
// - Select "Edit Scheme…" in the "Product" menu;
// - Check the box "use custom working directory";
// - Fill the text field with the folder path containing your resources;
//        (e.g. your project folder)
// - Click OK.
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>


class Target : public sf::Drawable {
    sf::CircleShape outerMost;
    sf::CircleShape firstInner;
    sf::CircleShape secondInner;
    sf::CircleShape almostMiddle;
    sf::CircleShape center;
    
public:
    Target(float x, float y, float radius){
        outerMost = sf::CircleShape(radius);
        firstInner = sf::CircleShape(.8*radius);
        secondInner = sf::CircleShape(.6*radius);
        almostMiddle = sf::CircleShape(.4*radius);
        center = sf::CircleShape(.2*radius);
        
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
    
protected:
    void draw (sf::RenderTarget &target, sf::RenderStates states) const {
        target.draw(outerMost, states);
        target.draw(firstInner, states);
        target.draw(secondInner, states);
        target.draw(almostMiddle, states);
        target.draw(center, states);
    }
};

int main(int argc, char const** argv)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Darts!");

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a sprite to display
    sf::Texture texture;
    if (!texture.loadFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/cute_image.jpg")) {
        return EXIT_FAILURE;
    }
    sf::Sprite sprite(texture);

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/sansation.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text("Hello SFML", font, 50);
    text.setColor(sf::Color::Black);

    // Load a music to play
    sf::Music music;
    if (!music.openFromFile("/Users/ajmalkunnummal/Dropbox/Dev/VRDart/dartUI/dartUI/nice_music.ogg")) {
        return EXIT_FAILURE;
    }

    // Play the music
//    music.play();
    
    Target t = Target(0, 0, 100);

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

        // Draw the sprite
        window.draw(t);

        // Draw the string
        window.draw(text);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
