#include "TileMap.hpp"
#include "TileDef.hpp"
#include "TileSet.hpp"
#include <SDL2/SDL_log.h>

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
  // Tile dimensions from the sprite sheet
  const int TILE_SIZE = 8;

  // Calculate how many tiles we can fit in the viewport
  int visibleTilesX = viewport.w / TILE_SIZE;
  int visibleTilesY = viewport.h / TILE_SIZE;

  // Calculate the top-left position to start rendering from
  int startX = std::max(0, playerX - visibleTilesX / 2);
  int startY = std::max(0, playerY - visibleTilesY / 2);

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

  // Log rendering details
  SDL_Log("TileMap: Rendering %d x %d tiles centered on player (%d,%d), "
          "starting at (%d,%d)",
          visibleTilesX, visibleTilesY, playerX, playerY, startX, startY);

  // Save current renderer viewport
  SDL_Rect originalViewport;
  SDL_RenderGetViewport(renderer, &originalViewport);

  // Set viewport to the provided area
  SDL_RenderSetViewport(renderer, &viewport);

  // Fill the background with black for void space
  SDL_SetRenderDrawColor(renderer, 0, 23, 25, 255);
  SDL_RenderClear(renderer);

  // Calculate the screen position of the player for highlighting later
  int playerScreenX = (playerX - startX) * TILE_SIZE;
  int playerScreenY = (playerY - startY) * TILE_SIZE;

  // Render the visible portion of the map
  for (int y = 0; y < visibleTilesY; ++y) {
    for (int x = 0; x < visibleTilesX; ++x) {
      // Calculate the map coordinates from screen position
      int mapX = startX + x;
      int mapY = startY + y;

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
        int screenX = x * TILE_SIZE;
        int screenY = y * TILE_SIZE;

        // Source rectangle from the sprite sheet
        SDL_Rect src = {def.sheetX * TILE_SIZE, def.sheetY * TILE_SIZE,
                        TILE_SIZE, TILE_SIZE};

        // Destination rectangle on screen
        SDL_Rect dest = {screenX, screenY, TILE_SIZE, TILE_SIZE};

        // Draw the tile
        SDL_RenderCopy(renderer, tileTexture, &src, &dest);
      }
    }
  }

  // Draw player position with a highlighted marker
  SDL_Rect playerMarker = {playerScreenX, playerScreenY, TILE_SIZE, TILE_SIZE};

  // Draw a colored rectangle at player position
  SDL_SetRenderDrawColor(renderer, 255, 255, 0,
                         180); // Yellow with transparency
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderDrawRect(renderer, &playerMarker);

  // Draw a smaller filled rectangle to make player more visible
  SDL_Rect playerDot = {playerScreenX + TILE_SIZE / 4,
                        playerScreenY + TILE_SIZE / 4, TILE_SIZE / 2,
                        TILE_SIZE / 2};
  SDL_RenderFillRect(renderer, &playerDot);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

  // Restore original viewport
  SDL_RenderSetViewport(renderer, &originalViewport);
}

void TileMap::render(SDL_Renderer *renderer, const TileSet &tiles,
                     SDL_Rect viewport) const {
  // Default render method now uses the full map view starting from (0,0)
  // This is kept for backward compatibility

  // Tile dimensions from the sprite sheet
  const int TILE_SIZE = 8;

  // Calculate how many tiles we can fit in the viewport
  int visibleTilesX = viewport.w / TILE_SIZE;
  int visibleTilesY = viewport.h / TILE_SIZE;

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
        int screenX = x * TILE_SIZE;
        int screenY = y * TILE_SIZE;

        // Source rectangle from the sprite sheet
        SDL_Rect src = {def.sheetX * TILE_SIZE, def.sheetY * TILE_SIZE,
                        TILE_SIZE, TILE_SIZE};

        // Destination rectangle on screen
        SDL_Rect dest = {screenX, screenY, TILE_SIZE, TILE_SIZE};

        // Draw the tile
        SDL_RenderCopy(renderer, tileTexture, &src, &dest);
      }
    }
  }

  // Restore original viewport
  SDL_RenderSetViewport(renderer, &originalViewport);
}
