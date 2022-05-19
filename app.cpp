#include "app.hpp"

#include <SDL.h>
#include <SDL_render.h>
#include <limits>
#include <memory>

#include "sdlwindow.hpp"
#include "sdlrenderer.hpp"
#include "sdltexture.hpp"

#define LOG_MODULE_NAME ("App")
#include "log.hpp"

App::App()
{
    LOG_INFO << "Instance created." << std::endl;
}

App::~App()
{
    SDL_Quit();
}

void App::run(const std::vector<std::string> &args)
{
    init();

    bool run = true;
    int ellapsed = 0;
    while (run)
    {
        int frame_start = SDL_GetTicks();
        int frame_delay = 4;
        
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                run = false;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    sdl_texture_ = std::make_shared<SDLTexture>(
                        sdl_renderer_,
                        SDL_TEXTUREACCESS_STREAMING,
                        e.window.data1,
                        e.window.data2);
            default:
                break;
            }
        }

        sdl_texture_->updateTexture();
        sdl_renderer_->renderCopy(sdl_texture_);
        sdl_renderer_->renderPresent();

        int frame_end = SDL_GetTicks();
        SDL_Delay(std::max(0, 4 - (frame_end - frame_start)));
        frame_delay = std::max(4, frame_end - frame_start);
        ellapsed += frame_delay;
    }
}

void App::init()
{
    int result;
    result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    if (result)
    {
        LOG_ERROR << "Failure in SDL_Init. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
    }
    sdl_window_ = std::make_shared<SDLWindow>("sw-renderer");
    sdl_renderer_ = std::make_shared<SDLRenderer>(sdl_window_);
    sdl_texture_ = std::make_shared<SDLTexture>(
        sdl_renderer_,
        SDL_TEXTUREACCESS_STREAMING,
        sdl_window_->getDefaultResolution().first,
        sdl_window_->getDefaultResolution().second);
}

