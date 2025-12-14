cmake_minimum_required(VERSION 3.12)
project(CANTramFramework VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(CANTRAM_BUILD_EXAMPLES "Build example projects" ON)
option(CANTRAM_BUILD_TESTS "Build unit tests" OFF)
option(CANTRAM_BUILD_DOCS "Build documentation" OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Platform detection
if(ESP32)
    add_definitions(-DESP32)
elseif(STM32)
    add_definitions(-DSTM32)
elseif(TEENSY)
    add_definitions(-DTEENSY)
endif()

# Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add libraries from lib directory
file(GLOB LIBRARY_DIRS "lib/*")
foreach(LIBRARY_DIR ${LIBRARY_DIRS})
    if(IS_DIRECTORY ${LIBRARY_DIR})
        get_filename_component(LIBRARY_NAME ${LIBRARY_DIR} NAME)
        if(EXISTS ${LIBRARY_DIR}/CMakeLists.txt)
            message(STATUS "Adding library: ${LIBRARY_NAME}")
            add_subdirectory(${LIBRARY_DIR})
        endif()
    endif()
endforeach()

# Build examples
if(CANTRAM_BUILD_EXAMPLES)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples)
        message(STATUS "Building examples")
        add_subdirectory(examples)
    endif()
endif()

# Build tests
if(CANTRAM_BUILD_TESTS)
    enable_testing()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        message(STATUS "Building tests")
        add_subdirectory(tests)
    endif()
endif()

# Build documentation
if(CANTRAM_BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        message(STATUS "Building documentation")
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        add_custom_target(doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    else()
        message(WARNING "Doxygen not found, documentation will not be built")
    endif()
endif()

# Installation rules
install(DIRECTORY lib/
    DESTINATION include/CANTram
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CANTramFrameworkConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CANTramFrameworkConfigVersion.cmake"
    DESTINATION lib/cmake/CANTramFramework
)
