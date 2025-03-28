// Window.hpp
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL2/SDL.h>
#include <string>

class Window {
public:
    Window();
    ~Window();

    bool init(const std::string& title, int width, int height);
    SDL_Renderer* getRenderer();
    SDL_Window* getWindow();

    void clear();
    void present();
    void clean();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

#endif