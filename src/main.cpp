// main.cpp
#include "GameManager.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  GameManager gameManager;

  gameManager.init();
  gameManager.run();
  gameManager.clean();

  return 0;
}
