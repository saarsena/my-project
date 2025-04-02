#include "Player.hpp"
#include "Game.hpp"
#include <SDL.h>

Player::Player(int x, int y, SDL_Texture *texture)
    : Entity("Player", Position(x, y), 100, 5, 200, 20), texture(texture) {}

void Player::handleInput(const SDL_Event &e) {
  if (e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
    case SDLK_w:
    case SDLK_UP:
      setDirection(Direction::UP);
      break;
    case SDLK_s:
    case SDLK_DOWN:
      setDirection(Direction::DOWN);
      break;
    case SDLK_a:
    case SDLK_LEFT:
      setDirection(Direction::LEFT);
      break;
    case SDLK_d:
    case SDLK_RIGHT:
      setDirection(Direction::RIGHT);
      break;
    }
  } else if (e.type == SDL_KEYUP) {
    // Only clear the direction if the released key matches the current
    // direction
    Direction currentDir = getDirection();
    switch (e.key.keysym.sym) {
    case SDLK_w:
    case SDLK_UP:
      if (currentDir == Direction::UP)
        setDirection(Direction::NONE);
      break;
    case SDLK_s:
    case SDLK_DOWN:
      if (currentDir == Direction::DOWN)
        setDirection(Direction::NONE);
      break;
    case SDLK_a:
    case SDLK_LEFT:
      if (currentDir == Direction::LEFT)
        setDirection(Direction::NONE);
      break;
    case SDLK_d:
    case SDLK_RIGHT:
      if (currentDir == Direction::RIGHT)
        setDirection(Direction::NONE);
      break;
    }
  }
}

void Player::update(Game *game, int currentTick) {
  // Use base class update which handles movement
  Entity::update(game, currentTick);
}

bool Player::move(Direction direction, Game *game) {
  // Call base class move implementation
  bool moved = Entity::move(direction, game);

  // Additional player-specific logic after moving could go here

  return moved;
}

void Player::takeDamage(int amount) {
  Entity::takeDamage(amount);
  // Additional player-specific damage logic could go here
}

void Player::render(SDL_Renderer *renderer) {
  const int TILE_SIZE = 8;
  const int tileID = 2;

  const int columns = 16;
  SDL_Rect src = {(tileID % columns) * TILE_SIZE,
                  (tileID / columns) * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  SDL_Rect dest = {pos.x * TILE_SIZE, pos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  SDL_RenderCopy(renderer, texture, &src, &dest);
}
