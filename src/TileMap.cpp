#include "TileMap.hpp"
#include "GameConfig.hpp"

#include <SDL2/SDL_log.h>

#include <algorithm>
#include <unordered_map>

#include "SDL_blendmode.h"
#include "TileDef.hpp"
#include "TileSet.hpp"

TileMap::TileMap(int w, int h)
    : width(w), height(h), map(h, std::vector<int>(w, 0)) {}

void TileMap::setTile(int x, int y, int tileID) {
  if (x >= 0 && x < width && y >= 0 && y < height)
    map[y][x] = tileID;
}

int TileMap::getTile(int x, int y) const {
  if (x >= 0 && x < width && y >= 0 && y < height)
    return map[y][x];
  return 0;
}

void TileMap::setSpriteSheet(SDL_Texture *texture) { spriteSheet = texture; }

void TileMap::setFont(TTF_Font *f) { font = f; }

void TileMap::setGlyphMap(const std::unordered_map<char, SDL_Rect> &map) {
  glyphMap = map;
}

// Add new method to render with player position
void TileMap::renderCentered(SDL_Renderer *renderer, const TileSet &tiles,
                             SDL_Rect viewport, int playerX,
                             int playerY) const {
  // This view should show exactly VISIBLE_TILES_X x VISIBLE_TILES_Y tiles
  const int visibleTilesX = GameConfig::VISIBLE_TILES_X;
  const int visibleTilesY = GameConfig::VISIBLE_TILES_Y;
  const int tileSize = GameConfig::TILE_SIZE;

  // Cast to float for division to avoid integer truncation
  const float playerTileX = static_cast<float>(playerX) / tileSize;
  const float playerTileY = static_cast<float>(playerY) / tileSize;

  // Log for debugging
  SDL_Log("TileMap rendering %dx%d tiles in viewport (%d,%d,%d,%d)",
          visibleTilesX, visibleTilesY, viewport.x, viewport.y, viewport.w,
          viewport.h);

  // Calculate the top-left position to start rendering from
  int startX = std::max(0, static_cast<int>(playerTileX - visibleTilesX / 2));
  int startY = std::max(0, static_cast<int>(playerTileY - visibleTilesY / 2));

  // Ensure we don't go beyond the map boundaries
  if (startX + visibleTilesX > width) {
    startX = width - visibleTilesX;
  }
  if (startY + visibleTilesY > height) {
    startY = height - visibleTilesY;
  }

  // Ensure we don't have negative start positions
  startX = std::max(0, startX);
  startY = std::max(0, startY);

  SDL_Texture *tileTexture = tiles.getSpriteSheet();
  if (!tileTexture) {
    SDL_Log("TileMap: No sprite sheet available");
    return;
  }

  // Calculate exact scaling factors based on the viewport and visible tiles
  float scaleX = static_cast<float>(viewport.w) / (visibleTilesX * tileSize);
  float scaleY = static_cast<float>(viewport.h) / (visibleTilesY * tileSize);

  // Render the visible portion of the map
  for (int y = 0; y < visibleTilesY; ++y) {
    for (int x = 0; x < visibleTilesX; ++x) {
      // Calculate the map coordinates
      int mapX = startX + x;
      int mapY = startY + y;

      // Skip if we're outside the map boundaries
      if (mapX >= width || mapY >= height) {
        continue;
      }

      // Get the tile ID at this position
      int tileID = map[mapY][mapX];
      if (tileID == 0)
        continue; // Skip empty tiles

      // Get the tile definition
      const TileDef &tileDef = tiles.get(tileID);

      // Calculate the screen position for this tile, using the viewport
      // coordinates
      int screenX = viewport.x + static_cast<int>(x * tileSize * scaleX);
      int screenY = viewport.y + static_cast<int>(y * tileSize * scaleY);

      // Create the destination rectangle
      SDL_Rect destRect = {screenX, screenY,
                           static_cast<int>(tileSize * scaleX),
                           static_cast<int>(tileSize * scaleY)};

      // Create the source rectangle from the sprite sheet
      SDL_Rect srcRect = {tileDef.sheetX * tileSize, tileDef.sheetY * tileSize,
                          tileSize, tileSize};

      // Render the tile
      SDL_RenderCopy(renderer, tileTexture, &srcRect, &destRect);
    }
  }

  // Highlight the player's position - calculate using floats to avoid
  // truncation
  float relativePlayerX = playerTileX - static_cast<float>(startX);
  float relativePlayerY = playerTileY - static_cast<float>(startY);

  int playerScreenX =
      viewport.x + static_cast<int>(relativePlayerX * tileSize * scaleX);
  int playerScreenY =
      viewport.y + static_cast<int>(relativePlayerY * tileSize * scaleY);

  SDL_SetRenderDrawColor(renderer, 38, 139, 210, 128); // Semi-transparent blue
  SDL_Rect playerRect = {playerScreenX, playerScreenY,
                         static_cast<int>(tileSize * scaleX),
                         static_cast<int>(tileSize * scaleY)};
  SDL_RenderFillRect(renderer, &playerRect);
}

// Add new method that assumes viewport is already set
void TileMap::renderCenteredInViewport(SDL_Renderer *renderer,
                                       const TileSet &tiles, int playerX,
                                       int playerY) const {
  // This view should show exactly VISIBLE_TILES_X x VISIBLE_TILES_Y tiles
  const int visibleTilesX = GameConfig::VISIBLE_TILES_X;
  const int visibleTilesY = GameConfig::VISIBLE_TILES_Y;
  const int tileSize = GameConfig::TILE_SIZE;

  // Cast to float for division to avoid integer truncation
  const float playerTileX = static_cast<float>(playerX) / tileSize;
  const float playerTileY = static_cast<float>(playerY) / tileSize;

  // Log for debugging
  SDL_Log("TileMap renderCenteredInViewport for player at (%.1f,%.1f)",
          playerTileX, playerTileY);

  // Calculate the top-left position to start rendering from - cast to avoid
  // integer truncation
  int startX = std::max(0, static_cast<int>(playerTileX - visibleTilesX / 2));
  int startY = std::max(0, static_cast<int>(playerTileY - visibleTilesY / 2));

  // Ensure we don't go beyond the map boundaries
  if (startX + visibleTilesX > width) {
    startX = width - visibleTilesX;
  }
  if (startY + visibleTilesY > height) {
    startY = height - visibleTilesY;
  }

  // Ensure we don't have negative start positions
  startX = std::max(0, startX);
  startY = std::max(0, startY);

  SDL_Texture *tileTexture = tiles.getSpriteSheet();
  if (!tileTexture) {
    SDL_Log("TileMap: No sprite sheet available");
    return;
  }

  // Get current viewport to determine scale
  SDL_Rect viewport;
  SDL_RenderGetViewport(renderer, &viewport);

  // Calculate exact scaling factors based on the viewport and visible tiles
  float scaleX = static_cast<float>(viewport.w) / (visibleTilesX * tileSize);
  float scaleY = static_cast<float>(viewport.h) / (visibleTilesY * tileSize);

  // Render the visible portion of the map - use local coordinates since
  // viewport is set
  for (int y = 0; y < visibleTilesY; ++y) {
    for (int x = 0; x < visibleTilesX; ++x) {
      // Calculate the map coordinates
      int mapX = startX + x;
      int mapY = startY + y;

      // Skip if we're outside the map boundaries
      if (mapX >= width || mapY >= height) {
        continue;
      }

      // Get the tile ID at this position
      int tileID = map[mapY][mapX];
      if (tileID == 0)
        continue; // Skip empty tiles

      // Get the tile definition
      const TileDef &tileDef = tiles.get(tileID);

      // Calculate the screen position for this tile - in local coordinates to
      // viewport
      int screenX = static_cast<int>(x * tileSize * scaleX);
      int screenY = static_cast<int>(y * tileSize * scaleY);

      // Create the destination rectangle
      SDL_Rect destRect = {screenX, screenY,
                           static_cast<int>(tileSize * scaleX),
                           static_cast<int>(tileSize * scaleY)};

      // Create the source rectangle from the sprite sheet
      SDL_Rect srcRect = {tileDef.sheetX * tileSize, tileDef.sheetY * tileSize,
                          tileSize, tileSize};

      // Render the tile
      SDL_RenderCopy(renderer, tileTexture, &srcRect, &destRect);
    }
  }

  // Highlight the player's position - calculate using floats to avoid
  // truncation
  float relativePlayerX = playerTileX - static_cast<float>(startX);
  float relativePlayerY = playerTileY - static_cast<float>(startY);

  int playerScreenX = static_cast<int>(relativePlayerX * tileSize * scaleX);
  int playerScreenY = static_cast<int>(relativePlayerY * tileSize * scaleY);

  SDL_SetRenderDrawColor(renderer, 38, 139, 210, 128); // Semi-transparent blue
  SDL_Rect playerRect = {playerScreenX, playerScreenY,
                         static_cast<int>(tileSize * scaleX),
                         static_cast<int>(tileSize * scaleY)};
  SDL_RenderFillRect(renderer, &playerRect);
}

void TileMap::render(SDL_Renderer *renderer, const TileSet &tiles,
                     SDL_Rect viewport) const {
  // Default render method now uses the full map view starting from (0,0)
  // This is kept for backward compatibility

  // Use GameConfig tile size
  const int tileSize = GameConfig::TILE_SIZE;

  // Calculate how many tiles we can fit in the viewport, using floating point
  // division
  float tilesPerWidthF = static_cast<float>(viewport.w) / tileSize;
  float tilesPerHeightF = static_cast<float>(viewport.h) / tileSize;
  int visibleTilesX = static_cast<int>(tilesPerWidthF);
  int visibleTilesY = static_cast<int>(tilesPerHeightF);

  SDL_Texture *tileTexture = tiles.getSpriteSheet();
  if (!tileTexture) {
    SDL_Log("TileMap: No sprite sheet available");
    return;
  }

  // Log rendering details
  SDL_Log("TileMap::render - Showing %d x %d tiles in viewport (%d,%d,%d,%d)",
          visibleTilesX, visibleTilesY, viewport.x, viewport.y, viewport.w,
          viewport.h);

  // Save current renderer viewport
  SDL_Rect originalViewport;
  SDL_RenderGetViewport(renderer, &originalViewport);

  // Set viewport to the provided area
  SDL_RenderSetViewport(renderer, &viewport);

  // Fill the background with black for void space
  SDL_SetRenderDrawColor(renderer, 0, 23, 25, 255);
  SDL_RenderClear(renderer);

  // Render the visible portion of the map
  for (int y = 0; y < visibleTilesY && y < height; ++y) {
    for (int x = 0; x < visibleTilesX && x < width; ++x) {
      // Get the tile ID at this position
      int mapX = x;
      int mapY = y;

      // Make sure the coordinates are within bounds
      if (mapX >= 0 && mapX < width && mapY >= 0 && mapY < height) {
        int tileID = map[mapY][mapX];

        // Skip rendering empty tiles (tile ID 0)
        if (tileID == 0) {
          continue;
        }

        // Get the tile definition
        const TileDef &def = tiles.get(tileID);

        // Calculate screen position
        int screenX = x * tileSize;
        int screenY = y * tileSize;

        // Source rectangle from the sprite sheet
        SDL_Rect src = {def.sheetX * tileSize, def.sheetY * tileSize, tileSize,
                        tileSize};

        // Destination rectangle on screen
        SDL_Rect dest = {screenX, screenY, tileSize, tileSize};

        // Draw the tile
        SDL_RenderCopy(renderer, tileTexture, &src, &dest);
      }
    }
  }

  // Restore original viewport
  SDL_RenderSetViewport(renderer, &originalViewport);
}

// Add new method that uses absolute positioning
void TileMap::renderCenteredAt(SDL_Renderer *renderer, const TileSet &tiles,
                               int screenX, int screenY, int width, int height,
                               int playerX, int playerY) const {
  // This view should show exactly VISIBLE_TILES_X x VISIBLE_TILES_Y tiles
  const int visibleTilesX = GameConfig::VISIBLE_TILES_X;
  const int visibleTilesY = GameConfig::VISIBLE_TILES_Y;
  const int tileSize = GameConfig::TILE_SIZE;

  // Cast to float for division to avoid integer truncation
  const float playerTileX = static_cast<float>(playerX) / tileSize;
  const float playerTileY = static_cast<float>(playerY) / tileSize;

  // Log for explicit debugging
  SDL_Log("TileMap renderCenteredAt at screen pos (%d,%d) size %dx%d for "
          "player at (%.1f,%.1f)",
          screenX, screenY, width, height, playerTileX, playerTileY);

  // Calculate the top-left position to start rendering from
  int startX = std::max(0, static_cast<int>(playerTileX - visibleTilesX / 2));
  int startY = std::max(0, static_cast<int>(playerTileY - visibleTilesY / 2));

  // Ensure we don't go beyond the map boundaries
  if (startX + visibleTilesX > width) {
    startX = width - visibleTilesX;
  }
  if (startY + visibleTilesY > height) {
    startY = height - visibleTilesY;
  }

  // Ensure we don't have negative start positions
  startX = std::max(0, startX);
  startY = std::max(0, startY);

  SDL_Texture *tileTexture = tiles.getSpriteSheet();
  if (!tileTexture) {
    SDL_Log("TileMap: No sprite sheet available");
    return;
  }

  // Calculate exact scaling factors based on the provided dimensions
  float scaleX = static_cast<float>(width) / (visibleTilesX * tileSize);
  float scaleY = static_cast<float>(height) / (visibleTilesY * tileSize);

  // Render the visible portion of the map with absolute coordinates
  for (int y = 0; y < visibleTilesY; ++y) {
    for (int x = 0; x < visibleTilesX; ++x) {
      // Calculate the map coordinates
      int mapX = startX + x;
      int mapY = startY + y;

      // Skip if we're outside the map boundaries
      if (mapX >= this->width || mapY >= this->height) {
        continue;
      }

      // Get the tile ID at this position
      int tileID = map[mapY][mapX];
      if (tileID == 0)
        continue; // Skip empty tiles

      // Get the tile definition
      const TileDef &tileDef = tiles.get(tileID);

      // Calculate the screen position for this tile using absolute coordinates
      int tileScreenX = screenX + static_cast<int>(x * tileSize * scaleX);
      int tileScreenY = screenY + static_cast<int>(y * tileSize * scaleY);

      // Create the destination rectangle
      SDL_Rect destRect = {tileScreenX, tileScreenY,
                           static_cast<int>(tileSize * scaleX),
                           static_cast<int>(tileSize * scaleY)};

      // Create the source rectangle from the sprite sheet
      SDL_Rect srcRect = {tileDef.sheetX * tileSize, tileDef.sheetY * tileSize,
                          tileSize, tileSize};

      // Render the tile
      SDL_RenderCopy(renderer, tileTexture, &srcRect, &destRect);
    }
  }

  // Highlight the player's position - calculate using floats to avoid
  // truncation
  float relativePlayerX = playerTileX - static_cast<float>(startX);
  float relativePlayerY = playerTileY - static_cast<float>(startY);

  int playerScreenX =
      screenX + static_cast<int>(relativePlayerX * tileSize * scaleX);
  int playerScreenY =
      screenY + static_cast<int>(relativePlayerY * tileSize * scaleY);

  SDL_SetRenderDrawColor(renderer, 38, 139, 210, 128); // Semi-transparent blue
  SDL_Rect playerRect = {playerScreenX, playerScreenY,
                         static_cast<int>(tileSize * scaleX),
                         static_cast<int>(tileSize * scaleY)};
  SDL_RenderFillRect(renderer, &playerRect);
}
