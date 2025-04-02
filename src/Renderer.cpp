// Renderer.cpp
#include "Renderer.hpp"

#include <iostream>

void Renderer::draw(const SparseMap &map, const TileSet &tiles, int width,
                    int height) const {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (map.hasTile(x, y)) {
        std::cout << tiles.get(map.getTileID(x, y)).glyph;
      } else {
        std::cout << ' ';
      }
    }
    std::cout << '\n';
  }
}
