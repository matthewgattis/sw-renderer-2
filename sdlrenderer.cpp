#include "sdlrenderer.hpp"

#include <SDL_render.h>

#include "sdltexture.hpp"
#include "sdlwindow.hpp"

#define LOG_MODULE_NAME ("SDLRenderer")
#include "log.hpp"

SDLRenderer::SDLRenderer(const std::shared_ptr<SDLWindow> &sdl_window) :
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

void SDLRenderer::copy(const std::shared_ptr<SDLTexture> &sdl_texture)
{
    SDL_RenderCopy(
        sdl_renderer_,
        sdl_texture->get(),
        nullptr,
        nullptr);
}

void SDLRenderer::present()
{
    SDL_RenderPresent(sdl_renderer_);
}

std::pair<int, int> SDLRenderer::getResolution() const
{
    int res;
    int w, h;
    res = SDL_GetRendererOutputSize(sdl_renderer_, &w, &h);
    if (res != 0)
    {
        LOG_ERROR << "Failure in SDL_GetRendererOuputSize. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
    }

    return std::make_pair(w, h);
}

