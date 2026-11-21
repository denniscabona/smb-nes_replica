#include <SFML/Graphics.hpp>
#include <iostream>
#include "../risorse/texture/textures.hpp"
#include "../risorse/font/fonts.hpp"

//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "09_camera_scrolling";
const unsigned window_width = 800;
const unsigned window_height = 600;
const float max_frame_rate = 60;

// Block
const sf::Vector2f block_size = {32.f, 32.f};

// Ground
const sf::Vector2f ground_initial_size = {window_width, 150.f};
const sf::Vector2f ground_initial_pos = {0.f, window_height - ground_initial_size.y};
const sf::Vector2i block_number = {60, 5};
const sf::Vector2f ground_displacement = {0.f, 450.f};

// Level
const sf::Color overworld_background_color(146, 144, 255);
const unsigned level_width = 1920;
const unsigned level_height = 1120;
const float gravity = 1500.f;

// Character
const sf::Vector2f character_initial_size = {32.f, 32.f};
const sf::Vector2f character_initial_pos = {20.f, window_height - ground_initial_size.y - character_initial_size.y};
const float character_initial_speed = 333.f;
const float jump_strength = 650.f;



/////////////
// Structs //
/////////////

struct Block {
    sf::Vector2f pos, size;
    sf::IntRect block_texture;

    Block(const sf::Vector2f& pos, const sf::Vector2f& size) : pos(pos), size(size), block_texture({229, 28}, {16, 16}) {}

    void draw(sf::RenderWindow& window, const sf::Texture& texture) {
        sf::RectangleShape block(size);
        block.setPosition(pos);
        block.setTexture(&texture);
        block.setTextureRect(block_texture);
        window.draw(block);
    }
};

struct Powerup {
    sf::Vector2f pos, size;
    sf::IntRect power_up_texture;
    bool is_active;
    int type; // 0 = Big Mario, 1 = Fire Mario

    Powerup(sf::Vector2f initial_pos, int type) : pos(initial_pos), size({32.f, 32.f}), is_active(true), type(type) {
        if(type == 0)
            power_up_texture = sf::IntRect({0, 8}, {16, 16});
        else if(type == 1)
            power_up_texture = sf::IntRect({68, 8}, {16, 16});
        else 
            std::cerr << "Error! Wrong power-up type integer value" << std::endl;
    }

    void draw(sf::RenderWindow& window, const sf::Texture& texture) {
        if(!is_active) return;
        sf::RectangleShape powerup(size);
        powerup.setPosition(pos);
        powerup.setTexture(&texture);
        powerup.setTextureRect(power_up_texture);
        window.draw(powerup);
    }

    void restart() {
        is_active = true;
    }
};

struct Enemy {
    sf::Vector2f pos, size;
    sf::IntRect enemy_texture;
    bool is_alive;
    int type; // 0 = Goomba, 1 = Piranha plant
    
    Enemy(sf::Vector2f initial_pos, int type) : pos(initial_pos), size({32.f, 32.f}), is_alive(true), type(type) {
        if(type == 0)
            enemy_texture = sf::IntRect({73, 300}, {16, 16});
        else if(type == 1) {
            size.y = 46.f;
            enemy_texture = sf::IntRect({161, 309}, {16, 23});
        }else
            std::cerr << "Error! Wrong enemy type integer value" << std::endl;
    }

    void draw(sf::RenderWindow& window, const sf::Texture& texture) {
        if(!is_alive) return;
        sf::RectangleShape enemy(size);
        enemy.setPosition(pos);
        enemy.setTexture(&texture);
        enemy.setTextureRect(enemy_texture);
        window.draw(enemy);
    }

    void hit(){
        is_alive = false;
    }

    void restart() {
        is_alive = true;
    }
};

struct Level {
    std::vector<Block> blocks;
    std::vector<Powerup> powerups;
    std::vector<Enemy> enemies;
    sf::Texture ground_and_enemy_texture, power_up_texture;

    Level() {
        if(!ground_and_enemy_texture.loadFromMemory(smb_map_png, smb_map_png_len))
            std::cerr << "Error loading the map texture from memory" << std::endl;
        if(!power_up_texture.loadFromMemory(smb_items_png, smb_items_png_len))
            std::cerr << "Error loading the items texture from memory" << std::endl;

        for (int row = 0; row < block_number.y; ++row) {
            for (int col = 0; col < block_number.x; ++col) {
                if(col == 14 || col == 15 || col == 41 || col == 42) continue;
                sf::Vector2f block_pos = {ground_displacement.x + (col * block_size.x), ground_displacement.y + (row * block_size.y)};
                blocks.push_back(Block(block_pos, block_size));
            }
        }
        blocks.push_back(Block({20.f, window_height - ground_initial_size.y - 100.f}, block_size));
        powerups.push_back(Powerup({250.f, window_height - ground_initial_size.y - 32.f}, 0));
        powerups.push_back(Powerup({550.f, window_height - ground_initial_size.y - 32.f}, 1));
        enemies.push_back(Enemy({125.f, window_height - ground_initial_size.y - 32.f}, 0));
        enemies.push_back(Enemy({375.f, window_height - ground_initial_size.y - 46.f}, 1));
    }
    
    void draw(sf::RenderWindow& window) {
        for (long unsigned int i = 0; i < blocks.size(); i++)
            blocks[i].draw(window, ground_and_enemy_texture);
        for (long unsigned int i = 0; i < powerups.size(); i++)
            powerups[i].draw(window, power_up_texture);
        for (long unsigned int i = 0; i < enemies.size(); i++)
            enemies[i].draw(window, ground_and_enemy_texture);
    }

    void restart() {
        for (long unsigned int i = 0; i < powerups.size(); i++)
            powerups[i].restart();
        for (long unsigned int i = 0; i < enemies.size(); i++)
            enemies[i].restart();   
    }
};

struct Character {
    sf::Vector2f pos, size, old_pos;
    sf::Texture full_texture;
    sf::IntRect character_texture;   
    float speed, y_velocity, invincibility_time;
    bool is_jumping, is_dead;
    int character_state; // 0 = Small Mario, 1 = Big Mario, 2 = Fire Mario

    Character() : pos(character_initial_pos), size(character_initial_size), old_pos(character_initial_pos), speed(character_initial_speed), y_velocity(0.f), invincibility_time(0), is_jumping(false), is_dead(false), character_state(0) {
        if(!full_texture.loadFromMemory(smb_characters_png, smb_characters_png_len))
            std::cerr << "Error loading the characters texture from memory" << std::endl;
        character_texture = sf::IntRect({0, 8}, {16, 16});
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape character(size);
        character.setPosition(pos);
        character.setTexture(&full_texture);
        character.setTextureRect(character_texture);
        window.draw(character);
    }

    void update(float elapsed) {
        if(invincibility_time > 0.f)
            invincibility_time -= elapsed;
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

    void get_power_up(int type) {
        if(type == 0) {
            if(character_state > 0) return;
            character_state = 1;
            pos.y -= 32.f;
            size.y = 64.f;
            character_texture = sf::IntRect({0, 32}, {16, 32});
        }else if(type == 1){
            if(character_state > 1) return;
            if(character_state == 0) {
                pos.y -= 32.f;
                size.y = 64.f;
            }
            character_state = 2;
            character_texture = sf::IntRect({0, 140}, {16, 32});
        }else
            std::cerr << "Error! Wrong power-up type integer value" << std::endl;
            
    }

    void hit() {
        if(invincibility_time > 0.f) return;

        if(character_state == 0)
            is_dead = true;
        else if(character_state == 1) {
            character_state = 0;
            size.y = 32.f;
            pos.y += 32.f;
            character_texture = sf::IntRect({0, 8}, {16, 16});
            invincibility_time = 2.f;
        }else if(character_state == 2){
            character_state = 1;
            character_texture = sf::IntRect({0, 32}, {16, 32});
            invincibility_time = 2.f;
        }
    }

    void restart() {
        pos = character_initial_pos;
        size = character_initial_size;
        old_pos = character_initial_pos;
        character_texture = sf::IntRect({0, 8}, {16, 16});
        speed = character_initial_speed;
        y_velocity = 0.f;
        invincibility_time = 0.f;
        is_jumping = false;
        is_dead = false;
        character_state = 0;
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
    int menu_selection;
    
    Menu() : menu_selection(0) {
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

        for(unsigned int i = 0; i < menu.size(); ++i)
            menu[i].draw(window);
    }

    void update() {
        for(int i = 0; i < 3; ++i) {
            if(i == menu_selection) {
                menu[i].button.setFillColor(sf::Color(150, 150, 150)); 
                menu[i].button.setOutlineThickness(3.f);
            }else {
                menu[i].button.setFillColor(sf::Color::Black);
                menu[i].button.setOutlineThickness(0.f);
            }
        }
    }
};

struct State {
    Level level;
    Character character;
    Menu menu;
    sf::View camera;
    bool is_paused, move_left, move_right, jump;

    State() : is_paused(true), move_left(false), move_right(false), jump(false) {
        camera.setSize({static_cast<float>(window_width), static_cast<float>(window_height)});
        camera.setCenter({window_width / 2.f, window_height / 2.f});
    }

    void draw(sf::RenderWindow& window) {
        window.setView(camera);
        level.draw(window);
        character.draw(window);

        window.setView(window.getDefaultView());
        if(is_paused)
            menu.draw(window);
    }

    void update(float elapsed) {
        if(is_paused){
            menu.update();
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
        character.update(elapsed);

        collisions();
        update_camera();
    }

    void update_camera() {
        float camera_x = character.pos.x + character.size.x / 2.f;

        if(camera_x < window_width / 2.f)
            camera_x = window_width / 2.f;
        else if(camera_x > level_width - window_width / 2.f)
            camera_x = level_width - window_width / 2.f;
            
        camera.setCenter({camera_x, window_height / 2.f});
    }

    bool overlap(sf::Vector2f pos1, sf::Vector2f size1, sf::Vector2f pos2, sf::Vector2f size2) {
        return pos1.x + size1.x > pos2.x &&
               pos1.x < pos2.x + size2.x &&
               pos1.y + size1.y > pos2.y &&
               pos1.y < pos2.y + size2.y;
    }

    void map_limits() {
        if(character.pos.x + character.size.x > level_width)
            character.pos.x = level_width - character.size.x;
        else if(character.pos.x < 0.f)
            character.pos.x = 0.f;
        
        bool is_on_ground = false;

        for (long unsigned int i = 0; i < level.blocks.size(); i++) {
            if(overlap(character.pos, character.size, level.blocks[i].pos, level.blocks[i].size)) {
                float b_left = level.blocks[i].pos.x;
                float b_right = level.blocks[i].pos.x + level.blocks[i].size.x;
                float b_top = level.blocks[i].pos.y;
                float b_bottom = level.blocks[i].pos.y + level.blocks[i].size.y;

                float old_left = character.old_pos.x;
                float old_right = character.old_pos.x + character.size.x;
                float old_top = character.old_pos.y;
                float old_bottom = character.old_pos.y + character.size.y;

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
        for (long unsigned int i = 0; i < level.powerups.size(); i++) {
            if(level.powerups[i].is_active){
                if(overlap(character.pos, character.size, level.powerups[i].pos, level.powerups[i].size)){
                    character.get_power_up(level.powerups[i].type);
                    level.powerups[i].is_active = false;
                }
            }
        }
    }

    void enemy_collision() {
        for (long unsigned int i = 0; i < level.enemies.size(); i++) {
            if(!level.enemies[i].is_alive) continue;
            if(overlap(character.pos, character.size, level.enemies[i].pos, level.enemies[i].size)) {
                if(level.enemies[i].type == 0) 
                    level.enemies[i].hit();
                else if(level.enemies[i].type == 1)
                    character.hit();
            }
        }
    }

    void collisions() {
        map_limits();
        powerup_collision();
        enemy_collision();
    }

    void restart() {
        character.restart();
        level.restart();
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
            gs.menu.menu_selection = (gs.menu.menu_selection + 1) % 3;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Up)
            gs.menu.menu_selection = (gs.menu.menu_selection - 1 + 3) % 3;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Enter) {
            if (gs.menu.menu_selection == 0)
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
    sf::RenderWindow window(sf::VideoMode ({window_width, window_height}), window_title);
    sf::Clock clock;
    State gs;

    window.setFramerateLimit(max_frame_rate);
    window.setMinimumSize(window.getSize());
    window.setKeyRepeatEnabled(false);
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
