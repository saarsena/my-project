// === GameManager.cpp ===
#include "GameManager.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

GameManager::GameManager()
    : window(nullptr), renderer(nullptr), running(false), cleaned(false) {}

GameManager::~GameManager() {
  if (!cleaned) {
    clean();
  }
}

void GameManager::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return;
  }

  if (TTF_Init() < 0) {
    std::cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError()
              << std::endl;
    return;
  }

  window = SDL_CreateWindow("Dungeon Crawler", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, Game::SCREEN_WIDTH,
                            Game::SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    return;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    return;
  }

  game = std::make_unique<Game>(renderer);
  running = true;
}

void GameManager::run() {
  SDL_Event event;

  while (running) {
    // Handle events on queue
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      game->handleInput(event);
    }

    game->tick();
    game->update();
    game->render();
    SDL_RenderPresent(renderer);

    // Check if game wants to quit (ESC or 'q' pressed)
    if (!game->isRunning()) {
      std::cout << "GameManager: Game requested to exit" << std::endl;
      running = false;
    }

    SDL_Delay(16); // Cap at ~60 FPS
  }

  std::cout << "GameManager: Exiting game loop" << std::endl;
}

void GameManager::clean() {
  if (cleaned) {
    return; // Already cleaned up
  }

  std::cout << "GameManager: Starting cleanup" << std::endl;

  // Release game object first (which holds renderer-created resources)
  game.reset();

  // Free resources
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  TTF_Quit();
  SDL_Quit();

  cleaned = true;
  std::cout << "GameManager: Cleanup complete" << std::endl;
}
