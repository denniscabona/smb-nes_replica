cmake_minimum_required(VERSION 4.0)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Minimum policy version for all subprojects")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_library(common INTERFACE)
target_compile_features(common INTERFACE cxx_std_17)
target_compile_options(common INTERFACE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall>
  $<$<CXX_COMPILER_ID:MSVC>: /W4>
)



# 01_basic_structure
SET (tappa01 "01_basic_structure")
add_executable(${tappa01} ${tappa01}/main.cc)
target_link_libraries(${tappa01} PRIVATE SFML::Graphics common)

# 02_character_movement
SET (tappa02 "02_character_movement")
add_executable(${tappa02} ${tappa02}/main.cc)
target_link_libraries(${tappa02} PRIVATE SFML::Graphics common)

# 03_pause_menu
SET (tappa03 "03_pause_menu")
add_executable(${tappa03} ${tappa03}/main.cc)
target_link_libraries(${tappa03} PRIVATE SFML::Graphics common)

# 04_power_up_introduction
SET (tappa04 "04_power_up_introduction")
add_executable(${tappa04} ${tappa04}/main.cc)
target_link_libraries(${tappa04} PRIVATE SFML::Graphics common)

# 05_damage_introduction
SET (tappa05 "05_damage_introduction")
add_executable(${tappa05} ${tappa05}/main.cc)
target_link_libraries(${tappa05} PRIVATE SFML::Graphics common)

# 06_better_ground_and_collisions
SET (tappa06 "06_better_ground_and_collisions")
add_executable(${tappa06} ${tappa06}/main.cc)
target_link_libraries(${tappa06} PRIVATE SFML::Graphics common)

# 07_power_up_and_enemy_improvement
SET (tappa07 "07_power_up_and_enemy_improvement")
add_executable(${tappa07} ${tappa07}/main.cc)
target_link_libraries(${tappa07} PRIVATE SFML::Graphics common)

# 08_struct_system_improvement
SET (tappa08 "08_struct_system_improvement")
add_executable(${tappa08} ${tappa08}/main.cc)
target_link_libraries(${tappa08} PRIVATE SFML::Graphics common)

# 09_camera_scrolling
SET (tappa09 "09_camera_scrolling")
add_executable(${tappa09} ${tappa09}/main.cc)
target_link_libraries(${tappa09} PRIVATE SFML::Graphics common)

# 10_enemy_movement
SET (tappa10 "10_enemy_movement")
add_executable(${tappa10} ${tappa10}/main.cc)
target_link_libraries(${tappa10} PRIVATE SFML::Graphics common)

# 11_power_up_completion
SET (tappa11 "11_power_up_completion")
add_executable(${tappa11} ${tappa11}/main.cc)
target_link_libraries(${tappa11} PRIVATE SFML::Graphics common)

# 12_enemies_completion
SET (tappa12 "12_enemies_completion")
add_executable(${tappa12} ${tappa12}/main.cc)
target_link_libraries(${tappa12} PRIVATE SFML::Graphics common)