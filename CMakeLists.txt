cmake_minimum_required(VERSION 3.14)
project(FridyGabaryV2 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/report
)

set(CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ArchiveEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ChronologyEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GabaryDualCalendarEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GlobalSolarDay.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/GlobalTemporalID.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LeapYearRules.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LongTimeEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LunarEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LunarEngineV2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarArchiveBuilder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarCycleArchive.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarCycleEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarEngineV2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarTimeEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarTimelineArchive.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/SolarTimelineGenerator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemporalCoordinate.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemporalCoordinateEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemporalEngineV2.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemporalQueryEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemporalReportEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemporalReportJSON.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TimeEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WeekCycleEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/WeekEngine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cycle_reference_compare.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine_cycle_compare.cpp"
)

add_library(fridey_core STATIC ${CORE_SOURCES})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Gabary_V2_UI")
    add_subdirectory(Gabary_V2_UI)
endif()
