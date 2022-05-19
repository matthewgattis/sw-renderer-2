#pragma once

#include <memory>

#include <SDL.h>

class SDLWindow;
class SDLTexture;

class SDLRenderer
{
public:
    SDLRenderer(
        const std::shared_ptr<SDLWindow> &sdl_window);
    ~SDLRenderer();

    SDL_Renderer *get() const;

    void renderCopy(
        const std::shared_ptr<SDLTexture> &sdl_texture);

    void renderPresent();

private:
    SDL_Renderer *sdl_renderer_;
};

