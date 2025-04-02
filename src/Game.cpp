// === Game.cpp ===
#include "Game.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <limits>
#include <unordered_map>

#include "ChatBox.hpp"
#include "Player.hpp"
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
      chatBox(0, GameConfig::GAME_AREA_HEIGHT, GameConfig::SCREEN_WIDTH,
              GameConfig::SCREEN_HEIGHT - GameConfig::GAME_AREA_HEIGHT),
      map(64, 64), dungeon(500, 3, 10, 3),
      camera(GameConfig::GAME_AREA_WIDTH, GameConfig::GAME_AREA_HEIGHT),
      gFont(nullptr) {
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

  // Initial position for the player
  int playerX = 16;
  int playerY = 16;

  // Create player instance
  player = new Player(playerX, playerY, tiles.getSpriteSheet());

  // Initialize other entities
  enemy = Entity("Goblin", Position(0, 0), 1000, 6, 250, 30);
  ssoyWielder = Entity("Player with SSoY", Position(0, 0), 100, 39, 70, 15);

  // Initialize combat queue
  actionQueue.push({0, &ssoyWielder});
  actionQueue.push({0, &enemy});

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

  // Add initial messages to the chat box
  chatBox.addMessage("Woooohoooooo!");
  chatBox.addMessage("Use arrow keys to move.");
  chatBox.addMessage("Press 'Q' or ESC to quit.");
  chatBox.addMessage("Good luck!");
}

Game::~Game() {
  std::cout << "Game: Starting cleanup" << std::endl;

  // Delete player object
  delete player;

  if (gFont) {
    std::cout << "Game: Freeing font" << std::endl;
    TTF_CloseFont(gFont);
    gFont = nullptr;
  }
  std::cout << "Game: Cleanup complete" << std::endl;
}

void Game::tick() {
  // Process combat actions
  while (!actionQueue.empty() && actionQueue.top().tick <= tickCount) {
    auto next = actionQueue.top();
    actionQueue.pop();
    Entity *attacker = next.entity;
    Entity *target = (attacker == &ssoyWielder) ? &enemy : &ssoyWielder;

    // Add attack message
    std::string attackMsg =
        attacker->getName() + " attacks " + target->getName() + " for ";

    // Determine message color based on who is attacking
    if (attacker == &ssoyWielder) {
      // Player is attacking
      chatBox.addMessage(attackMsg, colorBlue);
    } else {
      // Enemy is attacking
      chatBox.addMessage(attackMsg, colorRed);
    }

    // Add damage value message with colored text based on who is attacking
    std::string damageMsg =
        std::to_string(attacker->getAttackDamage()) + " damage!";
    if (attacker == &ssoyWielder) {
      // Player attacking - blue text
      chatBox.addMessage(damageMsg, colorBlue);
    } else {
      // Enemy attacking - red text
      chatBox.addMessage(damageMsg, colorRed);
    }

    // Process the attack logic
    target->takeDamage(attacker->getAttackDamage());
    if (target->getHealth() <= 0) {
      // Use yellow for defeat messages
      SDL_Color colorYellow = {255, 255, 0, 255};
      chatBox.addMessage(target->getName() + " is defeated!", colorYellow);
      running = false;
      return;
    }

    int nextTick = tickCount + attacker->getAttackSpeed();
    actionQueue.push({nextTick, attacker});
  }

  // Update all entities
  player->update(this, tickCount);

  // Increment tick counter
  tickCount++;
}

void Game::update() {
  // Update camera to follow player using TILE_SIZE from GameConfig
  const Position &playerPos = player->getPosition();
  camera.update(playerPos.x * GameConfig::TILE_SIZE,
                playerPos.y * GameConfig::TILE_SIZE,
                map.getWidth() * GameConfig::TILE_SIZE,
                map.getHeight() * GameConfig::TILE_SIZE);
}

void Game::render() {
  // Clear the entire screen
  SDL_SetRenderDrawColor(renderer, 0, 43, 54, 255);
  SDL_RenderClear(renderer);

  // Calculate dynamic sections - divide screen height into thirds
  int totalHeight = GameConfig::SCREEN_HEIGHT;
  int baseSectionHeight = totalHeight / 3;
  int remainder = totalHeight % 3;

  // Distribute heights for three equal sections (top, middle, bottom)
  int sectionHeights[3] = {baseSectionHeight, baseSectionHeight,
                           baseSectionHeight};

  // Distribute any remainder pixels to maintain exact total height
  for (int i = 0; i < remainder; ++i) {
    sectionHeights[i]++;
  }

  // Chat box takes bottom third, game area takes top two-thirds
  int chatBoxHeight = sectionHeights[2];
  int gameAreaHeight = sectionHeights[0] + sectionHeights[1];

  // Use GAME_VIEW dimensions for logical view size
  int logicalWidth = GameConfig::GAME_VIEW_WIDTH;
  int logicalHeight = GameConfig::GAME_VIEW_HEIGHT;

  // Calculate scale to fit in the available space while maintaining aspect
  // ratio
  float scaleX = static_cast<float>(GameConfig::SCREEN_WIDTH) / logicalWidth;
  float scaleY = static_cast<float>(gameAreaHeight) / logicalHeight;
  float scale = std::min(scaleX, scaleY);

  // Calculate the actual dimensions after scaling
  int actualWidth = static_cast<int>(logicalWidth * scale);
  int actualHeight = static_cast<int>(logicalHeight * scale);

  // Center the view horizontally and vertically in the available space
  int offsetX = (GameConfig::SCREEN_WIDTH - actualWidth) / 2;
  int offsetY = (gameAreaHeight - actualHeight) / 2; // Center in top two-thirds

  // Create the centered viewport rectangle
  SDL_Rect centeredRect = {offsetX, offsetY, actualWidth, actualHeight};

  // Log detailed viewport information
  SDL_Log("DYNAMIC THIRDS: Screen: %dx%d, Game area: %d, Chat: %d, "
          "View: %dx%d, Scale: %.2f, Rendered at: %dx%d, Position: (%d,%d)",
          GameConfig::SCREEN_WIDTH, totalHeight, gameAreaHeight, chatBoxHeight,
          logicalWidth, logicalHeight, scale, actualWidth, actualHeight,
          offsetX, offsetY);

  // Draw a background for the game view area
  SDL_SetRenderDrawColor(renderer, 0, 23, 25, 255); // Dark background
  SDL_RenderFillRect(renderer, &centeredRect);

  // Draw a border around the game view area
  SDL_SetRenderDrawColor(renderer, 253, 246, 227, 255); // White border
  SDL_RenderDrawRect(renderer, &centeredRect);

  // Render the map in the centered area using absolute coordinates
  map.renderCenteredAt(renderer, tiles, offsetX, offsetY, actualWidth,
                       actualHeight,
                       player->getPosition().x * GameConfig::TILE_SIZE,
                       player->getPosition().y * GameConfig::TILE_SIZE);

  // Position the chat box at the bottom of the screen
  SDL_Rect chatBoxRect = {0, gameAreaHeight, GameConfig::SCREEN_WIDTH,
                          chatBoxHeight};
  chatBox.setRect(chatBoxRect);

  // Render the combat log
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
      player->setPosition(Position(startX, startY));
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
  // Pass input events to the player
  player->handleInput(event);

  // Check for quit events
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
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
