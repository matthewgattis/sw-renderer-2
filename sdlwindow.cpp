#include "sdlwindow.hpp"

#include <SDL.h>

#define LOG_MODULE_NAME ("SDLWindow")
#include "log.hpp"

const std::vector<std::pair<int, int>> SDLWindow::default_resolution_list_ = 
{
    std::pair<int, int>(320, 240),
    std::pair<int, int>(640, 480),
    std::pair<int, int>(800, 600),
    std::pair<int, int>(1024, 768),
    std::pair<int, int>(1280, 960),
    std::pair<int, int>(1440, 1080),
    std::pair<int, int>(1600, 1200),
    std::pair<int, int>(2048, 1536)
};

SDLWindow::SDLWindow(const std::string &window_title) :
    sdl_window_(nullptr),
    window_title_(window_title)
{
    LOG_INFO << "Instance created." << std::endl;

    SDL_DisplayMode display_mode;
    SDL_GetDesktopDisplayMode(0, &display_mode);

    float ddpi;
    float hdpi;
    float vdpi;
    int sdl_result = SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi);
    if (sdl_result)
    {
        LOG_WARNING << "Failure in SDL_GetDisplayDPI. (" << SDL_GetError() << ")" << std::endl;
    }
    else
    {
        LOG_INFO << "Display DPI ddpi (" << ddpi << ")" << std::endl;
        LOG_INFO << "Display DPI hdpi (" << hdpi << ")" << std::endl;
        LOG_INFO << "Display DPI vdpi (" << vdpi << ")" << std::endl;
    }

    int selection = 0;
    for (int i = 0; i < default_resolution_list_.size(); i++)
    {
        if (default_resolution_list_[i].first < display_mode.w &&
            default_resolution_list_[i].second < display_mode.h)
        {
            selection = i;
            continue;
        }
        break;
    }

    resolution_selection_ = selection;

    LOG_INFO << "Window width (" << default_resolution_list_[selection].first << ")" << std::endl;
    LOG_INFO << "Window height (" << default_resolution_list_[selection].second << ")" << std::endl;

    sdl_window_ = SDL_CreateWindow(
        window_title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        default_resolution_list_[selection].first,
        default_resolution_list_[selection].second,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!sdl_window_)
    {
        LOG_ERROR << "Failure in SDL_CreateWindow. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
    }
}

SDLWindow::~SDLWindow()
{
    if (sdl_window_)
        SDL_DestroyWindow(sdl_window_);
}

SDL_Window *SDLWindow::get() const
{
    return sdl_window_;
}

const std::pair<int, int> &SDLWindow::getDefaultResolution() const
{
    return default_resolution_list_[resolution_selection_];
}

void SDLWindow::setWindowTitle(const std::string& window_title)
{
    SDL_SetWindowTitle(sdl_window_, (window_title_ + " " + window_title).c_str());
}

