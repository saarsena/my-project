// Position.cpp
#include "Position.hpp"

Position::Position(int x, int y) : x(x), y(y) {}

bool Position::operator==(const Position &other) const {
  return x == other.x && y == other.y;
}