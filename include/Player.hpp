#pragma once
#include "Entity.hpp"
#include <SDL.h>

class Player : public Entity {
public:
  Player(int x = 0, int y = 0, SDL_Texture *texture = nullptr);
  ~Player() override = default;

  // Override Entity virtual methods
  void update(Game *game, int currentTick) override;
  bool move(Direction direction, Game *game) override;
  void takeDamage(int amount) override;

  // Player-specific methods
  void handleInput(const SDL_Event &e);
  void render(SDL_Renderer *renderer);

private:
  SDL_Texture *texture;
};
