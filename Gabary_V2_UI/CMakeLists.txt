cmake_minimum_required(VERSION 3.14)
project(GabaryV2UI CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include Header Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Import Prebuilt Gabary Core Static Library
add_library(GabaryCore STATIC IMPORTED)
set_target_properties(GabaryCore PROPERTIES 
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/build/libGabaryCore.a"
)

# Add FTXUI Library Subdirectory
# Ensure 'ftxui_src' or 'ftxui' directory exists in project root
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ftxui_src")
    add_subdirectory(ftxui_src)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ftxui")
    add_subdirectory(ftxui)
else()
    message(FATAL_ERROR "FTXUI directory not found! Please verify ftxui_src path.")
endif()

# Define Main TUI Executable
add_executable(gabary_tui main.cpp)

# Link Libraries to Executable
target_link_libraries(gabary_tui
    PRIVATE
        GabaryCore
        ftxui::screen
        ftxui::dom
        ftxui::component
)

