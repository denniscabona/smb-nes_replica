#include <SFML/Graphics.hpp>
#include <iostream>
#include "../risorse/texture/textures.hpp"


//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "02_character_movement";
const unsigned window_width = 800;
const unsigned window_height = 600;
const float max_frame_rate = 60;
const sf::Color overworld_background_color(113, 137, 254);

// Ground
const sf::Vector2f ground_initial_size = {window_width, 150.f};
const sf::Vector2f ground_initial_pos = {0.f, window_height - ground_initial_size.y};

// Character
const sf::Vector2f character_initial_size = {32.f, 32.f};
const sf::Vector2f character_initial_pos = {20.f, window_height - ground_initial_size.y - character_initial_size.y};
const float character_initial_speed = 333.f;
const float gravity = 1500.f;
const float jump_strength = 650.f;


/////////////
// Structs //
/////////////

struct Ground {
    sf::Vector2f pos, size;
    sf::Texture texture;

    Ground() : pos(ground_initial_pos), size(ground_initial_size) {
        if(!texture.loadFromMemory(smb_map_png, smb_map_png_len, false, sf::IntRect({229, 28}, {16, 16})))
            std::cerr << "Error loading ground texture from memory" << std::endl;
        texture.setRepeated(true);
    }
    
    void draw(sf::RenderWindow& window) {
        sf::RectangleShape ground(size);
        ground.setPosition(pos);
        ground.setTexture(&texture);
        ground.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(size.x/2), static_cast<int>(size.y/2)}));
        window.draw(ground);
    }
};

struct Character {
    sf::Vector2f pos, size;
    sf::Texture texture;   
    float speed, y_velocity;
    bool is_jumping;

    Character() : pos(character_initial_pos), size(character_initial_size), speed(character_initial_speed), y_velocity(0.f), is_jumping(false) {
        if(!texture.loadFromMemory(smb_map_png, smb_map_png_len, false, sf::IntRect({171, 88}, {16, 16})))
            std::cerr << "Error loading Mario texture from memory" << std::endl;
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape character(size);
        character.setPosition(pos);
        character.setTexture(&texture);
        character.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(size.x / 2.f), static_cast<int>(size.y / 2.f)}));
        window.draw(character);
    }

    void move_left(float elapsed) {
        pos.x -= speed*elapsed;
    }

    void move_right(float elapsed) {
        pos.x += speed*elapsed;
    }

    void jump() { 
        if(!is_jumping){
            y_velocity = -jump_strength;
            is_jumping = true;
        }
    }

    void apply_physics(float elapsed) {
        y_velocity += gravity*elapsed;
        pos.y += y_velocity*elapsed;
    }
};

struct State {
    Ground ground;
    Character character;
    bool is_paused, move_left, move_right, jump;

    State() : ground(), character(), is_paused(true), move_left(false), move_right(false), jump(false) {}

    void draw(sf::RenderWindow& window) {
        ground.draw(window);
        character.draw(window);
    }

    void update(float elapsed) {
        if(is_paused) return;
        if(move_left) 
            character.move_left(elapsed);
        else if (move_right)
            character.move_right(elapsed);
        if(jump)
            character.jump();

        character.apply_physics(elapsed);

        collisions();
    }

    void map_limits() {
        if(character.pos.x + character.size.x > window_width)
            character.pos.x = window_width - character.size.x;
        else if(character.pos.x < 0.f)
            character.pos.x = 0.f;

        if(character.pos.y + character.size.y >= window_height - ground.size.y){
            character.pos.y = window_height - ground.size.y - character.size.y; 
            character.is_jumping = false;
            character.y_velocity = 0.f;
        }
    }

    void collisions() {
        map_limits();
    }
};



////////////
// Events //
////////////

void handle_close (sf::RenderWindow& window) {
    window.close();
}

void handle_resize (const sf::Event::Resized& resized, sf::RenderWindow& window) {   // constrain aspect ratio and map always the same portion of the world
    float aspect = static_cast<float>(window_width)/static_cast<float>(window_height);
    sf::Vector2u ws = resized.size;
    float new_aspect = static_cast<float>(ws.x)/static_cast<float>(ws.y);
    if (new_aspect < aspect)
        ws = {ws.x,static_cast<unsigned>(ws.x/aspect)};
    else
        ws = {static_cast<unsigned>(ws.y*aspect),ws.y};
    window.setSize(ws);
}

void handle(const sf::Event::KeyPressed& key_pressed, State &gs){
    if(key_pressed.scancode == sf::Keyboard::Scancode::Escape)
        gs.is_paused = !gs.is_paused;
    else if(key_pressed.scancode == sf::Keyboard::Scancode::Left)
        gs.move_left = true;
    else if(key_pressed.scancode == sf::Keyboard::Scancode::Right)
        gs.move_right = true;  
    else if(key_pressed.scancode == sf::Keyboard::Scancode::Up)
        gs.jump = true;
}   

void handle(const sf::Event::KeyReleased& key_released, State &gs){
    if(key_released.scancode == sf::Keyboard::Scancode::Left)
        gs.move_left = false;
    else if(key_released.scancode == sf::Keyboard::Scancode::Right)
        gs.move_right = false;  
    else if(key_released.scancode == sf::Keyboard::Scancode::Up)
        gs.jump= false;
}

void handle(const sf::Event::FocusLost& focus_lost, State &gs){
    gs.is_paused = true;
}

template <typename T>
void handle(const T &, State &gs) {
    // All unhandled events will end up here   
}



//////////
// Loop //
//////////

int main() {
    sf::RenderWindow window (sf::VideoMode ({window_width, window_height}), window_title);
    window.setFramerateLimit (max_frame_rate);
    window.setMinimumSize(window.getSize());
    State gs;
    sf::Clock clock;
    clock.start();

    while (window.isOpen()) {
        // events
        window.handleEvents (
                             [&window](const sf::Event::Closed&) { handle_close (window); },
                             [&window](const sf::Event::Resized& event) { handle_resize (event, window); },
                             [&gs](const auto& event) { handle(event, gs); }
        );

        // display
        window.clear(overworld_background_color);
        gs.draw(window);
        window.display();
        gs.update(clock.restart().asSeconds());

    }

    clock.stop();
}
