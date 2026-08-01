cmake_minimum_required(VERSION 3.20)

project(DoomLegacyNeo LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(SDL3 CONFIG REQUIRED)

find_package(OpenAL CONFIG REQUIRED)
find_package(Threads REQUIRED)

file(GLOB DOOM_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hardware/hw_*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/hardware/r_opengl/*.c"
)

file(GLOB CRYPTO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/crypto-algorithms/*.c"
)
list(FILTER CRYPTO_SOURCES EXCLUDE REGEX ".*/[^/]*_test\\.c$")
list(APPEND DOOM_SOURCES ${CRYPTO_SOURCES})

# --- ymfmidi library ---
file(GLOB YMFMIDI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/ymfm/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/ymfm/src/*.cpp"
)

list(REMOVE_ITEM YMFMIDI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/src/console.cpp"
)

add_library(ymfmidi STATIC ${YMFMIDI_SOURCES})
target_include_directories(ymfmidi PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/ymfm/src"
)
target_compile_features(ymfmidi PUBLIC cxx_std_14)

# --- doomlegacy includes and linking ---

add_executable(doomlegacy ${DOOM_SOURCES})

target_include_directories(doomlegacy PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl"
    "${CMAKE_CURRENT_SOURCE_DIR}/win32"
    "${CMAKE_CURRENT_SOURCE_DIR}/hardware"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/ymfmidi/include"
)

if (ENABLE_ASAN)
    foreach(_asan_target ymfmidi doomlegacy)
        if (TARGET ${_asan_target})
            if (MSVC)
                target_compile_options(${_asan_target} PRIVATE /fsanitize=address /Zi)
            elseif (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
                target_compile_options(${_asan_target} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
            endif()
        endif()
    endforeach()

    if (TARGET doomlegacy)
        if (MSVC)
            target_link_options(doomlegacy PRIVATE /fsanitize=address /INCREMENTAL:NO)
        elseif (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
            target_link_options(doomlegacy PRIVATE -fsanitize=address)
        endif()
    endif()
endif()

target_compile_definitions(doomlegacy PRIVATE
    __WIN32__
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

target_link_libraries(doomlegacy PRIVATE
    SDL3::SDL3
    OpenAL::OpenAL
    Threads::Threads
    shell32
    shlwapi
    ole32
	ymfmidi
)

if (WIN32)
    target_link_libraries(doomlegacy PRIVATE
        ws2_32
        opengl32
        glu32
    )
endif()

if(MSVC)
    target_compile_options(doomlegacy PRIVATE /W3)
endif()