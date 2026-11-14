#include <SFML/Graphics.hpp>
#include <iostream>
#include "../risorse/texture/textures.hpp"


//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "01_basic_structure";
const unsigned window_width = 800;
const unsigned window_height = 600;
const float max_frame_rate = 60;
const sf::Color overworld_background_color(113, 137, 254);

// Ground
const sf::Vector2f ground_initial_size = {window_width, 150.f};
const sf::Vector2f ground_initial_pos = {0.f, window_height - ground_initial_size.y};

// Character
const sf::Vector2f character_initial_size = {32.f, 32.f};
const sf::Vector2f character_initial_pos = {20.f, window_height-ground_initial_size.y - character_initial_size.y};



/////////////
// Structs //
/////////////

struct Ground {
    sf::Vector2f pos, size;
    sf::Texture texture;

    Ground() : pos(ground_initial_pos), size(ground_initial_size) {
        if(!texture.loadFromMemory(smb_textures_png, smb_textures_png_len, false, sf::IntRect({229, 28}, {16, 16})))
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

    Character() : pos(character_initial_pos), size(character_initial_size) {
        if(!texture.loadFromMemory(smb_textures_png, smb_textures_png_len, false, sf::IntRect({171, 88}, {16, 16})))
            std::cerr << "Error loading character texture from memory" << std::endl;
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape character(size);
        character.setPosition(pos);
        character.setTexture(&texture);
        character.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(size.x / 2.f), static_cast<int>(size.y / 2.f)}));
        window.draw(character);
    }
};

struct State {
    Ground ground;
    Character character;
    bool is_paused;

    State() : ground(), character(), is_paused(true) {}

    void draw(sf::RenderWindow& window) {
        ground.draw(window);
        character.draw(window);
    }

    void update() {
        if(is_paused) return;
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
        gs.update();

    }
}
