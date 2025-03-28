#pragma once
#include <SDL.h>

class Player {
public:
    Player(int x, int y, SDL_Texture* texture);
    void handleInput(const SDL_Event& e);
    void update();
    void render(SDL_Renderer* renderer);

private:
    int x, y;
    SDL_Texture* texture;
};
