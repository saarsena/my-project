// WalkerDungeon.cpp
#include "walkerdungeon.hpp"

#include <random>

// Constructor: Initialize parameters and seed the RNG.
WalkerDungeon::WalkerDungeon(int totalFloorCount, int minHall, int maxHall,
                             int roomDim)
    : totalFloorCount(totalFloorCount),
      minHall(minHall),
      maxHall(maxHall),
      roomDim(roomDim),
      rng(std::random_device()()) {}

// Returns the generated floor positions
const std::vector<SDL_Point> &WalkerDungeon::GetFloorList() const {
  return floorList;
}

// Check if the position already exists in the floor list
bool WalkerDungeon::InFloorList(
    const SDL_Point &pos) const {  // Make this method const
  for (const auto &p : floorList) {
    if (p.x == pos.x && p.y == pos.y) return true;
  }
  return false;
}

// Returns a random cardinal direction
SDL_Point WalkerDungeon::RandomDirection() {
  int r = std::uniform_int_distribution<int>(1, 4)(rng);
  switch (r) {
    case 1:
      return {0, 1};  // Up
    case 2:
      return {1, 0};  // Right
    case 3:
      return {0, -1};  // Down
    case 4:
      return {-1, 0};  // Left
    default:
      return {0, 0};
  }
}

// Create a random room around a position
void WalkerDungeon::RandomRoom(const SDL_Point &pos) {
  int width = std::uniform_int_distribution<int>(1, roomDim)(rng);
  int height = std::uniform_int_distribution<int>(1, roomDim)(rng);

  for (int w = -width; w <= width; ++w) {
    for (int h = -height; h <= height; ++h) {
      SDL_Point newPos = {pos.x + w, pos.y + h};
      if (!InFloorList(newPos)) {
        floorList.push_back(newPos);
      }
    }
  }
}

// RandomWalker: One-step-at-a-time random walk
void WalkerDungeon::GenerateRandomWalker() {
  floorList.clear();
  SDL_Point curPos = {0, 0};
  floorList.push_back(curPos);

  while (static_cast<int>(floorList.size()) < totalFloorCount) {
    SDL_Point dir = RandomDirection();
    curPos.x += dir.x;
    curPos.y += dir.y;
    if (!InFloorList(curPos)) {
      floorList.push_back(curPos);
    }
  }
}

// RoomWalker: Creates halls of random length then adds a room at the end.
void WalkerDungeon::GenerateRoomWalker() {
  floorList.clear();
  SDL_Point curPos = {0, 0};
  floorList.push_back(curPos);

  while (static_cast<int>(floorList.size()) < totalFloorCount) {
    SDL_Point walkDir = RandomDirection();
    int walkLength = std::uniform_int_distribution<int>(minHall, maxHall)(rng);

    for (int i = 0; i < walkLength; i++) {
      SDL_Point nextPos = {curPos.x + walkDir.x, curPos.y + walkDir.y};
      if (!InFloorList(nextPos)) {
        floorList.push_back(nextPos);
      }
      curPos = nextPos;
    }
    RandomRoom(curPos);
  }
}

std::vector<SDL_Point> WalkerDungeon::GetWallList() const {
  std::vector<SDL_Point> wallList;
  // Check all 8 neighbors for each floor tile.
  const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
  const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
  for (const auto &p : floorList) {
    for (int i = 0; i < 8; ++i) {
      SDL_Point neighbor = {p.x + dx[i], p.y + dy[i]};
      if (!InFloorList(neighbor)) {  // This call is now valid
        // Avoid duplicates.
        bool alreadyAdded = false;
        for (const auto &wp : wallList) {
          if (wp.x == neighbor.x && wp.y == neighbor.y) {
            alreadyAdded = true;
            break;
          }
        }
        if (!alreadyAdded) {
          wallList.push_back(neighbor);
        }
      }
    }
  }
  return wallList;
}
