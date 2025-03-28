#ifndef GAME_HPP
#define GAME_HPP

#include "ActionSchedule.hpp"
#include "Entity.hpp"
#include "Renderer.hpp"
#include "SparseMap.hpp"
#include "TileSet.hpp"
#include <SDL2/SDL.h>
#include <queue>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class Game {
public:
  Game(SDL_Renderer *renderer);
  void tick();
  void update();
  void render();
  void drawPanel(SDL_Rect viewport, int padding, SDL_Color border,
                 SDL_Color background);

private:
  TileSet tiles;
  SparseMap map;
  Renderer tileRenderer;

  const int width = 16;
  const int height = 14;
  const int TICKS_PER_SECOND = 60;
  const int TICK_DURATION_MS = 1000 / TICKS_PER_SECOND;

  int tickCount = 0;
  bool running = true;

  Entity player, enemy, ssoyWielder;
  std::priority_queue<ActionSchedule> actionQueue;

  SDL_Renderer *renderer;

  void loadArena();
  int glyphToTileID(char c);

  SDL_Color border[4] = {255, 255, 255, 255};
  SDL_Color background[4] = {0, 0, 0, 255};
};

#endif