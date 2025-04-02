// TileDef.h
#ifndef TILEDEF_H
#define TILEDEF_H

#include <cstdint>
#include <string>

class TileDef {
 public:
  int id;
  int sheetX, sheetY;
  uint8_t flags;
  char glyph;
  std::string name;

  TileDef(int id = 0, int sx = 0, int sy = 0, uint8_t flags = 0,
          char glyph = ' ', const std::string &name = "");
};

#endif
