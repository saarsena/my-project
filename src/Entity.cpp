#include "Entity.hpp"
#include "Game.hpp"
#include <iostream>

Entity::Entity(const std::string &name, const Position &position, int health,
               int attackDamage, int attackSpeed, int moveSpeed)
    : name(name), pos(position), health(health), attackDamage(attackDamage),
      attackSpeed(attackSpeed), moveSpeed(moveSpeed),
      direction(Direction::NONE), lastMoveTick(0) {}

void Entity::update(Game *game, int currentTick) {
  // Base implementation just checks for movement
  if (direction != Direction::NONE && canMove(currentTick)) {
    move(direction, game);
  }
}

bool Entity::move(Direction moveDirection, Game *game) {
  // Calculate new position based on direction
  int newX = pos.x;
  int newY = pos.y;

  switch (moveDirection) {
  case Direction::UP:
    newY--;
    break;
  case Direction::DOWN:
    newY++;
    break;
  case Direction::LEFT:
    newX--;
    break;
  case Direction::RIGHT:
    newX++;
    break;
  default:
    return false;
  }

  // Check if move is valid using the game's validation
  if (game->isValidMove(newX, newY)) {
    pos.x = newX;
    pos.y = newY;
    lastMoveTick = game->getCurrentTick();
    std::cout << name << " moved to (" << pos.x << "," << pos.y << ") at tick "
              << lastMoveTick << std::endl;
    return true;
  }
  return false;
}

void Entity::takeDamage(int amount) {
  health -= amount;
  std::cout << name << " took " << amount << " damage. Health: " << health
            << std::endl;
}

bool Entity::canMove(int currentTick) const {
  return (currentTick - lastMoveTick) >= moveSpeed;
}