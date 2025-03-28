// Window.cpp
#include "Window.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

Window::Window() {}
Window::~Window() { clean(); }

bool Window::init(const std::string& title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;
    if (!IMG_Init(IMG_INIT_PNG)) return false;
    if (TTF_Init() == -1) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    return true;
}

SDL_Window* Window::getWindow() { return window; }
SDL_Renderer* Window::getRenderer() { return renderer; }
void Window::clear() { SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer); }
void Window::present() { SDL_RenderPresent(renderer); }
void Window::clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit(); TTF_Quit(); IMG_Quit(); SDL_Quit();
}