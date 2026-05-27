cmake_minimum_required(VERSION 3.20)
project(ndisplay-gen LANGUAGES CXX)

if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# nlohmann/json
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

# ndisplay_gen library
add_library(ndisplay_gen STATIC
    src/serialize.cpp
)
target_link_libraries(ndisplay_gen PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(ndisplay_gen PUBLIC src)

# Catch2 (test only)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(Catch2)

add_executable(test_ndisplay
    tests/test_json_compare.cpp
    tests/test_basic.cpp
    tests/test_golden.cpp
    tests/deserialize.cpp
)
target_link_libraries(test_ndisplay PRIVATE ndisplay_gen Catch2::Catch2WithMain)
target_compile_definitions(test_ndisplay PRIVATE NDISPLAY_PROJECT_DIR="${CMAKE_SOURCE_DIR}")

# CTest + Catch2 discovery (VS Code / CMake Tools test UI lists each TEST_CASE)
include(CTest)
list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(test_ndisplay)

# Examples (optional)
add_executable(hello_wall examples/hello_wall.cpp)
target_link_libraries(hello_wall PRIVATE ndisplay_gen)
set_target_properties(hello_wall PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_executable(twin_wall examples/twin_wall.cpp)
target_link_libraries(twin_wall PRIVATE ndisplay_gen)
set_target_properties(twin_wall PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_executable(cube_room examples/cube_room.cpp)
target_link_libraries(cube_room PRIVATE ndisplay_gen)
set_target_properties(cube_room PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_executable(cube_room_cluster examples/cube_room_cluster.cpp)
target_link_libraries(cube_room_cluster PRIVATE ndisplay_gen)
set_target_properties(cube_room_cluster PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_executable(hello_renderstream examples/hello_renderstream.cpp)
target_link_libraries(hello_renderstream PRIVATE ndisplay_gen)
set_target_properties(hello_renderstream PROPERTIES EXCLUDE_FROM_ALL TRUE)
