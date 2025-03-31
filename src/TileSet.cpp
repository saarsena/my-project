#include "TileSet.hpp"
#include <SDL2/SDL.h>

TileSet::TileSet() : spriteSheet(nullptr) {
  // Constructor: initialize spriteSheet to nullptr or load a default texture.
}

TileSet::~TileSet() {
  // Optionally, if TileSet owns the texture, clean it up here:
  // if (spriteSheet) SDL_DestroyTexture(spriteSheet);
}

void TileSet::loadDefaults() {
  // ID 0 is void/empty space (outside the dungeon) - not rendered in
  // TileMap.cpp

  // ID 1 is floor (inside dungeon) - using a different sprite position for
  // better visuals Using position (1,1) for the floor which typically looks
  // like a stone floor in the tileset
  // TileDef parameters: (id, sheetX, sheetY, flags, glyph, name)
  // sheetX and sheetY are pixel coordinates on the sprite sheet
  // 131 is the tile ID, (26,64) are the x,y coordinates on the sprite sheet
  // TileDef parameters: (id, sheetX, sheetY, flags, glyph, name)
  // id=1: floor tile
  // sheetX=26, sheetY=64: coordinates on the sprite sheet (in pixels)
  // flags=0b00000001: walkable flag is set
  // glyph='.': character representation
  // name="floor": descriptive name
  tiles[1] = TileDef(1, 3, 8, 0b00000001, '.', "floor");

  // ID 2 is unused for now (water) - could be used for special terrain
  tiles[2] = TileDef(2, 0, 4, 0b00000000, '~', "player");

  // ID 3 is wall (dungeon boundary) - using position (2,1) for wall which
  // typically looks like a solid wall in the tileset
  tiles[3] = TileDef(3, 3, 2, 0b00000000, '#', "wall");

  // Additional tile definitions for gameplay elements
  // Door tile (currently unused)
  tiles[4] = TileDef(4, 4, 1, 0b00000000, 'D', "door");

  // Stairs tile (currently unused)
  tiles[5] = TileDef(5, 5, 1, 0b00000000, 'S', "stairs");
}

const TileDef &TileSet::get(int id) const {
  auto it = tiles.find(id);
  if (it != tiles.end())
    return it->second;
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
