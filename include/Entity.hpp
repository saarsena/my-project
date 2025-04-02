// Entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include "Position.hpp"
#include <string>

// Forward declaration
class Game;

// Movement direction enum
enum class Direction { NONE, UP, DOWN, LEFT, RIGHT };

class Entity {
public:
  Entity(const std::string &name = "",
         const Position &position = Position(0, 0), int health = 100,
         int attackDamage = 5, int attackSpeed = 204, int moveSpeed = 20);
  virtual ~Entity() = default;

  // Virtual methods for derived classes to override
  virtual void update(Game *game, int currentTick);
  virtual bool move(Direction direction, Game *game);
  virtual void takeDamage(int amount);

  // Getters
  const std::string &getName() const { return name; }
  const Position &getPosition() const { return pos; }
  int getHealth() const { return health; }
  int getAttackDamage() const { return attackDamage; }
  int getAttackSpeed() const { return attackSpeed; }
  int getMoveSpeed() const { return moveSpeed; }
  Direction getDirection() const { return direction; }
  int getLastMoveTick() const { return lastMoveTick; }

  // Setters
  void setName(const std::string &newName) { name = newName; }
  void setPosition(const Position &newPos) { pos = newPos; }
  void setPosition(int x, int y) {
    pos.x = x;
    pos.y = y;
  }
  void setHealth(int newHealth) { health = newHealth; }
  void setAttackDamage(int newDamage) { attackDamage = newDamage; }
  void setAttackSpeed(int newSpeed) { attackSpeed = newSpeed; }
  void setMoveSpeed(int newSpeed) { moveSpeed = newSpeed; }
  void setDirection(Direction newDirection) { direction = newDirection; }
  void setLastMoveTick(int tick) { lastMoveTick = tick; }

  // Basic movement
  bool canMove(int currentTick) const;

  // Data members
  std::string name;
  Position pos;
  int health;
  int attackDamage;
  int attackSpeed;

protected:
  // Movement properties
  int moveSpeed;       // Ticks between movements (lower = faster)
  Direction direction; // Current movement direction
  int lastMoveTick;    // Last tick when movement occurred
};

#endif
