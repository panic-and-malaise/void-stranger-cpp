cmake_minimum_required(VERSION 3.28)
project(void-stranger LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

if(NOT CMAKE_BUILD_TYPE)
	set(default_build_type "Debug")
	message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
	set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Chooce the build type." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
	"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Statically link libraries for Windows build)
set(BUILD_SHARED_LIBS OFF)

# Fetch and configure latest SFML version
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/resources/)
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# --- Main executable ---
add_executable(main main.cpp ${PROJECT_SOURCES})

target_include_directories(main PRIVATE src)

target_link_libraries(
	main
	PRIVATE
	SFML::System
	SFML::Window
	SFML::Graphics
	SFML::Audio
)
target_compile_features(main PRIVATE cxx_std_20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(main PRIVATE
        -fsanitize=address,undefined
        -g
    )

    target_link_options(main PRIVATE
        -fsanitize=address,undefined
    )
endif()


if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()
