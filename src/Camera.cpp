#include "Camera.hpp"
#include <algorithm>

Camera::Camera(int resolutionWidth, int resolutionHeight) {
  // Initialize viewport to show the correct number of tiles
  viewport = {0, 0, GameConfig::VISIBLE_TILES_X * GameConfig::TILE_SIZE,
              GameConfig::VISIBLE_TILES_Y * GameConfig::TILE_SIZE};
  resolution = {resolutionWidth, resolutionHeight};
}

void Camera::update(int targetX, int targetY, int mapWidth, int mapHeight) {
  // Center the camera on the target (in pixel coordinates)
  int newX = targetX - (viewport.w / 2);
  int newY = targetY - (viewport.h / 2);

  // Clamp the camera position to the map boundaries
  newX = std::max(0, std::min(newX, mapWidth - viewport.w));
  newY = std::max(0, std::min(newY, mapHeight - viewport.h));

  viewport.x = newX;
  viewport.y = newY;
}

SDL_Rect Camera::getViewport() const { return viewport; }

SDL_Point Camera::getResolution() const { return resolution; }

SDL_Point Camera::worldToScreen(int worldX, int worldY) const {
  // Convert from world coordinates to viewport-relative coordinates
  float relativeX = static_cast<float>(worldX - viewport.x);
  float relativeY = static_cast<float>(worldY - viewport.y);

  // Scale to screen resolution
  return {static_cast<int>(relativeX * getScaleX()),
          static_cast<int>(relativeY * getScaleY())};
}

SDL_Point Camera::screenToWorld(int screenX, int screenY) const {
  // Convert from screen coordinates to world coordinates
  return {static_cast<int>(screenX / getScaleX()) + viewport.x,
          static_cast<int>(screenY / getScaleY()) + viewport.y};
}

void Camera::setResolution(int width, int height) {
  resolution.x = width;
  resolution.y = height;
}