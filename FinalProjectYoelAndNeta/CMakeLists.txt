cmake_minimum_required(VERSION 3.8)

# Set the project name and C++ standard
project("FinalProjectYoelAndNeta")
set(CMAKE_CXX_STANDARD 20)

# Collect source and header files
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/cpp/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/Header/*.h")
file(GLOB TEST_FILES "${CMAKE_SOURCE_DIR}/testFiles/*")

# Check if required files exist
if(NOT SOURCES)
    message(FATAL_ERROR "No source files found in ${CMAKE_SOURCE_DIR}/cpp/")
endif()
if(NOT HEADERS)
    message(WARNING "No header files found in ${CMAKE_SOURCE_DIR}/Header/")
endif()

# Create the executable
add_executable(FinalProjectYoelAndNeta ${SOURCES} ${HEADERS} "Header/SyntexAnalysis.h" "cpp/SyntexAnalysis.cpp" "Header/Helper.h")

# Organize files in the IDE for better visibility
source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Test Files" FILES ${TEST_FILES})

# Add include directories for headers
target_include_directories(FinalProjectYoelAndNeta PRIVATE "${CMAKE_SOURCE_DIR}/Header")

# Set the output directory for the executable
set_target_properties(FinalProjectYoelAndNeta PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Print diagnostic messages for debugging
message(STATUS "Source files: ${SOURCES}")
message(STATUS "Header files: ${HEADERS}")
message(STATUS "Test files: ${TEST_FILES}")
message(STATUS "Executable will be created in: ${CMAKE_SOURCE_DIR}/bin")
