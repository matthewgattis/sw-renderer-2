#include "sdlrenderer.hpp"

#include <SDL_render.h>

#include "sdltexture.hpp"
#include "sdlwindow.hpp"

#define LOG_MODULE_NAME ("SDLRenderer")
#include "log.hpp"

SDLRenderer::SDLRenderer(
    const std::shared_ptr<SDLWindow> &sdl_window)
    :
		sdl_renderer_(nullptr)
{
    LOG_INFO << "Instance created." << std::endl;

    sdl_renderer_ = SDL_CreateRenderer(
        sdl_window->get(),
        -1,
        SDL_RENDERER_PRESENTVSYNC);

    if (!sdl_renderer_)
    {
        LOG_ERROR << "Failure in SDL_CreateRenderer. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
    }
}

SDLRenderer::~SDLRenderer()
{
    if (sdl_renderer_)
        SDL_DestroyRenderer(sdl_renderer_);
}

SDL_Renderer *SDLRenderer::get() const
{
    return sdl_renderer_;
}

void SDLRenderer::renderCopy(
    const std::shared_ptr<SDLTexture> &sdl_texture)
{
    SDL_RenderCopy(
        sdl_renderer_,
        sdl_texture->get(),
        nullptr,
        nullptr);
}

void SDLRenderer::renderPresent()
{
    SDL_RenderPresent(sdl_renderer_);
}

