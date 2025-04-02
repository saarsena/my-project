// SparseMap.h
#ifndef SPARSEMAP_H
#define SPARSEMAP_H

#include <unordered_map>

#include "Position.hpp"

class SparseMap {
private:
  std::unordered_map<Position, int> map;

public:
  void setTile(int x, int y, int tileID);
  bool hasTile(int x, int y) const;
  int getTileID(int x, int y) const;
};

#endif
