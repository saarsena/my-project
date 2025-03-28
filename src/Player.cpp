#include "Player.hpp"
#include <SDL.h>

Player::Player(int x, int y, SDL_Texture* texture)
    : x(x), y(y), texture(texture) {}

void Player::handleInput(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_w: y--; break;
            case SDLK_s: y++; break;
            case SDLK_a: x--; break;
            case SDLK_d: x++; break;
        }
    }
}

void Player::update() {
    // Add future logic here
}

void Player::render(SDL_Renderer* renderer) {
    SDL_Rect dest = { x * 8, y * 8, 8, 8 };
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
}
