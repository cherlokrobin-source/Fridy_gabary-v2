cmake_minimum_required(VERSION 3.10)

add_executable(gabary_tui main.cpp)

target_include_directories(gabary_tui PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(gabary_tui PRIVATE
    ftxui-screen
    ftxui-dom
    ftxui-component
    pthread
)
