// GameManager.cpp
#include "GameManager.hpp"
#include "Game.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

GameManager::GameManager() {}
GameManager::~GameManager() { clean(); }

bool GameManager::init() {
  if (!window.init("SDL2 Game", SCREEN_WIDTH, SCREEN_HEIGHT))
    return false;
  game = new Game(window.getRenderer());
  isRunning = true;
  return true;
}

void GameManager::run() {
  while (isRunning) {
    handleEvents();
    update();
    render();
    SDL_Delay(16);
  }
}

void GameManager::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT)
      isRunning = false;
  }
}

void GameManager::update() {
  game->tick();
  game->update();
}

void GameManager::render() {
  window.clear();
  game->render();

  window.present();
}

void GameManager::clean() {
  delete game;
  window.clean();
}