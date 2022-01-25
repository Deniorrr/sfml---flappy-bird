#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <fstream>

using namespace std;

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 600;

bool isNumber(const string& str) {
    for (char const& c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

class Game
{
public:
    int score = 0;
    int highscore;
    sf::Text score_text;
    sf::Font font;
    bool started = false;
    bool over = false;
    sf::Sound bounce_sound;
    sf::Sound click_sound;
    sf::Sound score_sound;
    sf::Sound lose_sound;
    sf::SoundBuffer buffer, buffer2, buffer3, buffer4;

    Game() {
        set_font(score_text);
        load_sounds();
        update_score();
    }
    void update_score() {
        score_text.setString(to_string(score));
    };
    void lose() {
        lose_sound.play();
        highscore = get_highscore();
        started = false;
        if (score >= highscore) {
            highscore = score;
            std::fstream plik("denis_poczety_highscore.txt", std::ios::out);
            plik << score;
            plik.close();
        }
        over = true;
    }
    void set_font(sf::Text& _text) {
        font.loadFromFile("assets/font.ttf");
        _text.setFont(font);
        _text.setCharacterSize(60);
        _text.setFillColor(sf::Color::White);
        _text.setOutlineColor(sf::Color::Black);
        _text.setOutlineThickness(5);
        _text.setPosition(30, 10);
    }
    void reset_object() {
        score = 0;
        over = false;
    }
    int get_highscore() {
        std::fstream plik("denis_poczety_highscore.txt", std::ios::in);
        std::string wynik;
        getline(plik, wynik);
        if (isNumber(wynik) && wynik.length()>0) {
            return stoi(wynik);
        }
        else {
            return 0;
        }
    }
    void load_sounds() {
        buffer.loadFromFile("assets/sfx_wing.wav");//no support for mp3. wav, ogg, flac works
        bounce_sound.setBuffer(buffer);
        buffer2.loadFromFile("assets/minecraft_click.wav");
        click_sound.setBuffer(buffer2);
        buffer3.loadFromFile("assets/sfx_point.wav");
        score_sound.setBuffer(buffer3);
        buffer4.loadFromFile("assets/Minecraft-death-sound.wav");
        lose_sound.setBuffer(buffer4);
    }
};
Game game;

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
        texture.loadFromFile("assets/player.png");
        sprite.setTexture(texture);
        sprite.setOrigin(64, 45);
        sprite.setPosition(100, 300);
        sprite.scale(scale, scale);
    }
    void reset_speed() {
        if (sprite.getPosition().y > -128 * scale) {//if player not over the top border
            speed = starting_speed;
        }
    }
    void move() {
        sprite.move(0, (speed * -1));
        sprite.setRotation(speed * -3);
    }
    void change_speed() {
        speed -= gravity_force;
    }
    bool collides(sf::Sprite pipe) {
        return sprite.getGlobalBounds().intersects(pipe.getGlobalBounds());
    }
    bool out_of_border() {//if out of bottom border
        return (sprite.getPosition().y > WINDOW_WIDTH);
    }
    void reset_object() {
        sprite.setPosition(100, 300);
        sprite.setRotation(0);
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
    int speed = 5;
    int gap = 250;
    bool scored = false;
    float starting_width;
    Obstacle(float _starting_width) {
        set_texture();
        starting_width = _starting_width;
        sprite_bot.setPosition(starting_width, 100 + gap);
        sprite_top.setPosition(starting_width, 100);
    }
    void set_texture() {
        texture.loadFromFile("assets/pipe.png");
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
        if (sprite_bot.getPosition().x < sprite_bot.getLocalBounds().width * (-0.2)) set_position(); //if out of left border: go back to right border
    }
    void set_position() {
        int height = rand() % (max_height - min_height) + (min_height);//set random height 
        sprite_bot.setPosition(WINDOW_WIDTH, height + gap);
        sprite_top.setPosition(WINDOW_WIDTH, height);
        scored = false;
    }
    void check_score() {
        if (sprite_bot.getPosition().x < 100 && !scored) {//if player dodged obstacle
            game.score += 1;
            game.update_score();
            game.score_sound.play();
            scored = true;
        }
    }
    void reset_object() {
        sprite_bot.setPosition(starting_width, 100 + gap);
        sprite_top.setPosition(starting_width, 100);
        scored = false;
    }
};

class Paralax
{
public:
    sf::Sprite sprite;
    sf::Sprite sprite2;
    sf::Texture texture;
    float speed;
    Paralax(string image_src, float _speed) {
        texture.loadFromFile(image_src);
        speed = _speed;
        set_texture();
        set_position();
    }
    void set_texture() {
        sprite.setTexture(texture);
        sprite2.setTexture(texture);
    }
    void set_position() {
        sprite.setPosition(0, 0);
        sprite2.setPosition(1000, 0);
    }
    void move() {
        sprite.move(speed, 0);
        sprite2.move(speed, 0);
    }
    void check_position() {
        if (out_of_border(sprite)) sprite.setPosition(1000, 0);
        if (out_of_border(sprite2)) sprite2.setPosition(1000, 0);
    }
    bool out_of_border(sf::Sprite _sprite) {
        return (_sprite.getPosition().x <= -1000);
    }
};

class Button
{
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Texture texture_hover;
    Button(string image, string image_hover, float x, float y) {
        texture.loadFromFile(image);
        texture_hover.loadFromFile(image_hover);
        set_standard_texture();
        sprite.scale(0.4, 0.4);
        sprite.setPosition(x, y);
    }
    void set_hover_texture() {
        sprite.setTexture(texture_hover);
    }
    void set_standard_texture() {
        sprite.setTexture(texture);
    }
    bool mouse_hovers(sf::RenderWindow& window) {
        return (sprite.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y));//if mouse is over an object
    }
};

int main()
{
    srand((unsigned int)time(NULL));
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird - Denis Poczety");
    window.setFramerateLimit(100);

    Player player;
    Obstacle obstacle(WINDOW_WIDTH);
    Obstacle obstacle2(WINDOW_WIDTH * 1.6);
    Paralax paralax1("assets/para1t.png", -0.5);
    Paralax paralax2("assets/para.png", -1);

    Button play_again("assets/play again.png", "assets/play againh.png", 100, 440);
    Button quit_game("assets/quit game.png", "assets/quitgameh.png", 500, 440);

    bool playagain_hovered = false;
    bool quitgame_hovered = false;

    sf::Text udied;
    udied.setFont(game.font);
    udied.setCharacterSize(60);
    udied.setFillColor(sf::Color::White);
    udied.setOutlineColor(sf::Color::Black);
    udied.setOutlineThickness(5);
    udied.setString("Game over!");
    udied.setPosition((WINDOW_WIDTH / 2) - (udied.getLocalBounds().width / 2), 100);//place object in the middle

    sf::Text score;
    score.setFont(game.font);
    score.setCharacterSize(30);
    score.setFillColor(sf::Color::White);
    score.setOutlineColor(sf::Color::Black);
    score.setOutlineThickness(3);

    sf::Text highscore;
    highscore.setFont(game.font);
    highscore.setCharacterSize(30);
    highscore.setFillColor(sf::Color::White);
    highscore.setOutlineColor(sf::Color::Black);
    highscore.setOutlineThickness(3);

    while (window.isOpen())//infinite loop for every frame
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered && event.text.unicode == 32) {
                if (!game.started) {
                    if (!game.over) {
                        game.started = true;
                    }
                }
                if (event.text.unicode == 32) {
                    player.reset_speed();
                    if (game.started) game.bounce_sound.play();
                }
            }
        }
        if (game.started) {
            if (player.out_of_border()) {
                game.lose();
            }
            if (player.collides(obstacle.sprite_bot)) {
                game.lose();
            }
            if (player.collides(obstacle.sprite_top)) {
                game.lose();
            }
            if (player.collides(obstacle2.sprite_bot)) {
                game.lose();
            }
            if (player.collides(obstacle2.sprite_top)) {
                game.lose();
            }
            obstacle.check_score();
            obstacle2.check_score();
            game.update_score();
            obstacle.reset_position();
            obstacle2.reset_position();
            player.move();
            obstacle.move();
            obstacle2.move();
            player.change_speed();
        }
        if (!game.started) {
            game.score_text.setString("Press spacebar to start");
        }
        window.clear(sf::Color(0, 0, 150));
        window.draw(paralax1.sprite);
        window.draw(paralax1.sprite2);
        window.draw(paralax2.sprite);
        window.draw(paralax2.sprite2);
        if (game.started) {
            paralax1.move();
            paralax2.move();
        }
        paralax1.check_position();
        paralax2.check_position();

        window.draw(player.sprite);
        window.draw(obstacle.sprite_bot);
        window.draw(obstacle.sprite_top);
        window.draw(obstacle2.sprite_bot);
        window.draw(obstacle2.sprite_top);
        if (!game.over) window.draw(game.score_text);
        sf::RectangleShape rectangle;
        rectangle.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        rectangle.setFillColor(sf::Color(255, 0, 0, 100));
        if (game.over) {
            string death_score = "Score: ";
            death_score = death_score.append(to_string(game.score));
            score.setString(death_score);
            score.setPosition((WINDOW_WIDTH / 2) - (score.getLocalBounds().width / 2), 250);//place object in the middle

            string death_highscore = "Highscore: ";
            death_highscore = death_highscore.append(to_string(game.highscore));
            highscore.setString(death_highscore);
            highscore.setPosition((WINDOW_WIDTH / 2) - (highscore.getLocalBounds().width / 2), 320);//place object in the middle

            window.draw(rectangle);
            window.draw(udied);
            window.draw(score);
            window.draw(highscore);
            if (quit_game.mouse_hovers(window)) {
                if (!quitgame_hovered) {
                    quitgame_hovered = true;
                    quit_game.set_hover_texture();
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    game.click_sound.play();
                    window.close();
                }
            }
            else if (quitgame_hovered) {
                quitgame_hovered = false;
                quit_game.set_standard_texture();
            }
            if (play_again.mouse_hovers(window)) {
                if (!playagain_hovered) {
                    playagain_hovered = true;
                    play_again.set_hover_texture();
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    game.click_sound.play();
                    game.over = false;
                    game.reset_object();
                    player.reset_object();
                    obstacle.reset_object();
                    obstacle2.reset_object();
                }
            }
            else if (playagain_hovered) {
                playagain_hovered = false;
                play_again.set_standard_texture();
            }
            window.draw(play_again.sprite);
            window.draw(quit_game.sprite);
        }
        window.display();
    }
    return 0;
}
