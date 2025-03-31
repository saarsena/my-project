// === GameManager.hpp ===
#pragma once
#include "Game.hpp"
#include <SDL2/SDL.h>
#include <memory>

class GameManager {
public:
  GameManager();
  ~GameManager();

  void init();
  void run();
  void clean();

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::unique_ptr<Game> game;
  bool running;
  bool cleaned; // Track if cleanup has been performed
};