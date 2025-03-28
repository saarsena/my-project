// TileDef.cpp
#include "TileDef.hpp"

TileDef::TileDef(int id, int sx, int sy, uint8_t flags, char glyph, const std::string &name)
  : id(id), sheetX(sx), sheetY(sy), flags(flags), glyph(glyph), name(name) {}