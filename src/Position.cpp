// Position.cpp
#include "Position.hpp"

Position::Position(int x, int y) : x(x), y(y) {}

bool Position::operator==(const Position &other) const {
  return x == other.x && y == other.y;
}

size_t std::hash<Position>::operator()(const Position &p) const {
  return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
}