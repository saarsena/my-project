#include "Tile.hpp"

Tile::Tile(int x, int y, SDL_Texture* texture)
    : x(x), y(y), texture(texture) {}

void Tile::render(SDL_Renderer* renderer) {
    SDL_Rect dest = { x * 8, y * 8, 8, 8 };
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
}
