cmake_minimum_required(VERSION 3.15)

project(smpn
    VERSION 1.0
    LANGUAGES C CXX
)

# -------------------------------------------------------------------
# Options
# -------------------------------------------------------------------

option(BUILD_EXAMPLES "Build example executables" ON)

# -------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings cross-platform
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Debug flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Optional static runtime linking
option(STATIC_LINKING "Enable static linking where supported" OFF)

if(STATIC_LINKING)
    if(MSVC)
        # MSVC static runtime
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        # GCC / Clang
        add_link_options(-static)
    endif()
endif()

# -------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------

find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(ENET libenet)
endif()

find_path(ENET_INCLUDE_DIR
    NAMES enet/enet.h
)

find_library(ENET_LIBRARY
    NAMES enet
)

if(ENET_FOUND)
    set(ENET_INCLUDE_DIRS ${ENET_INCLUDE_DIRS})
    set(ENET_LIBRARIES ${ENET_LIBRARIES})
elseif(ENET_INCLUDE_DIR AND ENET_LIBRARY)
    set(ENET_INCLUDE_DIRS ${ENET_INCLUDE_DIR})
    set(ENET_LIBRARIES ${ENET_LIBRARY})
else()
    message(FATAL_ERROR "ENet library not found")
endif()

# -------------------------------------------------------------------
# Library sources
# -------------------------------------------------------------------

file(GLOB SMPN_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# -------------------------------------------------------------------
# Static library
# -------------------------------------------------------------------

add_library(smpn STATIC ${SMPN_SOURCES})

target_include_directories(smpn
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${ENET_INCLUDE_DIRS}
)

target_link_libraries(smpn
    PUBLIC
        ${ENET_LIBRARIES}
)

# Output name: libsmpn.a / smpn.lib
set_target_properties(smpn PROPERTIES
    OUTPUT_NAME smpn
)

# -------------------------------------------------------------------
# Examples
# -------------------------------------------------------------------

if(BUILD_EXAMPLES)

    file(GLOB EXAMPLE_SOURCES
        "${CMAKE_SOURCE_DIR}/examples/*.cpp"
    )

    foreach(EXAMPLE_FILE ${EXAMPLE_SOURCES})

        get_filename_component(EXAMPLE_NAME
            ${EXAMPLE_FILE}
            NAME_WE
        )

        add_executable(${EXAMPLE_NAME} ${EXAMPLE_FILE})

        target_link_libraries(${EXAMPLE_NAME}
            PRIVATE
                smpn
        )

        target_include_directories(${EXAMPLE_NAME}
            PRIVATE
                ${CMAKE_SOURCE_DIR}/include
        )

        # Place executables in build/examples
        set_target_properties(${EXAMPLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY
                ${CMAKE_BINARY_DIR}/examples
        )

    endforeach()

endif()

# -------------------------------------------------------------------
# Install
# -------------------------------------------------------------------

include(GNUInstallDirs)

install(
    TARGETS smpn
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# -------------------------------------------------------------------
# Summary
# -------------------------------------------------------------------

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Building examples: ${BUILD_EXAMPLES}")
message(STATUS "Static linking: ${STATIC_LINKING}")
