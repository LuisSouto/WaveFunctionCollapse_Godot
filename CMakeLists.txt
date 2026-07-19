cmake_minimum_required(VERSION 3.17)
project(GodotWFC LANGUAGES CXX)

# --- 1. CONFIGURATION ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBNAME "wavefunctioncollapse")
set(GODOT_PROJECT_DIR "project" CACHE STRING "Directory of the Godot project")

# --- 2. DEPENDENCIES & SUBMODULES ---
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

# --- 3. TARGET DEFINITION ---
add_library(${LIBNAME} SHARED)

target_sources(${LIBNAME} PRIVATE
    src/register_types.cpp
    src/example_class.cpp
)

# Link bindings and your static WFC solver
target_link_libraries(${LIBNAME} 
    PRIVATE 
    godot-cpp 
    WaveFunctionCollapse
)

# --- 4. GODOT BINDING HELPERS ---
list(APPEND CMAKE_MODULE_PATH "${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)
get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot-cpp GODOTCPP_PLATFORM)

# Path to the Godot project's binary folder
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${GODOT_PROJECT_DIR}/bin/${GODOTCPP_PLATFORM}")

# --- 5. OUTPUT & PROPERTIES ---
set_target_properties(${LIBNAME} PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${LIBNAME}${GODOTCPP_SUFFIX}"
    
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${OUTPUT_DIR}"
)

# --- 6. PERFORMANCE (LTO/IPO) ---
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(${LIBNAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    # This ensures your solver submodule also gets optimized into the main plugin
    set_target_properties(WaveFunctionCollapse PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "LTO/IPO requested for non-debug build.")
endif()

# --- 7. DOCUMENTATION (Optional) ---
file(GLOB_RECURSE DOC_XML CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/doc_classes/*.xml")
if(DOC_XML AND GODOTCPP_TARGET MATCHES "editor|template_debug")
    target_doc_sources(${LIBNAME} "${DOC_XML}")
endif()