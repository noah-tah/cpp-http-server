# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

# Project name and version
project(cpp_http_server VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define source files
set(SOURCES
    src/windows/Networking.cpp
    src/windows/HTTPRequest.cpp
    src/windows/HTTPResponse.cpp
    src/windows/Utils.cpp
    src/windows/Server.cpp
    main.cpp
)

# Add executable target without extension
add_executable(server ${SOURCES})

# Include directories (scoped to the target)
target_include_directories(server PRIVATE include)

# Link libraries
target_link_libraries(server PRIVATE Ws2_32)