# CMakeLists.txt : CMake project for FinalProjectYoelAndNeta, includes source files and settings.

cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Define project name and settings
project("FinalProjectYoelAndNeta")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Collect all .cpp files in the cpp directory
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/cpp/*.cpp")

# Collect all .h files in the Header directory
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/Header/*.h")

# Optional: Collect test files if needed
file(GLOB TEST_FILES "${CMAKE_SOURCE_DIR}/testFiles/*")

# Create an executable from the source files
add_executable(FinalProjectYoelAndNeta ${SOURCES} ${HEADERS})

# Include header files in the project structure
source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Test Files" FILES ${TEST_FILES})

# Add include directories for header files
target_include_directories(FinalProjectYoelAndNeta PRIVATE "${CMAKE_SOURCE_DIR}/Header")

set_target_properties(FinalProjectYoelAndNeta PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")


# Add additional settings or logic if needed here (e.g., link libraries, etc.)

# Optional: Tests and install targets (to be added as needed)
