#include <SFML/Graphics.hpp>
#include <iostream>
#include "../risorse/texture/textures.hpp"
#include "../risorse/font/fonts.hpp"

//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "06_better_ground_and_collisions";
const unsigned window_width = 800;
const unsigned window_height = 600;
const float max_frame_rate = 60;
const sf::Color overworld_background_color(146, 144, 255);

// Block
const sf::Vector2f block_size = {32.f, 32.f};

// Ground
const sf::Vector2f ground_initial_size = {window_width, 150.f};
const sf::Vector2f ground_initial_pos = {0.f, window_height - ground_initial_size.y};
const sf::Vector2i block_number = {25, 5};
const sf::Vector2f ground_displacement = {0.f, 450.f};

// Character
const sf::Vector2f character_initial_size = {32.f, 32.f};
const sf::Vector2f character_initial_pos = {20.f, window_height - ground_initial_size.y - character_initial_size.y};
const float character_initial_speed = 333.f;
const float gravity = 1500.f;
const float jump_strength = 650.f;



/////////////
// Structs //
/////////////

struct Block {
    sf::Vector2f pos, size;
    sf::Texture texture;

    Block(const sf::Vector2f& pos, const sf::Vector2f& size) : pos(pos), size(size) {
        if(!texture.loadFromMemory(smb_textures_png, smb_textures_png_len, false, sf::IntRect({229, 28}, {16, 16})))
            std::cerr << "Error loading ground block texture from memory" << std::endl;
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape block(size);
        block.setPosition(pos);
        block.setTexture(&texture);
        window.draw(block);
    }
};

struct Ground {
    std::vector<Block> blocks;

    Ground() {
        for (int row = 0; row < block_number.y; ++row) {
            for (int col = 0; col < block_number.x; ++col) {
                if(col == 14 || col == 15) continue;
                sf::Vector2f block_pos = {ground_displacement.x + (col * block_size.x), ground_displacement.y + (row * block_size.y)};
                blocks.push_back(Block(block_pos, block_size));
            }
        }
        blocks.push_back(Block({20.f, window_height-ground_initial_size.y - 100.f}, block_size));
    }
    
    void draw(sf::RenderWindow& window) {
        for (long unsigned int i = 0; i < blocks.size(); i++) {
            blocks[i].draw(window);
        }
    }
};

struct Character {
    sf::Vector2f pos, size, old_pos;
    sf::Texture full_texture;
    sf::IntRect character_texture;   
    float speed, y_velocity;
    bool is_jumping, is_dead, is_big_mario, is_fire_mario;

    Character() : pos(character_initial_pos), size(character_initial_size), speed(character_initial_speed), y_velocity(0.f), is_jumping(false), is_big_mario(false), is_fire_mario(false) {
        if(!full_texture.loadFromMemory(smb_characters_png, smb_characters_png_len))
            std::cerr << "Error loading the full texture from memory" << std::endl;
        character_texture = sf::IntRect({0, 8}, {16, 16});
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape character(size);
        character.setPosition(pos);
        character.setTexture(&full_texture);
        character.setTextureRect(character_texture);
        window.draw(character);
    }

    void move_left(float elapsed) {
        pos.x -= speed*elapsed;
    }

    void move_right(float elapsed) {
        pos.x += speed*elapsed;
    }

    void jump() { 
        if(!is_jumping) {
            y_velocity = -jump_strength;
            is_jumping = true;
        }
    }

    void apply_physics(float elapsed) {
        y_velocity += gravity*elapsed;
        pos.y += y_velocity*elapsed;
    }

    void get_power_up(bool type) {
        if(!type) {
            if(is_big_mario || is_fire_mario) return;
            is_big_mario = true;
            pos.y -= 32.f;
            size.y = 64.f;
            character_texture = sf::IntRect({0, 32}, {16, 32});
        }else {
            if(is_fire_mario) return;
            if(!is_big_mario) {
                pos.y -= 32.f;
                size.y = 64.f;
            }
            is_fire_mario = true;
            character_texture = sf::IntRect({0, 140}, {16, 32});
        }
    }

    void hit() {
        if(!is_big_mario)
            is_dead = true;
        else if(is_big_mario) {
            is_big_mario = false;
            size.y = 32.f;
            pos.y += 32.f;
        }
    }

    void restart() {
        pos = character_initial_pos;
        size = character_initial_size;
        old_pos = character_initial_pos;
        character_texture = sf::IntRect({0, 8}, {16, 16});
        speed = character_initial_speed;
        y_velocity = 0.f;
        is_jumping = false;
        is_dead = false;
        is_big_mario = false;
        is_fire_mario = false;
    }
};

struct Powerup {
    sf::Vector2f pos, size;
    sf::Texture texture;
    bool type, is_active; // 0 = Big Mario, 1 = Fire Mario

    Powerup(sf::Vector2f initial_pos, bool type) : pos(initial_pos), size({32.f, 32.f}), type(type), is_active(true) {
        if(!type) {
            if(!texture.loadFromMemory(smb_items_png, smb_items_png_len, false, sf::IntRect({0, 8}, {16, 16})))
                std::cerr << "Error loading mushroom power-up texture from memory" << std::endl;
        }else {
            if(!texture.loadFromMemory(smb_items_png, smb_items_png_len, false, sf::IntRect({68, 8}, {16, 16})))
                std::cerr << "Error loading fire flower power-up texture from memory" << std::endl;
        }
    }

    void draw(sf::RenderWindow& window) {
        if(!is_active) return;
        sf::RectangleShape powerup(size);
        powerup.setPosition(pos);
        powerup.setTexture(&texture);
        powerup.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(size.x / 2.f), static_cast<int>(size.y / 2.f)}));
        window.draw(powerup);
    }

    void restart() {
        is_active = true;
    }
};

struct Enemy {
    sf::Vector2f pos, size;
    sf::Texture texture;
    bool type, is_alive; // 0 = Goomba, 1 = Piranha plant
    
    Enemy(sf::Vector2f initial_pos, bool type) : pos(initial_pos), size({32.f, 32.f}), is_alive(true) {
        if(!type) {
            if(!texture.loadFromMemory(smb_textures_png, smb_textures_png_len, false, sf::IntRect({73, 300}, {16, 16})))
                std::cerr << "Error loading Goomba texture from memory" << std::endl;
        }else {
            size.y = 46.f;
            if(!texture.loadFromMemory(smb_textures_png, smb_textures_png_len, false, sf::IntRect({161, 309}, {16, 23})))
                std::cerr << "Error loading Piranha Plant texture from memory" << std::endl;
        }
    }

    void draw(sf::RenderWindow& window) {
        if(!is_alive) return;
        sf::RectangleShape enemy(size);
        enemy.setPosition(pos);
        enemy.setTexture(&texture);
        enemy.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(size.x / 2.f), static_cast<int>(size.y / 2.f)}));
        window.draw(enemy);
    }

    void hit(){
        is_alive = false;
    }

    void restart() {
        is_alive = true;
    }
};

struct Button {
    sf::RectangleShape button;
    sf::Text button_text;

    Button(sf::Vector2f pos, sf::Vector2f size, const sf::Font& font, std::string text) : button_text(font, text, 24) {
        button.setPosition(pos);
        button.setSize(size);
        button.setFillColor(sf::Color::Black);

        button_text.setFillColor(sf::Color::White);
        sf::FloatRect bounds = button_text.getLocalBounds();
        button_text.setPosition({pos.x + (size.x - bounds.size.x) / 2.f - bounds.position.x, pos.y + (size.y - bounds.size.y) / 2.f - bounds.position.y});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(button);
        window.draw(button_text);
    }

    bool is_clicked(sf::Vector2f mouse_pos) {
        return button.getGlobalBounds().contains(mouse_pos);
    }
};

struct Menu {
    std::vector<Button> menu;
    sf::Font font;
    
    Menu() {
        if(!font.openFromMemory(smb_font_ttf, smb_font_ttf_len))
            std::cerr << "Error loading font from memory" << std::endl;
        menu.push_back(Button({200.f, 200.f}, {400.f, 50.f}, font, "RIPRENDI"));
        menu.push_back(Button({200.f, 280.f}, {400.f, 50.f}, font, "CAMBIA LIVELLO"));
        menu.push_back(Button({200.f, 360.f}, {400.f, 50.f}, font, "VEDI COMANDI"));
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape overlay({800.f, 600.f});
        overlay.setFillColor(sf::Color(0, 0, 0, 150)); 
        window.draw(overlay);

        for(unsigned int i = 0; i < menu.size(); ++i) {
            menu[i].draw(window);
        }
    }
};

struct State {
    Ground ground;
    Character character;
    Menu menu;
    Powerup mushroom, flower;
    Enemy goomba, plant;
    bool is_paused, move_left, move_right, jump;
    int menu_selection;

    State() : mushroom({250.f, window_height - ground_initial_size.y - 32.f}, 0), flower({550.f, window_height - ground_initial_size.y - 32.f}, 1), goomba({125.f, window_height - ground_initial_size.y - 32.f}, 0), plant({375.f, window_height - ground_initial_size.y - 46.f}, 1), is_paused(true), move_left(false), move_right(false), jump(false), menu_selection(0) {}

    void draw(sf::RenderWindow& window) {
        ground.draw(window);
        mushroom.draw(window);
        flower.draw(window);
        goomba.draw(window);
        plant.draw(window);
        character.draw(window);

        if(is_paused)
            menu.draw(window);
    }

    void update(float elapsed) {
        if(is_paused) {
            for(int i = 0; i < 3; ++i) {
                if(i == menu_selection) {
                    menu.menu[i].button.setFillColor(sf::Color(150, 150, 150)); 
                    menu.menu[i].button.setOutlineThickness(3.f);
                }else {
                    menu.menu[i].button.setFillColor(sf::Color::Black);
                    menu.menu[i].button.setOutlineThickness(0.f);
                }
            }
            return;
        } 

        character.old_pos = character.pos;

        if(character.is_dead) 
            restart();
        if(move_left) 
            character.move_left(elapsed);
        else if(move_right)
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
        
        bool is_on_ground = false;

        for (long unsigned int i = 0; i < ground.blocks.size(); i++) {
            float m_left = character.pos.x;
            float m_right = character.pos.x + character.size.x;
            float m_top = character.pos.y;
            float m_bottom = character.pos.y + character.size.y;

            float b_left = ground.blocks[i].pos.x;
            float b_right = ground.blocks[i].pos.x + ground.blocks[i].size.x;
            float b_top = ground.blocks[i].pos.y;
            float b_bottom = ground.blocks[i].pos.y + ground.blocks[i].size.y;

            if(m_right > b_left && m_left < b_right && m_bottom > b_top && m_top < b_bottom) {
                float old_bottom = character.old_pos.y + character.size.y;
                float old_top = character.old_pos.y;
                float old_right = character.old_pos.x + character.size.x;
                float old_left = character.old_pos.x;

                if(old_bottom <= b_top) {
                    if (character.y_velocity >= 0.f) {
                        character.pos.y = b_top - character.size.y;
                        character.y_velocity = 0.f;
                        character.is_jumping = false;
                        is_on_ground = true;
                    }
                }else if(old_top >= b_bottom) {
                    character.pos.y = b_bottom;
                    character.y_velocity = 0.f;
                }else {
                    if(old_right <= b_left)
                        character.pos.x = b_left - character.size.x;
                    else if (old_left >= b_right)
                        character.pos.x = b_right;
                }
            }
        }

        if (!is_on_ground)
            character.is_jumping = true; 

        if(character.pos.y > window_height + 1000.f)
            character.is_dead = true;
    }

    void powerup_collision() {
        if(mushroom.is_active || flower.is_active){
            if(character.pos.x + character.size.x >= mushroom.pos.x && character.pos.x <= mushroom.pos.x + mushroom.size.x && character.pos.y + character.size.y >= mushroom.pos.y && character.pos.y + character.size.y <= mushroom.pos.y + mushroom.size.y){
                if(!character.is_big_mario && !character.is_fire_mario)
                    character.get_power_up(mushroom.type);
                mushroom.is_active = false;
            }
            if(character.pos.x + character.size.x >= flower.pos.x && character.pos.x <= flower.pos.x + flower.size.x && character.pos.y + character.size.y >= flower.pos.y && character.pos.y + character.size.y <= flower.pos.y + flower.size.y){
                if(!character.is_fire_mario)
                    character.get_power_up(flower.type);
                flower.is_active = false;
            } 
        }
    }

    void enemy_collision() {
        if(character.pos.x + character.size.x >= goomba.pos.x && character.pos.x <= goomba.pos.x + goomba.size.x && character.pos.y + character.size.y >= goomba.pos.y && character.pos.y + character.size.y <= goomba.pos.y + goomba.size.y)
            character.hit();
        if(character.pos.x + character.size.x >= plant.pos.x && character.pos.x <= plant.pos.x + plant.size.x && character.pos.y + character.size.y >= plant.pos.y && character.pos.y + character.size.y <= plant.pos.y + plant.size.y)
            plant.hit();
    }

    void collisions() {
        map_limits();
        powerup_collision();
        enemy_collision();
    }

    void restart() {
        character.restart();
        mushroom.restart();
        flower.restart();
        goomba.restart();
        plant.restart();
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
    if(new_aspect < aspect)
        ws = {ws.x,static_cast<unsigned>(ws.x/aspect)};
    else
        ws = {static_cast<unsigned>(ws.y*aspect),ws.y};
    window.setSize(ws);
}

void handle(const sf::Event::KeyPressed& key_pressed, State &gs){
    if(key_pressed.scancode == sf::Keyboard::Scancode::Escape)
        gs.is_paused = !gs.is_paused;
    else if (gs.is_paused){
        if(key_pressed.scancode == sf::Keyboard::Scancode::Down)
            gs.menu_selection = (gs.menu_selection + 1) % 3;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Up)
            gs.menu_selection = (gs.menu_selection - 1 + 3) % 3;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Enter) {
            if (gs.menu_selection == 0)
                gs.is_paused = false;
        }
    } 
    else{
        if(key_pressed.scancode == sf::Keyboard::Scancode::Left)
            gs.move_left = true;
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Right)
            gs.move_right = true;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Up)
            gs.jump = true;
    }
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
    window.setKeyRepeatEnabled(false);
    State gs;
    sf::Clock clock;
    clock.start();

    while(window.isOpen()) {
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
