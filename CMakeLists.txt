# Minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Project name and version
project(SeniorDesignProject VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
add_executable(Vision_Camera src/main.cpp)

# Include directories
# target_include_directories(ProjectName PUBLIC include)
target_include_directories(Vision_Camera PRIVATE include)

# Link libraries
# target_link_libraries(ProjectName PRIVATE some_library)

# Enable testing (optional)
# include(CTest)
# enable_testing()

# Install rules (optional)
# install(TARGETS ProjectName DESTINATION bin)