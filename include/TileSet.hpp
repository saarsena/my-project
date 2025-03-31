#ifndef TILESET_H
#define TILESET_H

#include "TileDef.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

class TileSet {
private:
  std::unordered_map<int, TileDef> tiles;
  SDL_Texture *spriteSheet = nullptr;

public:
  TileSet();
  ~TileSet();
  void loadDefaults();
  const TileDef &get(int id) const;
  bool loadSpriteSheet(const std::string &path, SDL_Renderer *renderer);
  void setSpriteSheet(SDL_Texture *texture);
  SDL_Texture *getSpriteSheet() const;
};

#endif