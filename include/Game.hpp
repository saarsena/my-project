// === Game.hpp ===
#pragma once
#include <queue>

#include "Camera.hpp"
#include "ChatBox.hpp"
#include "Entity.hpp"
#include "GameConfig.hpp"
#include "TileMap.hpp"
#include "TileSet.hpp"
#include "walkerdungeon.hpp"

class Player;

class Game {
public:
  Game(SDL_Renderer *renderer);
  ~Game();
  void tick();
  void update();
  void render();
  void handleInput(const SDL_Event &event);

  // Check if the game is still running
  bool isRunning() const { return running; }

  // Made public for Entity access
  bool isValidMove(int newX, int newY) const;
  int getCurrentTick() const { return tickCount; }

private:
  void loadArena();
  void generateDungeon();
  void drawPanel(SDL_Rect viewport, int padding, SDL_Color border,
                 SDL_Color background);
  int glyphToTileID(char c);
  void flavorText(const char *str, SDL_Rect &tRV);

  SDL_Renderer *renderer;
  TTF_Font *gFont; // Font used for rendering text
  ChatBox chatBox;
  TileSet tiles;
  TileMap map;
  WalkerDungeon dungeon;
  Camera camera;
  Player *player; // Updated to Player pointer
  Entity enemy;
  Entity ssoyWielder;
  int tickCount = 0;
  bool running = true;

  struct Action {
    int tick;
    Entity *entity;
    bool operator<(const Action &other) const { return tick > other.tick; }
  };
  std::priority_queue<Action> actionQueue;
};
