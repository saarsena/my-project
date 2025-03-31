// === Game.hpp ===
#pragma once
#include <queue>

#include "ChatBox.hpp"
#include "Entity.hpp"
#include "TileMap.hpp"
#include "TileSet.hpp"
#include "walkerdungeon.hpp"

class Game {
public:
  // Define constants at the start of the class
  static const int SCREEN_WIDTH = 800;
  static const int SCREEN_HEIGHT = 600;

  Game(SDL_Renderer *renderer);
  ~Game();
  void tick();
  void update();
  void render();
  void handleInput(const SDL_Event &event);

  // Check if the game is still running
  bool isRunning() const { return running; }

private:
  void loadArena();
  void generateDungeon();
  void drawPanel(SDL_Rect viewport, int padding, SDL_Color border,
                 SDL_Color background);
  int glyphToTileID(char c);
  void flavorText(const char *str, SDL_Rect &tRV);
  bool isValidMove(int newX, int newY) const;

  SDL_Renderer *renderer;
  TTF_Font *gFont; // Font used for rendering text
  ChatBox chatBox;
  TileSet tiles;
  TileMap map;
  WalkerDungeon dungeon;
  Entity player, enemy, ssoyWielder;
  int tickCount = 0;
  bool running = true;

  struct Action {
    int tick;
    Entity *entity;
    bool operator<(const Action &other) const { return tick > other.tick; }
  };
  std::priority_queue<Action> actionQueue;
};
