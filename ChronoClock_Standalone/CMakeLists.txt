cmake_minimum_required(VERSION 3.14)
project(ChronoClockStandalone CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# إضافة المجلد المحلي المنسوخ
add_subdirectory(ftxui_lib EXCLUDE_FROM_ALL)

# المكون التنفيذي
add_executable(chrono_clock clock_main.cpp)

# ربط مكتبات FTXUI
target_link_libraries(chrono_clock PRIVATE 
    ftxui::screen 
    ftxui::dom 
    ftxui::component
)
