// Entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "Position.hpp"

struct Entity {
  std::string name;
  Position pos;
  int health = 100;
  int attackDamage = 5;
  int attackSpeed = 204;
};

#endif