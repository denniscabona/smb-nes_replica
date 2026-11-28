#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "../risorse/texture/textures.hpp"
#include "../risorse/font/fonts.hpp"

//////////////////////
// Initial defaults //
//////////////////////

// window
const char* window_title = "15_end_of_level";
const unsigned window_width = 800;
const unsigned window_height = 600;
const float max_frame_rate = 60;

// Generic
const sf::Vector2i small_sprite_size = {8, 8};
const sf::Vector2i normal_sprite_size = {16, 16};
const sf::Vector2i powerupped_sprite_size = {16, 32};
const sf::Vector2i large_sprite_size = {32, 32};
const sf::Vector2f generic_size = {32.f, 32.f};

// Level
const sf::Color overworld_background_color(146, 144, 255);
unsigned level_width = window_width;
unsigned level_height = window_height;
const float gravity = 1500.f;
const std::string level_path = "risorse/tilemap/livello1.txt";

// Powerup
const float mushroom_speed = 40.f;

// Enemy
const float goomba_speed = 40.f;
const sf::Vector2i plant_size = {16, 23};

// BackgroundElement
const sf::Vector2i small_bush_size = {32, 16};
const sf::Vector2i big_bush_size = {64, 16};
const sf::Vector2i small_cloud_size = {32, 24};
const sf::Vector2i big_cloud_size = {48, 24};
const sf::Vector2i small_hill_size = {48, 19};
const sf::Vector2i big_hill_size = {80, 34};

// FinishFlag
const sf::Vector2f pole_initial_pos = {level_width - 10.f*generic_size.x, level_height - 10.f*generic_size.y};
const sf::Vector2i g_pole_size = {32, 334};

// Character
sf::Vector2f character_initial_pos = {3.f, 0.f};
const float character_speed = 333.f;
const float character_jump_strength = 650.f;
const float character_invincibility_time = 2.f;

// Fireball
const float fireball_speed = 233.f;
const float fireball_bounce = -350.f;
const int max_number_of_fireballs = 3;

// Menu
const sf::Vector2f menu_button_size = {400.f, 50.f};



/////////////
// Structs //
/////////////

struct Block {
    sf::Vector2f pos, size;
    sf::IntRect block_texture;

    Block(const sf::Vector2f& pos) : pos(pos), size(generic_size), block_texture({229, 28}, normal_sprite_size) {}

    void draw(sf::RenderWindow& window, const sf::Texture& texture) {
        sf::RectangleShape block(size);
        block.setPosition(pos);
        block.setTexture(&texture);
        block.setTextureRect(block_texture);
        window.draw(block);
    }
};

struct Powerup {
    sf::Vector2f pos, size, initial_pos, old_pos;
    sf::IntRect power_up_texture;
    bool is_active;
    int type; // 0 = Mushroom, 1 = Fire Flower
    float speed, y_velocity;

    Powerup(sf::Vector2f initial_pos, int type) : pos(initial_pos), size(generic_size), initial_pos(initial_pos), old_pos(initial_pos), is_active(true), type(type), y_velocity(0.f) {
        if(type == 0) {
            power_up_texture = sf::IntRect({0, 8}, normal_sprite_size);
            speed = -mushroom_speed;
        }else if(type == 1) {
            power_up_texture = sf::IntRect({68, 8}, normal_sprite_size);
            speed = 0.f;
        }else 
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
        pos = initial_pos;
        old_pos = initial_pos;
        speed = -mushroom_speed;
        y_velocity = 0.f;
    }

    void update(float elapsed) {
        old_pos = pos;
        pos.x += speed*elapsed;
        y_velocity += gravity*elapsed;
        pos.y += y_velocity*elapsed;
    }
};

struct Enemy {
    sf::Vector2f pos, size, initial_pos, old_pos;
    sf::IntRect enemy_texture;
    bool is_alive;
    int type; // 0 = Goomba, 1 = Piranha plant
    float speed, y_velocity;
    
    Enemy(sf::Vector2f initial_pos, int type) : pos(initial_pos), size(generic_size), initial_pos(initial_pos), old_pos(initial_pos), is_alive(true), type(type), y_velocity(0.f) {
        if(type == 0) {
            enemy_texture = sf::IntRect({73, 300}, normal_sprite_size);
            speed = -goomba_speed;
        }else if(type == 1) {
            size.y = 2*plant_size.y;
            enemy_texture = sf::IntRect({161, 309}, plant_size);
            speed = 0.f;
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
        pos = initial_pos;
        old_pos = initial_pos;
        speed = -goomba_speed;
        y_velocity = 0.f;
    }

    void update(float elapsed) { 
        if(!is_alive) return;
        old_pos = pos;
        pos.x += speed*elapsed;
        y_velocity += gravity*elapsed;
        pos.y += y_velocity*elapsed;
    }
};

struct BackgroundElements {
    sf::Vector2f pos, size;
    sf::IntRect element_texture;
    int type; // 0 = Small bush, 1 = Big Bush, 2 = Small Cloud, 3 = Big Cloud, 4 = Small Hill, 5 = Big Hill
    float size_mult;
    
    BackgroundElements(sf::Vector2f pos, int type) : pos(pos), type(type), size_mult(2.f) { 
        switch (type) {
        case 0:
            element_texture = sf::IntRect({208, 704}, small_bush_size);
            break;
        case 1:
            element_texture = sf::IntRect({120, 704}, big_bush_size);
            break;
        case 2:
            element_texture = sf::IntRect({352, 632}, small_cloud_size);
            break;
        case 3:
            element_texture = sf::IntRect({624, 608}, big_cloud_size);
            break;
        case 4:
            element_texture = sf::IntRect({200, 757}, small_hill_size);
            size_mult = 4.f;
            break;
        case 5:
            element_texture = sf::IntRect({112, 741}, big_hill_size);
            size_mult = 4.f;
            break;  
        default:
            break;
        }

        size = sf::Vector2f({size_mult*element_texture.size.x, size_mult*element_texture.size.y});

        if (type == 4 || type == 5) {
            this->pos.x -= (size.x - generic_size.x) / 2.f; 
            this->pos.y -= (size.y - generic_size.y);
        }
    }

    void draw(sf::RenderWindow& window, const sf::Texture& texture) {
        sf::RectangleShape element(size);
        element.setPosition(pos);
        element.setTexture(&texture);
        element.setTextureRect(element_texture);
        window.draw(element);
    }
};

struct FinishFlag {
    sf::Vector2f pole_pos, pole_size, flag_pos, flag_initial_pos, flag_size;
    sf::Texture pole_texture, flag_texture;

    FinishFlag() : pole_pos(pole_initial_pos), pole_size(g_pole_size), flag_pos(pole_pos), flag_size(generic_size) {
        if(!pole_texture.loadFromMemory(smb_background_elements_png, smb_background_elements_png_len, false, sf::IntRect({0, 608}, {g_pole_size.x / 2, g_pole_size.y / 2})))
            std::cerr << "Error loading the flag-pole texture from memory" << std::endl;
        if(!flag_texture.loadFromMemory(smb_items_png, smb_items_png_len, false, sf::IntRect({92, 90}, normal_sprite_size)))
            std::cerr << "Error loading the flag texture from memory" << std::endl;
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape pole(pole_size);
        pole.setPosition(pole_pos);
        pole.setTexture(&pole_texture);
        window.draw(pole);
        sf::RectangleShape flag(flag_size);
        flag.setPosition(flag_pos);
        flag.setTexture(&flag_texture);
        window.draw(flag);
    }

    void restart() {
        flag_pos = flag_initial_pos;
    }
};

struct Level {
    std::vector<Block> blocks;
    std::vector<Powerup> powerups;
    std::vector<Enemy> enemies;
    std::vector<BackgroundElements> background_elements;
    FinishFlag flag;
    sf::Texture ground_and_enemy_texture, power_up_texture, background_elements_texture;

    Level() {
        if(!ground_and_enemy_texture.loadFromMemory(smb_map_png, smb_map_png_len))
            std::cerr << "Error loading the map texture from memory" << std::endl;
        if(!power_up_texture.loadFromMemory(smb_items_png, smb_items_png_len))
            std::cerr << "Error loading the items texture from memory" << std::endl;
        if(!background_elements_texture.loadFromMemory(smb_background_elements_png, smb_background_elements_png_len))
            std::cerr << "Error loading the background elements texture from memory" << std::endl;

        load_level();
    }
    
    void draw(sf::RenderWindow& window) {
        for (long unsigned int i = 0; i < background_elements.size(); i++)
            background_elements[i].draw(window, background_elements_texture);     
        for (long unsigned int i = 0; i < blocks.size(); i++)
            blocks[i].draw(window, ground_and_enemy_texture);
        for (long unsigned int i = 0; i < powerups.size(); i++)
            powerups[i].draw(window, power_up_texture);
        for (long unsigned int i = 0; i < enemies.size(); i++)
            enemies[i].draw(window, ground_and_enemy_texture);
        flag.draw(window);       
    }

    void restart() {
        for (long unsigned int i = 0; i < powerups.size(); i++)
            powerups[i].restart();
        for (long unsigned int i = 0; i < enemies.size(); i++)
            enemies[i].restart();
        flag.restart();
    }

    void load_level() {
        std::ifstream level(level_path);
        
        char c;
        int row = 0, col = 0, tmp_level_size_x = 0;

        while(level.get(c)) {
            if(c == '\r') continue;
            if(c == '\n') {
                if(tmp_level_size_x < col) 
                    tmp_level_size_x = col;
                row++;
                col = 0;
                continue;
            }
            
            sf::Vector2f initial_pos = {col * generic_size.x, row * generic_size.y};

            switch (c) {
            case 'B':
                blocks.push_back(Block(initial_pos));
                break;
            case 'C':
                character_initial_pos = initial_pos;
                break;                
            case 'M':
                powerups.push_back(Powerup(initial_pos, 0));
                break;
            case 'F':
                powerups.push_back(Powerup(initial_pos, 1));
                break;
            case 'G':
                enemies.push_back(Enemy(initial_pos, 0));
                break;
            case 'P':
                enemies.push_back(Enemy({initial_pos.x, initial_pos.y - (2*plant_size.y - generic_size.y)}, 1));
                break;
            case 'E':
                flag.pole_pos = {initial_pos.x, initial_pos.y - flag.pole_size.y + 2.f*generic_size.y};
                flag.flag_initial_pos = {flag.pole_pos.x - 16.f, flag.pole_pos.y + 16.f};
                flag.flag_pos = flag.flag_initial_pos;
                break;
            case '0':
                background_elements.push_back(BackgroundElements(initial_pos, 0));
                break;             
            case '1':
                background_elements.push_back(BackgroundElements(initial_pos, 1));
                break;
            case '2':
                background_elements.push_back(BackgroundElements(initial_pos, 2));
                break;
            case '3':
                background_elements.push_back(BackgroundElements(initial_pos, 3));
                break;
            case '4':
                background_elements.push_back(BackgroundElements(initial_pos, 4));
                break; 
            case '5':
                background_elements.push_back(BackgroundElements(initial_pos, 5));
                break;                      
            default:
                break;
            }

            col++;

        }

        if (tmp_level_size_x < col) tmp_level_size_x = col;
        int tmp_level_size_y = row;
        if(col > 0)
            tmp_level_size_y++;

        level_width = tmp_level_size_x * generic_size.x;
        level_height = tmp_level_size_y * generic_size.y;
    }
};

struct Fireball {
    sf::Vector2f pos, size, old_pos;
    float speed, y_velocity;
    bool is_active;

    Fireball(sf::Vector2f pos, int direction) : pos(pos), size(normal_sprite_size), old_pos(pos), y_velocity(0.f), is_active(true) {
        if(direction == 1)
            speed = fireball_speed;
        else if(direction == -1)
            speed = -fireball_speed;
        else
            std::cerr << "Error! Wrong direction integer value" << std::endl;
    }

    void draw(sf::RenderWindow& window, const sf::Texture& texture) {
        if(!is_active) return;
        sf::RectangleShape fireball(size);
        fireball.setPosition(pos);
        fireball.setTexture(&texture);
        fireball.setTextureRect(sf::IntRect({180, 54}, small_sprite_size));
        window.draw(fireball);
    }

    void update(float elapsed) {
        old_pos = pos;
        pos.x += speed*elapsed;
        y_velocity += gravity*elapsed;
        pos.y += y_velocity*elapsed;
    }

    void bounce() {
        y_velocity = fireball_bounce;
    }

};

struct Character {
    sf::Vector2f pos, size, old_pos;
    sf::Texture full_texture;
    sf::IntRect character_texture;   
    float speed, y_velocity, invincibility_time;
    bool is_jumping, is_dead;
    int character_state, direction; // 0 = Small Mario, 1 = Big Mario, 2 = Fire Mario     // -1 = Left, 1 = Right

    Character() : pos(character_initial_pos), size(generic_size), old_pos(character_initial_pos), speed(character_speed), y_velocity(0.f), invincibility_time(0), is_jumping(false), is_dead(false), character_state(0), direction(1) {
        if(!full_texture.loadFromMemory(smb_characters_png, smb_characters_png_len))
            std::cerr << "Error loading the characters texture from memory" << std::endl;
        character_texture = sf::IntRect({0, 8}, normal_sprite_size);
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
        direction = -1;
    }

    void move_right(float elapsed) {
        pos.x += speed*elapsed;
        direction = 1;
    }

    void jump() { 
        if(!is_jumping) {
            y_velocity = -character_jump_strength;
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
            size.y = 2*generic_size.y;
            character_texture = sf::IntRect({0, 32}, powerupped_sprite_size);
        }else if(type == 1) {
            if(character_state > 1) return;
            if(character_state == 0) {
                pos.y -= 32.f;
                size.y = 2*generic_size.y;
            }
            character_state = 2;
            character_texture = sf::IntRect({0, 140}, powerupped_sprite_size);
        }else
            std::cerr << "Error! Wrong power-up type integer value" << std::endl;
            
    }

    void hit() {
        if(invincibility_time > 0.f) return;

        if(character_state == 0)
            is_dead = true;
        else if(character_state == 1) {
            character_state = 0;
            size.y = generic_size.y;
            pos.y += generic_size.y;
            character_texture = sf::IntRect({0, 8}, normal_sprite_size);
            invincibility_time = character_invincibility_time;
        }else if(character_state == 2){
            character_state = 1;
            character_texture = sf::IntRect({0, 32}, powerupped_sprite_size);
            invincibility_time = character_invincibility_time;
        }
    }

    void restart() {
        pos = character_initial_pos;
        size = generic_size;
        old_pos = character_initial_pos;
        character_texture = sf::IntRect({0, 8}, normal_sprite_size);
        speed = character_speed;
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
    int menu_selection, type; // 0 = Continue, 1 = Restart
    
    Menu() : menu_selection(0), type(0) {
        if(!font.openFromMemory(smb_font_ttf, smb_font_ttf_len))
            std::cerr << "Error loading font from memory" << std::endl;

        menu.push_back(Button({200.f, 200.f}, menu_button_size, font, "RIPRENDI"));
        menu.push_back(Button({200.f, 280.f}, menu_button_size, font, "CAMBIA LIVELLO"));
        menu.push_back(Button({200.f, 360.f}, menu_button_size, font, "VEDI COMANDI"));
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape overlay({window_width, window_height});
        overlay.setFillColor(sf::Color(0, 0, 0, 150)); 
        window.draw(overlay);

        for(unsigned int i = 0; i < menu.size(); ++i)
            menu[i].draw(window);
    }

    void update() {
        if(type == 0)
            menu[0] = Button({200.f, 200.f}, menu_button_size, font, "RIPRENDI");
        else if(type == 1)
            menu[0] = Button({200.f, 200.f}, menu_button_size, font, "RIGIOCA");
        else
            std::cerr << "Error! Wrong button type integer value " << std::endl;

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

    void restart() {
        menu_selection = 0;
        type = 0;
    }
};

struct State {
    Level level;
    Character character;
    Menu menu;
    std::vector<Fireball> fireballs;
    sf::View camera;
    bool is_paused, move_left, move_right, jump, is_finished;

    State() : is_paused(true), move_left(false), move_right(false), jump(false), is_finished(false) {
        camera.setSize({static_cast<float>(window_width), static_cast<float>(window_height)});
        camera.setCenter({window_width / 2.f, window_height / 2.f});
    }

    void draw(sf::RenderWindow& window) {
        window.setView(camera);
        level.draw(window);
        character.draw(window);

        for (long unsigned int i = 0; i < fireballs.size(); i++)
            fireballs[i].draw(window, level.power_up_texture);      

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

        for (long unsigned int i = 0; i < level.enemies.size(); i++)
            if(level.enemies[i].is_alive && level.enemies[i].type == 0)
                level.enemies[i].update(elapsed);

        
        for (long unsigned int i = 0; i < level.powerups.size(); i++)
            if(level.powerups[i].is_active && level.powerups[i].type == 0)
                level.powerups[i].update(elapsed); 

        for (int i = static_cast<int>(fireballs.size()) - 1; i >= 0; i--)
            if (fireballs[i].is_active)
                fireballs[i].update(elapsed);
            else 
                fireballs.erase(fireballs.begin() + i);
        
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

    bool x_limit_outside_level(sf::Vector2f pos, sf::Vector2f size) {
        return pos.x + size.x < 0.f || pos.x > level_width;
    }

    bool y_limit_outside_level(sf::Vector2f pos) {
        return pos.y > window_height + 1000.f;
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

        if(y_limit_outside_level(character.pos))
            character.is_dead = true;
    }

    void powerup_collision() {
        for (long unsigned int i = 0; i < level.powerups.size(); i++) {
            if(level.powerups[i].is_active){
                if(overlap(character.pos, character.size, level.powerups[i].pos, level.powerups[i].size)){
                    character.get_power_up(level.powerups[i].type);
                    level.powerups[i].is_active = false;
                }
                if(level.powerups[i].type == 0) {
                    for (long unsigned int j = 0; j < level.blocks.size(); j++) {
                        if(overlap(level.powerups[i].pos, level.powerups[i].size, level.blocks[j].pos, level.blocks[j].size)) {
                            float b_left = level.blocks[j].pos.x;
                            float b_right = level.blocks[j].pos.x + level.blocks[j].size.x;
                            float b_top = level.blocks[j].pos.y;

                            float old_left = level.powerups[i].old_pos.x;
                            float old_right = level.powerups[i].old_pos.x + level.powerups[i].size.x;
                            float old_bottom = level.powerups[i].old_pos.y + level.powerups[i].size.y;
                            
                            if(old_bottom <= b_top) {
                                if (level.powerups[i].y_velocity >= 0.f) {
                                    level.powerups[i].pos.y = b_top - level.powerups[i].size.y;
                                    level.powerups[i].y_velocity = 0.f;
                                }
                            }else {
                                if(old_right <= b_left) {
                                    level.powerups[i].pos.x = b_left - level.powerups[i].size.x;
                                    level.powerups[i].speed = -mushroom_speed;
                                }else if(old_left >= b_right) {
                                    level.powerups[i].pos.x = b_right;
                                    level.powerups[i].speed = mushroom_speed;
                                }
                            }
                        }
                    }
                }

                if(x_limit_outside_level(level.powerups[i].pos, level.powerups[i].size) && level.powerups[i].type == 0)
                    level.powerups[i].is_active = false;
                if(y_limit_outside_level(level.powerups[i].pos) && level.powerups[i].type == 0)
                    level.powerups[i].is_active = false;
            }
        }
    }

    void enemy_collision() {
        for (long unsigned int i = 0; i < level.enemies.size(); i++) {
            if(level.enemies[i].is_alive) {
                if(overlap(character.pos, character.size, level.enemies[i].pos, level.enemies[i].size)) {
                    if(level.enemies[i].type == 0) {
                        float e_old_top = level.enemies[i].pos.y;

                        float c_old_bottom = character.old_pos.y + character.size.y;
                        
                        if(c_old_bottom <= e_old_top && character.y_velocity > 0.f) {
                            level.enemies[i].hit();
                            character.pos.y = level.enemies[i].pos.y - character.size.y;
                            character.y_velocity = -character_jump_strength;
                            character.is_jumping = true;
                        }else 
                            character.hit();                        

                    }else if(level.enemies[i].type == 1)
                        character.hit();
                }
                if(level.enemies[i].type == 0) {
                    for (long unsigned int j = 0; j < level.blocks.size(); j++) {
                        if(overlap(level.enemies[i].pos, level.enemies[i].size, level.blocks[j].pos, level.blocks[j].size)) {
                            float b_left = level.blocks[j].pos.x;
                            float b_right = level.blocks[j].pos.x + level.blocks[j].size.x;
                            float b_top = level.blocks[j].pos.y;

                            float old_left = level.enemies[i].old_pos.x;
                            float old_right = level.enemies[i].old_pos.x + level.enemies[i].size.x;
                            float old_bottom = level.enemies[i].old_pos.y + level.enemies[i].size.y;
                            
                            if(old_bottom <= b_top) {
                                if (level.enemies[i].y_velocity >= 0.f) {
                                    level.enemies[i].pos.y = b_top - level.enemies[i].size.y;
                                    level.enemies[i].y_velocity = 0.f;
                                }
                            }else {
                                if(old_right <= b_left) {
                                    level.enemies[i].pos.x = b_left - level.enemies[i].size.x;
                                    level.enemies[i].speed = -goomba_speed;
                                }else if(old_left >= b_right) {
                                    level.enemies[i].pos.x = b_right;
                                    level.enemies[i].speed = goomba_speed;
                                }
                            }
                        }
                    }
                }

                for (long unsigned int j =  i + 1; j < level.enemies.size(); j++) {
                    if(level.enemies[j].is_alive) {
                        if(overlap(level.enemies[i].pos, level.enemies[i].size, level.enemies[j].pos, level.enemies[j].size)) {                     
                            if(level.enemies[i].type == 0 && level.enemies[j].type == 0) {
                                if(level.enemies[i].pos.x < level.enemies[j].pos.x) {
                                    level.enemies[i].pos.x = level.enemies[j].pos.x - level.enemies[i].size.x;
                                    level.enemies[i].speed = -goomba_speed;
                                    level.enemies[j].speed = goomba_speed;
                                }else {
                                    level.enemies[j].pos.x = level.enemies[i].pos.x - level.enemies[j].size.x;
                                    level.enemies[j].speed = -goomba_speed;
                                    level.enemies[i].speed = goomba_speed;
                                }
                            }else if(level.enemies[i].type == 0 && level.enemies[j].type == 1) {
                                if(level.enemies[i].pos.x < level.enemies[j].pos.x) {
                                    level.enemies[i].pos.x = level.enemies[j].pos.x - level.enemies[i].size.x;
                                    level.enemies[i].speed = -goomba_speed;
                                }else {
                                    level.enemies[i].pos.x = level.enemies[j].pos.x + level.enemies[j].size.x;
                                    level.enemies[i].speed = goomba_speed;
                                } 
                            }else if(level.enemies[i].type == 1 && level.enemies[j].type == 0) {
                                if(level.enemies[j].pos.x < level.enemies[i].pos.x) {
                                    level.enemies[j].pos.x = level.enemies[i].pos.x - level.enemies[j].size.x;
                                    level.enemies[j].speed = -goomba_speed;
                                }else {
                                    level.enemies[j].pos.x = level.enemies[i].pos.x + level.enemies[i].size.x;
                                    level.enemies[j].speed = goomba_speed;
                                }  
                            }                    
                        }
                    }
                }

                if(x_limit_outside_level(level.enemies[i].pos, level.enemies[i].size) && level.enemies[i].type == 0)
                    level.enemies[i].is_alive = false;
                if(y_limit_outside_level(level.enemies[i].pos) && level.enemies[i].type == 0)
                    level.enemies[i].is_alive = false;
            }
        }
    }

    void fireball_collsions() {
        float cam_left = camera.getCenter().x - window_width / 2.f;
        float cam_right = camera.getCenter().x + window_width / 2.f;

        for (long unsigned int i = 0; i < fireballs.size(); i++) {

            if(fireballs[i].pos.x + fireballs[i].size.x > cam_right + 50.f || fireballs[i].pos.x < cam_left - 50.f || fireballs[i].pos.y > window_height + 100.f) {
                fireballs[i].is_active = false;
                continue;
            }

            for (long unsigned int j = 0; j < level.blocks.size(); j++) {
                if(overlap(fireballs[i].pos, fireballs[i].size, level.blocks[j].pos, level.blocks[j].size)) {
                    float b_top = level.blocks[j].pos.y;

                    float old_bottom = fireballs[i].old_pos.y + fireballs[i].size.y;

                    if(old_bottom <= b_top) {
                        if (fireballs[i].y_velocity >= 0.f) {
                            fireballs[i].pos.y = b_top - fireballs[i].size.y;
                            fireballs[i].bounce();
                        }
                    }else 
                        fireballs[i].is_active = false;
                }
            }
    
            if(!fireballs[i].is_active) continue;
            for (long unsigned int j = 0; j < level.enemies.size(); j++) {
                if (!level.enemies[j].is_alive) continue;
                if (overlap(fireballs[i].pos, fireballs[i].size, level.enemies[j].pos, level.enemies[j].size)) {
                    level.enemies[j].hit();
                    fireballs[i].is_active = false;
                    break;
                }
            }
        }

    }

    void flag_collisions() {
        if(overlap(character.pos, character.size, level.flag.pole_pos, level.flag.pole_size)) {
            character.pos.x = level.flag.pole_pos.x;              
            character.pos.y = character.old_pos.y;
            is_finished = true;
        }
    }        

    void shoot() {
        if(character.character_state == 2 && fireballs.size() < max_number_of_fireballs) {
            if(character.direction == 1)
                fireballs.push_back(Fireball({character.pos.x + character.size.x, character.pos.y + character.size.y / 2.f}, 1));
            else if(character.direction == -1)
                fireballs.push_back(Fireball({character.pos.x - 16.f, character.pos.y + character.size.y / 2.f}, -1));
        }
    }

    void collisions() {
        map_limits();
        powerup_collision();
        enemy_collision();
        fireball_collsions();
        flag_collisions();
    }

    void restart() {
        character.restart();
        level.restart();
        menu.restart();
        fireballs.clear();
        is_paused = false;
        move_left = false; 
        move_right = false; 
        jump = false;
        is_finished = false;
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
    if(key_pressed.scancode == sf::Keyboard::Scancode::Escape){
        if(gs.is_finished) 
            gs.menu.type = 1;
        else
            gs.menu.type = 0;
        gs.is_paused = !gs.is_paused;
    }
    else if (gs.is_paused){
        if(key_pressed.scancode == sf::Keyboard::Scancode::Down)
            gs.menu.menu_selection = (gs.menu.menu_selection + 1) % 3;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Up)
            gs.menu.menu_selection = (gs.menu.menu_selection - 1 + 3) % 3;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Enter) {
            if(gs.menu.menu_selection == 0 && gs.menu.type == 0)
                gs.is_paused = false;
            else if(gs.menu.menu_selection == 0 && gs.menu.type == 1)
                gs.restart();
        }
    } 
    else{
        if(key_pressed.scancode == sf::Keyboard::Scancode::Left || key_pressed.scancode == sf::Keyboard::Scancode::A)
            gs.move_left = true;
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Right || key_pressed.scancode == sf::Keyboard::Scancode::D)
            gs.move_right = true;  
        else if(key_pressed.scancode == sf::Keyboard::Scancode::Up || key_pressed.scancode == sf::Keyboard::Scancode::W)
            gs.jump = true;
        else if(key_pressed.scancode == sf::Keyboard::Scancode::E)
            gs.shoot();
    }
}   

void handle(const sf::Event::KeyReleased& key_released, State &gs){
    if(key_released.scancode == sf::Keyboard::Scancode::Left || key_released.scancode == sf::Keyboard::Scancode::A)
        gs.move_left = false;
    else if(key_released.scancode == sf::Keyboard::Scancode::Right || key_released.scancode == sf::Keyboard::Scancode::D)
        gs.move_right = false;  
    else if(key_released.scancode == sf::Keyboard::Scancode::Up || key_released.scancode == sf::Keyboard::Scancode::W)
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
