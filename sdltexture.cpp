#include "sdltexture.hpp"

#include "sdlrenderer.hpp"

#define LOG_MODULE_NAME ("SDLTexture")
#include "log.hpp"

SDLTexture::SDLTexture(
    const std::shared_ptr<SDLRenderer> &sdl_renderer,
    SDL_TextureAccess access,
    int width,
    int height)
    :
        width_(width),
        height_(height),
        pixels_(nullptr)
{
    LOG_INFO << "Instance created." << std::endl;

    sdl_texture_ = SDL_CreateTexture(
        sdl_renderer->get(),
        SDL_PIXELFORMAT_RGBA8888,
        access,
        width,
        height);

    if (!sdl_texture_)
    {
        LOG_ERROR << "Failure in SDL_CreateTexture. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
    }

    pixels_ = new unsigned char[4 * (size_t)width * (size_t)height]();
}

SDLTexture::~SDLTexture()
{
    if (pixels_)
        delete[] pixels_;

    if (sdl_texture_)
        SDL_DestroyTexture(sdl_texture_);
}

SDL_Texture *SDLTexture::get() const
{
    return sdl_texture_;
}

void SDLTexture::updateTexture()
{
    SDL_UpdateTexture(
        sdl_texture_,
        nullptr,
        pixels_,
        4 * width_);
}

unsigned char *SDLTexture::getPixels() const
{
    return pixels_;
}

int SDLTexture::getWidth() const
{
    return width_;
}

int SDLTexture::getHeight() const
{
    return height_;
}

