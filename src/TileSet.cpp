#include "TileSet.hpp"

#include <SDL2/SDL.h>

#include <string>

TileSet::TileSet() : spriteSheet(nullptr) {
  // Constructor: initialize spriteSheet to nullptr or load a default texture.
}

TileSet::~TileSet() {
  // Optionally, if TileSet owns the texture, clean it up here:
  // if (spriteSheet) SDL_DestroyTexture(spriteSheet);
}

void TileSet::loadDefaults() {
  tiles[1] = TileDef(1, 3, 8, 0b00000001, '.', "floor");

  tiles[2] = TileDef(2, 0, 4, 0b00000000, '~', "player");

  tiles[3] = TileDef(3, 3, 2, 0b00000000, '#', "wall");

  tiles[4] = TileDef(4, 4, 1, 0b00000000, 'D', "door");

  tiles[5] = TileDef(5, 5, 1, 0b00000000, 'S', "stairs");
}

const TileDef &TileSet::get(int id) const {
  auto it = tiles.find(id);
  if (it != tiles.end()) return it->second;
  static TileDef unknown(0, 0, 0, 0, '?', "unknown");
  return unknown;
}

void TileSet::setSpriteSheet(SDL_Texture *texture) { spriteSheet = texture; }

SDL_Texture *TileSet::getSpriteSheet() const { return spriteSheet; }

bool TileSet::loadSpriteSheet(const std::string &path, SDL_Renderer *renderer) {
  SDL_Surface *surface = IMG_Load(path.c_str());
  if (!surface) {
    return false;
  }
  spriteSheet = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);
  return true;
}
