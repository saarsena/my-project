// GameManager.hpp
#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include "Window.hpp"
#include "Game.hpp"

class GameManager {
public:
    GameManager();
    ~GameManager();

    bool init();
    void run();
    void clean();

private:
    Window window;
    Game* game = nullptr;
    bool isRunning = false;

    void handleEvents();
    void update();
    void render();
};

#endif