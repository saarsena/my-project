// === Game.cpp ===
#include "Game.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <limits>
#include <unordered_map>

#include "ChatBox.hpp"
#include "TileMap.hpp"
#include "TileSet.hpp"

SDL_Color border[1] = {{253, 246, 227, 255}}; // White border
SDL_Color background[1] = {{0, 43, 54, 255}}; // Black background

// Define colors for combat messages
SDL_Color colorWhite = {253, 246, 227, 255}; // Default white text
SDL_Color colorRed = {220, 50, 47, 255};     // Red for enemy attacks
SDL_Color colorBlue = {38, 139, 210, 255};   // Blue for player attacks

Game::Game(SDL_Renderer *renderer)
    : renderer(renderer),
      chatBox(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2),
      map(64, 64), dungeon(500, 3, 10, 3), gFont(nullptr) {
  std::cout << "Game: Constructor started, TTF initialized: " << TTF_WasInit()
            << std::endl;
  tiles.loadDefaults();

  // Load sprite sheet
  SDL_Texture *defaultTexture = nullptr;
  bool spriteSheetLoaded = false;

  // Try to load sprite sheet from assets
  if (tiles.loadSpriteSheet("assets/spritesheet.png", renderer)) {
    std::cout << "Sprite sheet loaded successfully" << std::endl;
    spriteSheetLoaded = true;
  } else {
    std::cerr << "Error loading tile sheet: " << IMG_GetError() << std::endl;

    // Create a fallback texture for tiles (8x8 blue square)
    defaultTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, 8, 8);
    if (defaultTexture) {
      SDL_SetRenderTarget(renderer, defaultTexture);
      SDL_SetRenderDrawColor(renderer, 38, 139, 210, 255); // Blue
      SDL_Rect rect = {0, 0, 8, 8};
      SDL_RenderFillRect(renderer, &rect);
      SDL_SetRenderDrawColor(renderer, 253, 246, 227, 255); // White
      rect = {8, 0, 8, 8};
      SDL_RenderFillRect(renderer, &rect);
      SDL_SetRenderDrawColor(renderer, 133, 153, 0, 255); // Green
      rect = {0, 8, 8, 8};
      SDL_RenderFillRect(renderer, &rect);
      SDL_SetRenderDrawColor(renderer, 220, 50, 47, 255); // Red
      rect = {8, 8, 8, 8};
      SDL_RenderFillRect(renderer, &rect);
      SDL_SetRenderTarget(renderer, nullptr);

      tiles.setSpriteSheet(defaultTexture);
      std::cout << "Created fallback texture for tiles" << std::endl;
      spriteSheetLoaded = true;
    }
  }

  // Set the sprite sheet for the map
  if (spriteSheetLoaded) {
    map.setSpriteSheet(tiles.getSpriteSheet());
  }

  // Create a temporary player
  player.pos.x = 16;
  player.pos.y = 16;
  player.health = 100;
  player.attackDamage = 1;
  player.attackSpeed = 10;
  player.name = "Player";

  // Load font
  TTF_Font *font = nullptr;
  bool fontLoaded = false;

  // Try to load font
  const char *fontPaths[] = {
      "assets/TerminessNerdFontMono-Regular.ttf",
      "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
      "/usr/share/fonts/liberation-mono/LiberationMono-Regular.ttf",
      "/usr/share/fonts/dejavu/DejaVuSansMono.ttf",
      "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"};

  for (const char *path : fontPaths) {
    std::cout << "Trying to load font from: " << path << std::endl;
    font = TTF_OpenFont(path, 16);
    if (font) {
      std::cout << "Successfully loaded font from: " << path << std::endl;
      fontLoaded = true;
      break;
    } else {
      std::cerr << "Failed to load font from " << path << ": " << TTF_GetError()
                << std::endl;
    }
  }

  if (!fontLoaded) {
    std::cerr << "Failed to load any font, using system font as fallback"
              << std::endl;
  }

  if (font) {
    map.setFont(font);
    chatBox.setFont(font); // Set the font for ChatBox as well
    gFont = font;
    std::cout << "Font set for map and chatbox" << std::endl;
  }

  // Manual glyph map (ASCII 32-126 assumed, laid out in 16x16 grid)
  // Glyph map is now handled elsewhere
  std::unordered_map<char, SDL_Rect> glyphMap;

  map.setGlyphMap(glyphMap);
  std::cout << "Glyph map initialized" << std::endl;

  // Generate dungeon using WalkerDungeon
  generateDungeon();
  std::cout << "Dungeon generated" << std::endl;

  // Initialize entities after dungeon generation
  enemy = {"Goblin", {}, 1000, 6, 250};
  ssoyWielder = {"Player with SSoY", {}, 100, 39, 70};
  actionQueue.push({0, &ssoyWielder});
  actionQueue.push({0, &enemy});
  std::cout << "Entities initialized" << std::endl;

  // Add initial messages to the chat box
  chatBox.addMessage("Welcome to the dungeon!");
  chatBox.addMessage("Use arrow keys to move.");
  chatBox.addMessage("Press 'Q' or ESC to quit.");
  chatBox.addMessage("Good luck!");
}

Game::~Game() {
  std::cout << "Game: Starting cleanup" << std::endl;
  if (gFont) {
    std::cout << "Game: Freeing font" << std::endl;
    TTF_CloseFont(gFont);
    gFont = nullptr;
  }
  std::cout << "Game: Cleanup complete" << std::endl;
}

void Game::tick() {
  while (!actionQueue.empty() && actionQueue.top().tick <= tickCount) {
    auto next = actionQueue.top();
    actionQueue.pop();
    Entity *attacker = next.entity;
    Entity *target = (attacker == &ssoyWielder) ? &enemy : &ssoyWielder;

    // Add attack message
    std::string attackMsg =
        attacker->name + " attacks " + target->name + " for ";

    // Determine message color based on who is attacking
    if (attacker == &ssoyWielder) {
      // Player is attacking
      chatBox.addMessage(attackMsg, colorBlue);
    } else {
      // Enemy is attacking
      chatBox.addMessage(attackMsg, colorRed);
    }

    // Add damage value message with colored text based on who is attacking
    std::string damageMsg = std::to_string(attacker->attackDamage) + " damage!";
    if (attacker == &ssoyWielder) {
      // Player attacking - blue text
      chatBox.addMessage(damageMsg, colorBlue);
    } else {
      // Enemy attacking - red text
      chatBox.addMessage(damageMsg, colorRed);
    }

    // Process the attack logic
    target->health -= attacker->attackDamage;
    if (target->health <= 0) {
      // Use yellow for defeat messages
      SDL_Color colorYellow = {255, 255, 0, 255};
      chatBox.addMessage(target->name + " is defeated!", colorYellow);
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

  // Clear the entire screen with the specified background color
  SDL_SetRenderDrawColor(renderer, 0, 43, 54, 255);
  SDL_RenderClear(renderer);

  // Main viewport (full screen for dungeon view)
  SDL_Rect mainViewport = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  drawPanel(mainViewport, padding, border[0], background[0]);

  // Combat log viewport (bottom portion of the screen)
  SDL_Rect bottomViewport = {0, SCREEN_HEIGHT - SCREEN_HEIGHT / 4, SCREEN_WIDTH,
                             SCREEN_HEIGHT / 4};
  drawPanel(bottomViewport, padding - 2, border[0], background[0]);

  // Create an adjusted viewport for the dungeon that accounts for the border
  SDL_Rect mapRect = {mainViewport.x + padding, mainViewport.y + padding,
                      mainViewport.w - 2 * padding,
                      mainViewport.h - 2 * padding};

  // Set viewport for the dungeon
  SDL_RenderSetViewport(renderer, &mapRect);

  // Log the map viewport dimensions and player position
  SDL_Log("Dungeon view: x=%d, y=%d, w=%d, h=%d, Player: (%d,%d)", mapRect.x,
          mapRect.y, mapRect.w, mapRect.h, player.pos.x, player.pos.y);

  // Render the tilemap centered on player's position
  map.renderCentered(renderer, tiles, {0, 0, mapRect.w, mapRect.h},
                     player.pos.x, player.pos.y);

  // Display player stats as an overlay in the top-left corner of the main view
  std::string playerInfo = "Position: (" + std::to_string(player.pos.x) + "," +
                           std::to_string(player.pos.y) + ")   " +
                           "Health: " + std::to_string(player.health);

  // Create a small semi-transparent background for the stats text
  SDL_Rect statsRect = {10, 10, 300, 30};
  SDL_SetRenderDrawColor(renderer, 0, 43, 54, 180);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderFillRect(renderer, &statsRect);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

  // Render the player stats text
  if (gFont) {
    SDL_Color textColor = {253, 246, 227, 255};
    SDL_Surface *surface =
        TTF_RenderText_Solid(gFont, playerInfo.c_str(), textColor);
    if (surface) {
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_Rect dest = {15, 15, surface->w, surface->h};
      SDL_RenderCopy(renderer, texture, nullptr, &dest);
      SDL_FreeSurface(surface);
      SDL_DestroyTexture(texture);
    }
  }

  // Reset viewport for the chat box rendering
  SDL_RenderSetViewport(renderer, nullptr);

  // Make sure the chatBox rect is correctly positioned at the bottom viewport
  chatBox.setRect(bottomViewport);

  // Render the combat log (bottom viewport)
  chatBox.render(renderer);
}

void Game::generateDungeon() {
  // Generate the dungeon using the walker algorithm
  dungeon.GenerateRoomWalker();

  // Get floor and wall lists
  const std::vector<SDL_Point> &floors = dungeon.GetFloorList();
  std::vector<SDL_Point> walls = dungeon.GetWallList();

  SDL_Log("Generated %zu floor tiles and %zu wall tiles", floors.size(),
          walls.size());

  // Clear the current map - set all tiles to empty (0)
  for (int y = 0; y < 64; y++) {
    for (int x = 0; x < 64; x++) {
      map.setTile(x, y, 0); // Empty tile (void space)
    }
  }

  // Find the min/max coordinates to center the dungeon
  int minX = std::numeric_limits<int>::max();
  int minY = std::numeric_limits<int>::max();
  int maxX = std::numeric_limits<int>::min();
  int maxY = std::numeric_limits<int>::min();

  for (const auto &point : floors) {
    minX = std::min(minX, point.x);
    minY = std::min(minY, point.y);
    maxX = std::max(maxX, point.x);
    maxY = std::max(maxY, point.y);
  }
  for (const auto &point : walls) {
    minX = std::min(minX, point.x);
    minY = std::min(minY, point.y);
    maxX = std::max(maxX, point.x);
    maxY = std::max(maxY, point.y);
  }

  int dungeonWidth = maxX - minX + 1;
  int dungeonHeight = maxY - minY + 1;
  int offsetX = (64 - dungeonWidth) / 2 - minX;
  int offsetY = (64 - dungeonHeight) / 2 - minY;

  // First place floor tiles - these will fill the walkable dungeon space
  for (const auto &point : floors) {
    int x = point.x + offsetX;
    int y = point.y + offsetY;
    if (x >= 0 && x < 64 && y >= 0 && y < 64) {
      map.setTile(x, y, 1); // Floor tile
    }
  }

  // Then place walls - these will define the boundaries of the dungeon
  for (const auto &point : walls) {
    int x = point.x + offsetX;
    int y = point.y + offsetY;
    if (x >= 0 && x < 64 && y >= 0 && y < 64) {
      map.setTile(x, y, 3); // Wall tile
    }
  }

  // Log the dungeon layout
  SDL_Log("Dungeon generated with dimensions: %d x %d, offset: (%d, %d)",
          dungeonWidth, dungeonHeight, offsetX, offsetY);

  // Set player starting position to the first floor tile
  if (!floors.empty()) {
    int startX = floors[0].x + offsetX;
    int startY = floors[0].y + offsetY;
    if (startX >= 0 && startX < 64 && startY >= 0 && startY < 64) {
      player = {"Player", Position(startX, startY), 100, 5, 204};
      SDL_Log("Player starting position: %d, %d", startX, startY);
    }
  }
}

void Game::drawPanel(SDL_Rect viewport, int padding, SDL_Color border,
                     SDL_Color background) {
  // Draw the background
  SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b,
                         background.a);
  SDL_RenderFillRect(renderer, &viewport);

  // Draw the border
  SDL_Rect borderRect = {viewport.x - padding, viewport.y - padding,
                         viewport.w + 2 * padding, viewport.h + 2 * padding};
  SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
  SDL_RenderDrawRect(renderer, &borderRect);
}

void Game::flavorText(const char *str, SDL_Rect &tRV) {
  SDL_RenderSetViewport(renderer, &tRV);
  if (gFont) {
    SDL_Color textColor = {253, 246, 227, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(gFont, str, textColor);
    if (surface) {
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_Rect dest = {10, 10, surface->w,
                       surface->h}; // Position the text with padding
      SDL_RenderCopy(renderer, texture, nullptr, &dest);
      SDL_FreeSurface(surface);
      SDL_DestroyTexture(texture);
    } else {
      SDL_Log("Error rendering text: %s", TTF_GetError());
    }
  }
}

int Game::glyphToTileID(char c) {
  switch (c) {
  case '#': // Walls
    return 35;
  case '.': // Floor
    return 131;
  case ' ': // Void space (outside dungeon)
    return 32;
  default: // Default to void space
    return 0;
  }
}

void Game::handleInput(const SDL_Event &event) {
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_UP: {
      int newY = player.pos.y - 1;
      if (isValidMove(player.pos.x, newY)) {
        player.pos.y = newY;
        std::cout << "Player moved to (" << player.pos.x << "," << player.pos.y
                  << ")" << std::endl;
      }
      break;
    }
    case SDLK_DOWN: {
      int newY = player.pos.y + 1;
      if (isValidMove(player.pos.x, newY)) {
        player.pos.y = newY;
        std::cout << "Player moved to (" << player.pos.x << "," << player.pos.y
                  << ")" << std::endl;
      }
      break;
    }
    case SDLK_LEFT: {
      int newX = player.pos.x - 1;
      if (isValidMove(newX, player.pos.y)) {
        player.pos.x = newX;
        std::cout << "Player moved to (" << player.pos.x << "," << player.pos.y
                  << ")" << std::endl;
      }
      break;
    }
    case SDLK_RIGHT: {
      int newX = player.pos.x + 1;
      if (isValidMove(newX, player.pos.y)) {
        player.pos.x = newX;
        std::cout << "Player moved to (" << player.pos.x << "," << player.pos.y
                  << ")" << std::endl;
      }
      break;
    }
    case SDLK_q:
    case SDLK_ESCAPE:
      // Set running to false to signal to GameManager to exit
      running = false;
      std::cout << "Player requested to quit the game" << std::endl;
      break;
    default:
      break;
    }
  }
}

bool Game::isValidMove(int newX, int newY) const {
  // Check if the new position is within map bounds
  if (newX < 0 || newX >= map.getWidth() || newY < 0 ||
      newY >= map.getHeight()) {
    return false;
  }

  // Get the tile at the new position
  int tileID = map.getTile(newX, newY);

  // Allow movement on floor tiles (ID 1) but not on walls (ID 3) or void (ID 0)
  return tileID == 1;
}
