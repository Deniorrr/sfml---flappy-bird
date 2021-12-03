#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <stdlib.h>
#include <time.h>
using namespace std;

class Settings
{
public:
    
    int window_width = 1000;
    int window_height = 600;
    int player_bounce_speed = 8;
    int player_beginning_width = 100;
    int player_beginning_height = 300;
    int player_gravity_force = 0.3;
    int pipe_speed = 4;
    //int pipe_gap = 200;
};

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 600;

class Game
{
public:
    void lose(sf::RenderWindow& window) {
        window.close();
    }
};
Game game;
//class Window
//{
//public:
//    int width, height;
//    Window(int _width, int _height) {
//        height = _height;
//        width = _width;
//    }
//};

class Player
{
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float starting_speed = 8;
    float speed;
    float gravity_force = 0.3;
    float scale = 0.7;
    Player() {
        set_texture();
        reset_speed();
    };

    void set_texture() {
        texture.loadFromFile("player.png");
        sprite.setTexture(texture);
        sprite.setPosition(100, 300);
        sprite.scale(scale, scale);
        sprite.setOrigin(64, 45);
    }

    void reset_speed() {
        if (sprite.getPosition().y > -128*scale) speed = starting_speed;
    }
    void move() {
        sprite.move(0, (speed * -1));
        sprite.setRotation(speed* -3);
    }
    void change_speed() {
        speed -= gravity_force;
    }
    bool collides(sf::Sprite pipe) {
        return sprite.getGlobalBounds().intersects(pipe.getGlobalBounds());
    }
    bool out_of_border() {
        return (sprite.getPosition().y > WINDOW_WIDTH);
    }
    bool draw_object(sf::RenderWindow window) {
        window.draw(sprite);
    }
};

class Obstacle
{
public:
    sf::Sprite sprite_bot;
    sf::Sprite sprite_top;
    sf::Texture texture;
    int min_height = 50;
    int max_height = 250;
    int speed = 4;
    int gap = 300;
    Obstacle(float starting_width) {
        set_texture();
        sprite_bot.setPosition(starting_width, 100+gap);
        sprite_top.setPosition(starting_width, 100);
    }
    void set_texture() {
        texture.loadFromFile("pipe.png");
        sprite_bot.setTexture(texture);
        sprite_bot.scale(0.2, 0.2);
        
        
        sprite_top.setTexture(texture);
        sprite_top.scale(0.2, -0.2);
        
    }

    void move() {
        sprite_bot.move(speed * (-1), 0);
        sprite_top.move(speed * (-1), 0);
    }
    void reset_position() {
        if (sprite_bot.getPosition().x < sprite_bot.getLocalBounds().width * (-0.2))
        {
            set_position();
        }
    }
    void set_position() {
        int height = rand() % (max_height - min_height) + (min_height);
        sprite_bot.setPosition(WINDOW_WIDTH, height + gap);
        sprite_top.setPosition(WINDOW_WIDTH, height);
    }
    bool draw_object(sf::RenderWindow window) {
        window.draw(sprite_bot);
        window.draw(sprite_top);
    }
};

int main()
{
    srand((unsigned int)time(NULL));
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird - Denis Poczêty");
    window.setFramerateLimit(100);

    Player player;
    Obstacle obstacle(WINDOW_WIDTH);
    Obstacle obstacle2(WINDOW_WIDTH*1.6);
    /*sf::Sprite player;
    sf::Texture player_texture;
    player_texture.loadFromFile("player.png");
    player.setTexture(player_texture);*/


    /*sf::Sprite pipe_down;
    sf::Texture pipe_down_texture;
    pipe_down_texture.loadFromFile("pipe.png");
    pipe_down.setTexture(pipe_down_texture);
    pipe_down.setPosition(window_object.width, 400);
    pipe_down.scale(0.2, 0.2);

    sf::Sprite pipe_up;
    pipe_up.setTexture(pipe_down_texture);
    pipe_up.setPosition(window_object.width, 100);
    pipe_up.scale(0.2, -0.2);
    int pipe_speed = 4;*/
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 32) player.reset_speed();
            }
            
        }
        if (player.out_of_border()){
            game.lose(window);
        }
        if (player.collides(obstacle.sprite_bot)) {
            game.lose(window);
        }
        if (player.collides(obstacle.sprite_top)) {
            game.lose(window);
        }
        if (player.collides(obstacle2.sprite_bot)) {
            game.lose(window);
        }
        if (player.collides(obstacle2.sprite_top)) {
            game.lose(window);
        }
        
        obstacle.reset_position();
        obstacle2.reset_position();
        player.move();
        obstacle.move();
        obstacle2.move();
        player.change_speed();
        
        window.clear(sf::Color(0,0,150));
        window.draw(player.sprite);
        window.draw(obstacle.sprite_bot);
        window.draw(obstacle.sprite_top);
        window.draw(obstacle2.sprite_bot);
        window.draw(obstacle2.sprite_top);
        window.display();
    }

    return 0;
}