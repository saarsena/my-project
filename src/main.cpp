// main.cpp

#include "GameManager.hpp"

int main(int argc, char *argv[]) {
  GameManager gameManager;

  gameManager.init();
  gameManager.run();
  gameManager.clean();

  return 0;
}
