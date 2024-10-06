# CMakeLists.txt : CMake project for LLVMPOC
# project specific logic here.

cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(LLVMPOC)

# Add source to this project's executable.
add_executable(LLVMPOC "LLVMPOC.cpp" "LLVMPOC.h" "IntConstentNode.h" "BinaryOperationNode.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LLVMPOC PROPERTY CXX_STANDARD 20)
endif()

# Specify the path to your LLVM installation
set(LLVM_DIR "I:/llvm/lib/cmake/llvm")  # Adjust this to match your actual path

# Find the LLVM package
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set LLVM include directories and link libraries
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Link to LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link the executable with LLVM libraries
target_link_libraries(LLVMPOC ${llvm_libs})
