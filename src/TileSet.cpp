// TileSet.cpp
#include "TileSet.hpp"

void TileSet::loadDefaults() {
  tiles[1] = TileDef(1, 0, 0, 0b00000001, ' ', "floor");
  tiles[2] = TileDef(2, 1, 0, 0b00000000, '~', "water");
  tiles[3] = TileDef(3, 2, 0, 0b00000000, '#', "wall");
}

const TileDef &TileSet::get(int id) const {
  auto it = tiles.find(id);
  if (it != tiles.end()) return it->second;
  static TileDef unknown(0, 0, 0, 0, '?', "unknown");
  return unknown;
}