cmake_minimum_required(VERSION 3.10)
project(Warzone VERSION 1.0)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include src directory
include_directories(${PROJECT_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE CARDS_SOURCES "${PROJECT_SOURCE_DIR}/src/Cards/*.cpp")
file(GLOB_RECURSE GAME_ENGINE_SOURCES "${PROJECT_SOURCE_DIR}/src/GameEngine/*.cpp")
file(GLOB_RECURSE MAP_SOURCES "${PROJECT_SOURCE_DIR}/src/Map/*.cpp")
file(GLOB_RECURSE ORDERS_SOURCES "${PROJECT_SOURCE_DIR}/src/Orders/*.cpp")
file(GLOB_RECURSE PLAYER_SOURCES "${PROJECT_SOURCE_DIR}/src/Player/*.cpp")
file(GLOB_RECURSE UTILS_SOURCES "${PROJECT_SOURCE_DIR}/src/Utils/*.cpp")

# Create executable
add_executable(
	${PROJECT_NAME} 
	${CARDS_SOURCES}
	${GAME_ENGINE_SOURCES}
	${MAP_SOURCES}
	${ORDERS_SOURCES}
    ${PLAYER_SOURCES}
    ${UTILS_SOURCES}
    src/MainDriver.cpp
)

# Set output directories
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)