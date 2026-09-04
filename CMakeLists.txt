cmake_minimum_required(VERSION 3.10)
project(Gabary_V2 VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# تفعيل CTest للاختبارات الحسابية
enable_testing()

include_directories(include)

# هدف الاختبارات الحسابية (تجميع محلي سليم وسريع)
add_executable(unit_tests test_chronology.cpp)
add_test(NAME ChronologyEngineTests COMMAND unit_tests)

# تضمين الواجهة فقط إذا كانت تعتمد بناءً محلياً
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Gabary_V2_UI/CMakeLists.txt")
    add_subdirectory(Gabary_V2_UI)
endif()
