# Minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Project name and version
project(SeniorDesignProject VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)

# Add source files
add_executable(CamServer src/server.cpp src/shannon-fano.cpp)
add_executable(Test_Pkg src/test.cpp src/shannon-fano.cpp)


# Include directories: Vision Camera
target_include_directories(CamServer PRIVATE include)
target_include_directories(CamServer PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(CamServer PRIVATE OpenSSL::Crypto ${OpenCV_LIBS})


# Include directories: Test Package
target_include_directories(Test_Pkg PRIVATE include)
target_include_directories(Test_Pkg PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Test_Pkg PRIVATE OpenSSL::Crypto ${OpenCV_LIBS} gtest gtest_main)



# Link libraries
# target_link_libraries(ProjectName PRIVATE some_library)

# Enable testing (optional)
# include(CTest)
# enable_testing()

# Install rules (optional)
# install(TARGETS ProjectName DESTINATION bin)