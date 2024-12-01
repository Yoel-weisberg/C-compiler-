cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Define project name and settings
project("FinalProjectYoelAndNeta")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)



# Add this line to ensure compatibility with LLVM's iterator debug level
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_ITERATOR_DEBUG_LEVEL=0")

add_compile_options("$<$<CONFIG:Debug>:/MD>")

# Collect all .cpp files in the cpp directory
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/cpp/*.cpp")

# Collect all .h files in the Header directory
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/Header/*.h")

# Create an executable from the source files
add_executable(FinalProjectYoelAndNeta ${SOURCES} ${HEADERS})

# Include header files in the project structure
source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})

# Add include directories for header files
target_include_directories(FinalProjectYoelAndNeta PRIVATE "${CMAKE_SOURCE_DIR}/Header")

# Set output directories for the executable
set_target_properties(FinalProjectYoelAndNeta PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")


# Specify the LLVM installation directory
set(LLVM_DIR "I:/llvm-project/install")

# Locate LLVMConfig.cmake
find_package(LLVM REQUIRED CONFIG PATHS "${LLVM_DIR}/lib/cmake/llvm")

# Print LLVM details (optional)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}/lib/cmake/llvm")

# Include LLVM headers
include_directories(${LLVM_INCLUDE_DIRS})

# Define LLVM libraries
llvm_map_components_to_libnames(LLVM_LIBS
    core
    bitwriter
    support
    analysis
    irPrinter
    target
)

# Link LLVM libraries to the project
target_link_libraries(FinalProjectYoelAndNeta PRIVATE ${LLVM_LIBS})



# Message for debugging
message(STATUS "LLVM libraries directory: ${LLVM_DIR}/lib")
