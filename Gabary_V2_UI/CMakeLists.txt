cmake_minimum_required(VERSION 3.14)

# إضافة FTXUI كمشروع فرعي محلي (مضمون ومتوافق 100% مع Termux)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/ftxui ${CMAKE_CURRENT_BINARY_DIR}/ftxui_build EXCLUDE_FROM_ALL)

add_executable(gabary_tui main.cpp)

target_include_directories(gabary_tui PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(gabary_tui PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    pthread
)
