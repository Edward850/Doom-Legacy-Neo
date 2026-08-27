cmake_minimum_required(VERSION 3.25)

cmake_policy(SET CMP0141 NEW)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
    "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>")

project(DoomLegacyNeo LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

find_package(SDL3 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)
find_package(libopenmpt REQUIRED)
find_package(Ogg REQUIRED)
find_package(Vorbis REQUIRED)
find_package(OpusFile REQUIRED)

option(BUILD_WSL2 "Build for WSL2 / Linux (when ON, do not use WIN32 executable flag)" OFF)

# --- Windows-specific resource files ---
set(WIN_RESOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.ico"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/default.manifest"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/resources.rc"
)

# --- doomlegacy source files ---
file(GLOB BASE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)
file(GLOB SDL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/sdl/*.cpp")
file(GLOB WIN32_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.cpp")
file(GLOB LINUX_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/linux/*.cpp")
file(GLOB HW_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/source/hardware/hw_*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/hardware/r_opengl/*.cpp"
)
file(GLOB CRYPTO_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/crypto-algorithms/*.c")
list(FILTER CRYPTO_SOURCES EXCLUDE REGEX ".*/[^/]*_test\\.c$")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/source" PREFIX "Source Files" FILES ${BASE_SOURCES} ${HW_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/sdl" PREFIX "Source Files\\sdl" FILES ${SDL_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/win32" PREFIX "Source Files\\win32" FILES ${WIN32_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/linux" PREFIX "Source Files\\linux" FILES ${LINUX_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/crypto-algorithms" PREFIX "Source Files\\crypto-algorithms" FILES ${CRYPTO_SOURCES})

set(DOOM_SOURCES 
    ${BASE_SOURCES} 
    ${SDL_SOURCES} 
    ${WIN32_SOURCES} 
    ${LINUX_SOURCES} 
    ${HW_SOURCES} 
    ${CRYPTO_SOURCES}
)

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

# Are we keeping the console for some reason? Add WIN32 flag if we aren't.
add_executable(doomlegacy ${DOOM_SOURCES} ${WIN_RESOURCE_FILES})

add_custom_command(TARGET doomlegacy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/legacy.dat"
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/legacy.wad"
        "$<TARGET_FILE_DIR:doomlegacy>"
    COMMENT "Copying core game assets to build directory..."
)

target_include_directories(doomlegacy PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/source"
    "${CMAKE_CURRENT_SOURCE_DIR}/sdl"
    "${CMAKE_CURRENT_SOURCE_DIR}/win32"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/hardware"
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

if(WIN32 AND NOT BUILD_WSL2)
target_compile_definitions(doomlegacy PRIVATE
    __WIN32__
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(doomlegacy PRIVATE
        LINUX
        VID_X11
    )
endif()

target_link_libraries(doomlegacy PRIVATE
    SDL3::SDL3
    OpenAL::OpenAL
	OpenSSL::SSL
    OpenSSL::Crypto
	CURL::libcurl
    Threads::Threads
    libopenmpt::libopenmpt
    Ogg::ogg
    Vorbis::vorbis
    Vorbis::vorbisfile
    OpusFile::opusfile
	ymfmidi
)

if (WIN32)
    target_link_libraries(doomlegacy PRIVATE
        shell32
        shlwapi
        ole32
        ws2_32
        opengl32
        glu32
    )
endif()

if(MSVC)
    target_compile_options(doomlegacy PRIVATE /W3)
	
	# Enable PDB debug symbols and MAP file output for Release builds
    target_compile_options(doomlegacy PRIVATE $<$<CONFIG:Release>:/Zi>)
    target_link_options(doomlegacy PRIVATE $<$<CONFIG:Release>:/DEBUG /MAP /OPT:REF /OPT:ICF>)
endif()