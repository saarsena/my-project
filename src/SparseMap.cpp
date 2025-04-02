
// SparseMap.cpp
#include "SparseMap.hpp"

void SparseMap::setTile(int x, int y, int tileID) { map[{x, y}] = tileID; }

bool SparseMap::hasTile(int x, int y) const {
  return map.find({x, y}) != map.end();
}

int SparseMap::getTileID(int x, int y) const {
  auto it = map.find({x, y});
  return it != map.end() ? it->second : 0;
}
