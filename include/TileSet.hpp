
// TileSet.h
#ifndef TILESET_H
#define TILESET_H

#include <unordered_map>
#include "TileDef.hpp"

class TileSet {
private:
  std::unordered_map<int, TileDef> tiles;

public:
  void loadDefaults();
  const TileDef &get(int id) const;
};

#endif