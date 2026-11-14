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

# 01b_texture_files

# 01c_texture_embedded

# 02a_move_ball

# 02b_startpause

# 02c_move_paddle

# 03a_field_limits

# 03b_robust_field_limits

# 03c_basic_strike

# 03d_variable_strike

# 03e_paddle_edges

# 04a_blocks

# 04b_hits
