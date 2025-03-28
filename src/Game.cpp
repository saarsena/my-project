// Game.cpp
#include "Game.hpp"
#include "SDL_render.h"
#include "SDL_surface.h"
#include <chrono>
#include <iostream>

Game::Game(SDL_Renderer *renderer) : renderer(renderer) {
  tiles.loadDefaults();
  loadArena();
  player = {"Player", {}, 100, 12, 100};
  enemy = {"Goblin", {}, 1000, 6, 2004};
  ssoyWielder = {"Player with SSoY", {}, 100, 39, 70};
  actionQueue.push({0, &ssoyWielder});
  actionQueue.push({0, &enemy});
}

void Game::tick() {
  std::cout << "\r[Tick " << tickCount << "]" << std::flush;
  while (!actionQueue.empty() && actionQueue.top().tick <= tickCount) {
    auto next = actionQueue.top();
    actionQueue.pop();
    Entity *attacker = next.entity;
    Entity *target = (attacker == &ssoyWielder) ? &enemy : &ssoyWielder;
    std::cout << attacker->name << " attacks " << target->name << " for "
              << attacker->attackDamage << " damage!\n";
    if (attacker->health <= 0 || target->health <= 0)
      continue;
    target->health -= attacker->attackDamage;
    if (target->health <= 0) {
      std::cout << target->name << " is defeated!\n";
      running = false;
      return;
    }
    int nextTick = tickCount + attacker->attackSpeed;
    actionQueue.push({nextTick, attacker});
  }
  tickCount++;
}

void Game::update() {}

void Game::render() {
  const int padding = 2;
  SDL_Rect topLeftViewport = {0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
  drawPanel(topLeftViewport, padding, border[0], background[0]);
  SDL_Rect topRightViewport = {SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2,
                               SCREEN_HEIGHT / 2};
  drawPanel(topRightViewport, padding, border[0], background[0]);
  SDL_Rect bottomViewport = {0, SCREEN_HEIGHT / 2, SCREEN_WIDTH,
                             SCREEN_HEIGHT / 2};
  drawPanel(bottomViewport, padding, border[0], background[0]);
}

void Game::loadArena() {
  const char *arena[] = {
      "################", "#              #", "#              #",
      "#              #", "#              #", "#              #",
      "#              #", "#              #", "#              #",
      "#              #", "#              #", "#              #",
      "#              #", "################"};
  int height = sizeof(arena) / sizeof(arena[0]);
  for (int y = 0; y < height; ++y) {
    const char *row = arena[y];
    for (int x = 0; row[x] != '\0'; ++x) {
      int tileID = glyphToTileID(row[x]);
      if (tileID != 0)
        map.setTile(x, y, tileID);
    }
  }
}

void Game::drawPanel(SDL_Rect viewport, int padding, SDL_Color border,
                     SDL_Color background) {
  SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
  SDL_RenderFillRect(renderer, &viewport);
  SDL_Rect inner = {viewport.x + padding, viewport.y + padding,
                    viewport.w - 2 * padding, viewport.h - 2 * padding};
  SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b,
                         background.a);
  SDL_RenderFillRect(renderer, &inner);
}

int Game::glyphToTileID(char c) {
  switch (c) {
  case '#':
    return 3;
  case ' ':
    return 1;
  default:
    return 0;
  }
}
