#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "GameConfig.hpp"
#include <SDL2/SDL.h>

class Camera {
public:
  // Constructor now takes only the render resolution
  Camera(int resolutionWidth = GameConfig::GAME_AREA_WIDTH,
         int resolutionHeight = GameConfig::GAME_AREA_HEIGHT);

  // Update camera position to follow target (in pixel coordinates)
  void update(int targetX, int targetY, int mapWidth, int mapHeight);

  // Get the camera's viewport in pixel coordinates
  SDL_Rect getViewport() const;

  // Get the camera's resolution in pixels
  SDL_Point getResolution() const;

  // Convert world coordinates to screen coordinates (in pixels)
  SDL_Point worldToScreen(int worldX, int worldY) const;

  // Convert screen coordinates to world coordinates (in pixels)
  SDL_Point screenToWorld(int screenX, int screenY) const;

  // Get visible tiles
  SDL_Point getVisibleTiles() const {
    return {GameConfig::VISIBLE_TILES_X, GameConfig::VISIBLE_TILES_Y};
  }

  // Get the scale factors (from logical to screen coordinates)
  float getScaleX() const {
    return static_cast<float>(resolution.x) /
           (GameConfig::VISIBLE_TILES_X * GameConfig::TILE_SIZE);
  }
  float getScaleY() const {
    return static_cast<float>(resolution.y) /
           (GameConfig::VISIBLE_TILES_Y * GameConfig::TILE_SIZE);
  }

  // Set new resolution
  void setResolution(int width, int height);

private:
  SDL_Rect viewport;    // The visible area in pixel coordinates
  SDL_Point resolution; // The render resolution in pixels
};

#endif