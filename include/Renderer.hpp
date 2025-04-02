// Renderer.h
#ifndef RENDERER_H
#define RENDERER_H

#include "SparseMap.hpp"
#include "TileSet.hpp"

class Renderer {
 public:
  void draw(const SparseMap &map, const TileSet &tiles, int width,
            int height) const;
};

#endif
