#include "Tile.hpp"

const int TILE_SIZE = 8; // Example tile size

Tile::Tile(int x, int y, SDL_Texture *texture) : x(x), y(y), texture(texture) {}

void Tile::render(SDL_Renderer *renderer) {
  SDL_Rect dest = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  SDL_RenderCopy(renderer, texture, nullptr, &dest);
}
