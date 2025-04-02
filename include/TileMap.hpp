#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <unordered_map>
#include <vector>

#include "GameConfig.hpp"

class TileSet;

class TileMap {
public:
  TileMap(int w, int h);

  void setTile(int x, int y, int tileID);
  int getTile(int x, int y) const;

  void setSpriteSheet(SDL_Texture *texture);
  void setFont(TTF_Font *f);
  void setGlyphMap(const std::unordered_map<char, SDL_Rect> &map);

  void render(SDL_Renderer *renderer, const TileSet &tiles,
              SDL_Rect viewport) const;

  // New method to render map centered on player
  void renderCentered(SDL_Renderer *renderer, const TileSet &tiles,
                      SDL_Rect viewport, int playerX, int playerY) const;

  // New method - assumes viewport already set
  void renderCenteredInViewport(SDL_Renderer *renderer, const TileSet &tiles,
                                int playerX, int playerY) const;

  // New method - uses absolute positioning with explicit coordinates
  void renderCenteredAt(SDL_Renderer *renderer, const TileSet &tiles,
                        int screenX, int screenY, int width, int height,
                        int playerX, int playerY) const;

  int getWidth() const { return width; }
  int getHeight() const { return height; }

  // Convert between pixel and tile coordinates
  static int pixelToTile(int pixel) { return pixel / GameConfig::TILE_SIZE; }
  static int tileToPixel(int tile) { return tile * GameConfig::TILE_SIZE; }

private:
  int width;  // Width in tiles
  int height; // Height in tiles
  std::vector<std::vector<int>> map;

  SDL_Texture *spriteSheet = nullptr;
  TTF_Font *font = nullptr;
  std::unordered_map<char, SDL_Rect> glyphMap;

public:
  void loadFont(TTF_Font *font) {
    if (!font) {
      SDL_Log("Error loading font: %s", TTF_GetError());
    } else {
      setFont(font);
    }
  }
};
