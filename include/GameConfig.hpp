#pragma once

namespace GameConfig {
// Base configuration
static const int TILE_SIZE = 8;        // Size of each tile in pixels
static const int VISIBLE_TILES_X = 40; // Number of tiles visible horizontally
static const int VISIBLE_TILES_Y = 25; // Number of tiles visible vertically

// Screen dimensions
static const int SCREEN_WIDTH = 1600;
static const int SCREEN_HEIGHT = 900;

// Derived view dimensions
static const int GAME_VIEW_WIDTH =
    VISIBLE_TILES_X * TILE_SIZE; // Logical view width in pixels
static const int GAME_VIEW_HEIGHT =
    VISIBLE_TILES_Y * TILE_SIZE; // Logical view height in pixels

// Render area dimensions (maintains square aspect ratio)
static const int GAME_AREA_SIZE =
    SCREEN_WIDTH / 3; // Size of the square game area
static const int GAME_AREA_WIDTH = GAME_AREA_SIZE;
static const int GAME_AREA_HEIGHT = GAME_AREA_SIZE;
} // namespace GameConfig