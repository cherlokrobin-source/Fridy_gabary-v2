cmake_minimum_required(VERSION 3.14)
project(GabaryV2UI CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ftxui_src")
    add_subdirectory(ftxui_src)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ftxui")
    add_subdirectory(ftxui)
endif()

add_executable(gabary_tui
    main.cpp
)

target_link_libraries(gabary_tui
    PRIVATE
        fridey_core
        ftxui::screen
        ftxui::dom
        ftxui::component
)
