#include "sdltexture.hpp"

#include <ranges>

#include <SDL_render.h>

#include "sdlrenderer.hpp"

#define LOG_MODULE_NAME ("SDLTexture")
#include "log.hpp"

SDLTexture::SDLTexture(
    const std::shared_ptr<SDLRenderer> &sdl_renderer,
    int width,
    int height,
    uint32_t clear_color) :
        width_(width),
        clear_color_(clear_color),
        pixels_(width * height, clear_color),
        sdl_texture_(nullptr)
{
    LOG_INFO << "Instance created." << std::endl;

    sdl_texture_ = SDL_CreateTexture(
        sdl_renderer->get(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STATIC,
        width,
        height);

    if (!sdl_texture_)
    {
        LOG_ERROR << "Failure in SDL_CreateTexture. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
    }
}

SDLTexture::~SDLTexture()
{
    if (sdl_texture_)
        SDL_DestroyTexture(sdl_texture_);
}

void SDLTexture::update()
{
    SDL_UpdateTexture(sdl_texture_, nullptr, pixels_.data(), 4 * width_);
}

void SDLTexture::clear()
{
    std::ranges::fill(pixels_, clear_color_);
}

