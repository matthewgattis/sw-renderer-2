#pragma once

#include <memory>
#include <utility>

#include <SDL.h>

class SDLWindow;
class SDLTexture;

class SDLRenderer
{
public:
    SDLRenderer(const std::shared_ptr<SDLWindow> &sdl_window);
    ~SDLRenderer();

    SDL_Renderer *get() const { return sdl_renderer_; }

    std::pair<int, int> getResolution() const;

    void copy(const std::shared_ptr<SDLTexture> &sdl_texture);

    void present();

private:
    SDL_Renderer *sdl_renderer_;

    std::pair<int, int> resolution_;
};

