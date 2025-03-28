// Position.h
#ifndef POSITION_H
#define POSITION_H

#include <functional>

class Position {
public:
  int x, y;
  Position(int x = 0, int y = 0);
  bool operator==(const Position &other) const;
};

namespace std {
template <> struct hash<Position> {
  size_t operator()(const Position &p) const;
};
} // namespace std

#endif