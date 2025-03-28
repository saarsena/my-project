// main.cpp
#include "GameManager.hpp"

int main() {

  GameManager gameManager;

  gameManager.init();
  gameManager.run();
  gameManager.clean();

  return 0;
}
