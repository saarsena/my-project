cmake_minimum_required(VERSION 3.10)
project(mockup C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

include_directories(
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
  ${SDL2_MIXER_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
)

file(GLOB SRC_FILES
  ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Explicitly add walkerdungeon.cpp in case it's not picked up by GLOB
set(SRC_FILES ${SRC_FILES} ${CMAKE_SOURCE_DIR}/src/walkerdungeon.cpp)

# Add source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/GameManager.cpp
    src/TileMap.cpp
    src/TileSet.cpp
    src/ChatBox.cpp
    src/Camera.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
  ${SDL2_TTF_LIBRARIES}
  ${SDL2_MIXER_LIBRARIES}
)

# Create assets directory if it doesn't exist in the source
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/assets)

# Copy assets directory to build directory
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
  COMMENT "Copying assets directory to build directory"
)

# Create fallback fonts if missing
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/assets/TerminessNerdFontMono-Regular.ttf")
  message("Downloading a fallback font to the assets directory...")
  file(DOWNLOAD
    "https://github.com/ryanoasis/nerd-fonts/raw/master/patched-fonts/Terminus/terminus-ttf-4.49.1/Regular/complete/Terminess%20(TTF)%20Nerd%20Font%20Complete%20Mono%20Windows%20Compatible%20Regular.ttf"
    "${CMAKE_SOURCE_DIR}/assets/TerminessNerdFontMono-Regular.ttf"
    SHOW_PROGRESS
    STATUS DOWNLOAD_STATUS
  )
  list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
  if(NOT STATUS_CODE EQUAL 0)
    message(WARNING "Failed to download font, using system fonts as fallback")
  endif()
endif()
