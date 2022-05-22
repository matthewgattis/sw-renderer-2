#pragma once

#include <memory>
#include <vector>

#include <SDL.h>

class SDLRenderer;

class SDLTexture
{
public:
    SDLTexture(
        const std::shared_ptr<SDLRenderer> &sdl_renderer,
        SDL_TextureAccess access,
        int width,
        int height);

    ~SDLTexture();

    SDL_Texture *get() const;

    void updateTexture();

public:
    unsigned char *getPixels() const;
    int getWidth() const;
    int getHeight() const;
    void clear();

private:
    SDL_Texture *sdl_texture_;

    unsigned char *pixels_;

    int width_;
    int height_;
};
