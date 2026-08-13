cmake_minimum_required(VERSION 3.11)
project(gabary_tui CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

file(GLOB_RECURSE ALL_HEADERS "*.h" "*.hpp")
set(INCLUDE_DIRS "")
foreach(HEADER ${ALL_HEADERS})
  get_filename_component(DIR ${HEADER} DIRECTORY)
  list(APPEND INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE ALL_SOURCES "Gabary_V2_UI/*.cpp")

set(SOURCES "")
foreach(SRC ${ALL_SOURCES})
  if(NOT SRC MATCHES "ftxui_src" AND NOT SRC MATCHES "test" AND NOT SRC MATCHES "build" AND NOT SRC MATCHES "console/main.cpp" AND NOT SRC MATCHES "src/main.cpp")
    list(APPEND SOURCES ${SRC})
  endif()
endforeach()

add_executable(gabary_tui ${SOURCES})

target_link_libraries(gabary_tui PRIVATE ftxui::screen ftxui::dom ftxui::component)
