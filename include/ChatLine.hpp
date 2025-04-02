#pragma once
#include <SDL2/SDL.h>

#include <string>

class ChatLine {
 public:
  std::string text;
  SDL_Color color;

  ChatLine(const std::string &t, SDL_Color c = {255, 255, 255, 255})
      : text(t), color(c) {}
};
