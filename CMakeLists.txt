cmake_minimum_required(VERSION 3.16)
project(pyc_procmod)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find IDA SDK
if(NOT DEFINED IDA_SDK_DIR)
    if(DEFINED ENV{IDA_SDK_DIR})
        set(IDA_SDK_DIR $ENV{IDA_SDK_DIR})
    elseif(DEFINED ENV{IDASDK})
        set(IDA_SDK_DIR $ENV{IDASDK})
    elseif(DEFINED ENV{IDA_SDK})
        set(IDA_SDK_DIR $ENV{IDA_SDK})
    else()
        message(FATAL_ERROR 
            "IDA SDK not found. Please set IDA_SDK_DIR:\n"
            "  cmake -DIDA_SDK_DIR=/path/to/idasdk ..\n"
            "  OR set IDA_SDK_DIR / IDASDK environment variable")
    endif()
endif()

# Check if SDK has ida.hpp directly or in src/ subdirectory
if(NOT EXISTS "${IDA_SDK_DIR}/include/ida.hpp")
    if(EXISTS "${IDA_SDK_DIR}/src/include/ida.hpp")
        set(IDA_SDK_DIR "${IDA_SDK_DIR}/src")
        message(STATUS "Found IDA SDK in src/ subdirectory")
    else()
        message(FATAL_ERROR
            "Invalid IDA SDK path: ${IDA_SDK_DIR}\n"
            "Could not find ${IDA_SDK_DIR}/include/ida.hpp")
    endif()
endif()

message(STATUS "IDA SDK: ${IDA_SDK_DIR}")

# Platform detection
if(APPLE)
    set(IDA_PLATFORM "mac")
    set(IDA_PROC_EXT ".dylib")
    set(IDA_LDR_EXT ".dylib")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(IDA_LIB_DIR "${IDA_SDK_DIR}/lib/arm64_mac_clang_64")
    else()
        set(IDA_LIB_DIR "${IDA_SDK_DIR}/lib/x64_mac_clang_64")
    endif()
    add_compile_definitions(__MAC__)
elseif(WIN32)
    set(IDA_PLATFORM "win")
    set(IDA_PROC_EXT ".dll")
    set(IDA_LDR_EXT ".dll")
    set(IDA_LIB_DIR "${IDA_SDK_DIR}/lib/x64_win_vc_64")
    add_compile_definitions(__NT__)
else()
    set(IDA_PLATFORM "linux")
    set(IDA_PROC_EXT ".so")
    set(IDA_LDR_EXT ".so")
    set(IDA_LIB_DIR "${IDA_SDK_DIR}/lib/x64_linux_gcc_64")
    add_compile_definitions(__LINUX__)
endif()

# Common definitions
add_compile_definitions(
    __IDP__
    __PLUGIN__
    __EA64__
    USE_STANDARD_FILE_FUNCTIONS
)

# Common include directories
set(COMMON_INCLUDES
    ${IDA_SDK_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# =============================================================================
# Core source files (shared between loader and processor)
# =============================================================================

set(CORE_SOURCES
    src/core/version/magic.cpp
)

# =============================================================================
# Bytecode source files (shared)
# =============================================================================

set(BYTECODE_SOURCES
    src/bytecode/instruction/intrinsics.cpp
    src/bytecode/opcodes/opcodes_27.cpp
    src/bytecode/opcodes/opcodes_36.cpp
    src/bytecode/opcodes/opcodes_312.cpp
    src/bytecode/opcodes/opcodes_314.cpp
    src/bytecode/opcodes/table.cpp
    src/bytecode/operators/compare.cpp
    src/bytecode/operators/binary.cpp
)

# =============================================================================
# Loader module sources
# =============================================================================

set(LOADER_SOURCES
    src/ida/loader/loader.cpp
    ${CORE_SOURCES}
)

# =============================================================================
# Processor module sources
# =============================================================================

set(PROC_SOURCES
    src/ida/processor/processor.cpp
    ${CORE_SOURCES}
    ${BYTECODE_SOURCES}
)

# =============================================================================
# Loader module
# =============================================================================

add_library(pyc_ldr SHARED ${LOADER_SOURCES})
target_include_directories(pyc_ldr PRIVATE ${COMMON_INCLUDES})
target_link_directories(pyc_ldr PRIVATE ${IDA_LIB_DIR})

if(APPLE)
    target_link_libraries(pyc_ldr PRIVATE "-undefined dynamic_lookup")
    target_link_libraries(pyc_ldr PRIVATE ida)
elseif(WIN32)
    target_link_libraries(pyc_ldr PRIVATE ida)
else()
    target_link_options(pyc_ldr PRIVATE "-Wl,--allow-shlib-undefined")
    target_link_libraries(pyc_ldr PRIVATE ida)
endif()

set_target_properties(pyc_ldr PROPERTIES
    PREFIX ""
    OUTPUT_NAME "pyc_ldr"
    SUFFIX "${IDA_LDR_EXT}"
)

# =============================================================================
# Processor module
# =============================================================================

add_library(pyc_proc SHARED ${PROC_SOURCES})
target_include_directories(pyc_proc PRIVATE ${COMMON_INCLUDES})
target_link_directories(pyc_proc PRIVATE ${IDA_LIB_DIR})

if(APPLE)
    target_link_libraries(pyc_proc PRIVATE "-undefined dynamic_lookup")
    target_link_libraries(pyc_proc PRIVATE ida)
elseif(WIN32)
    target_link_libraries(pyc_proc PRIVATE ida)
else()
    target_link_options(pyc_proc PRIVATE "-Wl,--allow-shlib-undefined")
    target_link_libraries(pyc_proc PRIVATE ida)
endif()

set_target_properties(pyc_proc PROPERTIES
    PREFIX ""
    OUTPUT_NAME "pyc"
    SUFFIX "${IDA_PROC_EXT}"
)

# =============================================================================
# Install targets
# =============================================================================

if(DEFINED ENV{HOME})
    if(APPLE OR UNIX)
        set(IDA_USER_DIR "$ENV{HOME}/.idapro")
    endif()
elseif(DEFINED ENV{APPDATA})
    set(IDA_USER_DIR "$ENV{APPDATA}/Hex-Rays/IDA Pro")
endif()

if(DEFINED IDA_USER_DIR)
    install(TARGETS pyc_ldr DESTINATION "${IDA_USER_DIR}/loaders")
    install(TARGETS pyc_proc DESTINATION "${IDA_USER_DIR}/procs")
endif()

# Custom target for quick install
add_custom_target(install_local
    COMMAND ${CMAKE_COMMAND} -E make_directory "${IDA_USER_DIR}/loaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${IDA_USER_DIR}/procs"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pyc_ldr> "${IDA_USER_DIR}/loaders/"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pyc_proc> "${IDA_USER_DIR}/procs/"
    DEPENDS pyc_ldr pyc_proc
    COMMENT "Installing to ${IDA_USER_DIR}"
)

# =============================================================================
# macOS code signing
# =============================================================================

if(APPLE)
    add_custom_command(TARGET pyc_ldr POST_BUILD
        COMMAND codesign --force --sign - $<TARGET_FILE:pyc_ldr> || true
        COMMENT "Signing pyc_ldr"
    )
    add_custom_command(TARGET pyc_proc POST_BUILD
        COMMAND codesign --force --sign - $<TARGET_FILE:pyc_proc> || true
        COMMENT "Signing pyc_proc"
    )
endif()
