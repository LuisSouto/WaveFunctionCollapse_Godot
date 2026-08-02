cmake_minimum_required(VERSION 3.17)
project(GodotWFC LANGUAGES CXX)

# Configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBNAME "wavefunctioncollapse")
set(GODOT_PROJECT_DIR "project" CACHE STRING "Directory of the Godot project")

# Dependencies
find_package(Python3 3.4 REQUIRED)
find_program(GIT git REQUIRED)

# Helper to auto-update submodules if they are missing
macro(check_submodule PATH NAME)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PATH}/CMakeLists.txt")
        message(STATUS "Initializing submodule: ${NAME}")
        execute_process(
            COMMAND ${GIT} submodule update --init --recursive ${PATH}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND_ERROR_IS_FATAL ANY
        )
    endif()
endmacro()

check_submodule("godot-cpp" "Godot CPP Bindings")
check_submodule("wfc_solver" "WFC Core Solver")

# Add submodules
add_subdirectory(godot-cpp SYSTEM)
add_subdirectory(wfc_solver)

# Target definition
add_library(${LIBNAME} SHARED)

target_sources(${LIBNAME} PRIVATE
    src/register_types.cpp
    src/example_class.cpp
    src/wfc_config.cpp
    src/wfc_wrapper.cpp
)

target_include_directories(${LIBNAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link bindings
target_link_libraries(${LIBNAME} 
    PRIVATE 
    godot-cpp 
    WaveFunctionCollapse
)

# Godot binding helpers
list(APPEND CMAKE_MODULE_PATH "${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)
get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot-cpp GODOTCPP_PLATFORM)

# Documentation
file(GLOB_RECURSE DOC_XML LIST_DIRECTORIES NO CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/doc_classes/*.xml")
if(DOC_XML)
    target_doc_sources(${LIBNAME} "${DOC_XML}")
endif()

# Path to the Godot project's binary folder
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${GODOT_PROJECT_DIR}/bin/${GODOTCPP_PLATFORM}")

# Output & properties
set_target_properties(${LIBNAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${LIBNAME}${GODOTCPP_SUFFIX}"
    
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${OUTPUT_DIR}"
)

# Performance (LTO/IPO)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(${LIBNAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_target_properties(WaveFunctionCollapse PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "LTO/IPO enabled for Release build.")
else()
    message(STATUS "Build type is ${CMAKE_BUILD_TYPE}, skipping LTO.")
endif()