# Minimum CMake version required
cmake_minimum_required(VERSION 3.16)

# Project name and version
project(SeniorDesignProject VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_COMPILER "g++-13")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add source files
add_executable(CamServer src/server.cpp src/shannon-fano.cpp)
add_executable(CamClient src/client.cpp)
add_executable(Test_Pkg src/test.cpp src/shannon-fano.cpp)


# Include Directories: Camera Server
target_include_directories(CamServer PRIVATE include)
target_include_directories(CamServer PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(CamServer PRIVATE OpenSSL::Crypto ${OpenCV_LIBS})
target_link_libraries(CamServer PRIVATE nlohmann_json::nlohmann_json)

# Include Directories: Camera Client
target_include_directories(CamClient PRIVATE include)
target_include_directories(CamClient PRIVATE ${OPENCV_INCLUDE_DIRS})
target_link_libraries(CamClient PRIVATE OpenSSL::Crypto ${OpenCV_LIBS})
target_link_libraries(CamClient PRIVATE nlohmann_json::nlohmann_json)

# Include Directories: Test Package
target_include_directories(Test_Pkg PRIVATE include)
target_include_directories(Test_Pkg PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Test_Pkg PRIVATE OpenSSL::Crypto ${OpenCV_LIBS} gtest gtest_main)
target_link_libraries(Test_Pkg PRIVATE nlohmann_json::nlohmann_json)

# Link libraries
# target_link_libraries(ProjectName PRIVATE some_library)

# Enable testing (optional)
# include(CTest)
# enable_testing()

# Install rules (optional)
# install(TARGETS ProjectName DESTINATION bin)