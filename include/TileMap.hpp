#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <vector>

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

  int getWidth() const { return width; }
  int getHeight() const { return height; }

private:
  int width;
  int height;
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
