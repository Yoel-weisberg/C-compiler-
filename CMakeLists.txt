# CMakeLists.txt

cmake_minimum_required(VERSION 3.15)
project(C_Compiler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add executable
add_executable(C_Compiler {CMAKE_SOURCE_DIR}/FinalProjectYoelAndNeta/cpp/main.cpp)

# Include directories (if needed)
target_include_directories(C_Compiler PRIVATE include)
