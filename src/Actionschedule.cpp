// ActionSchedule.cpp
#include "ActionSchedule.hpp"
#include <SDL2/SDL.h>

bool ActionSchedule::operator<(const ActionSchedule &a) const {
  return tick > a.tick;
}

SDL_Color chatborder[1] = {{255, 255, 255, 255}};
SDL_Color chatbackground[1] = {{0, 0, 0, 255}};
