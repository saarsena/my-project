#include "Player.hpp"
#include <SDL.h>

Player::Player(int x, int y, SDL_Texture *texture)
    : x(x), y(y), texture(texture) {}

void Player::handleInput(const SDL_Event &e) {
  if (e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
    case SDLK_w:
      y--;
      break;
    case SDLK_s:
      y++;
      break;
    case SDLK_a:
      x--;
      break;
    case SDLK_d:
      x++;
      break;
    }
  }
}

void Player::update() {
  // Add future logic here
}

void Player::render(SDL_Renderer *renderer) {
  const int TILE_SIZE = 8;
  const int tileID = 2;
  // Adjust columns to match your tilesheet (here assumed 16 columns)
  const int columns = 16;
  SDL_Rect src = {(tileID % columns) * TILE_SIZE,
                  (tileID / columns) * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  SDL_Rect dest = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  SDL_RenderCopy(renderer, texture, &src, &dest);
}