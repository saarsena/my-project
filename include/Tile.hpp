#pragma once
#include <SDL.h>

class Tile {
 public:
  Tile(int x, int y, SDL_Texture* texture);
  void render(SDL_Renderer* renderer);

 private:
  int x, y;
  SDL_Texture* texture;
};
