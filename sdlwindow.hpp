#pragma once

#include <string>
#include <utility>
#include <vector>

#include <SDL.h>

class SDLWindow
{
public:
    SDLWindow(const std::string &window_title);
    ~SDLWindow();

    SDL_Window *get() const;

    const std::pair<int, int> &getDefaultResolution() const;

    void setWindowTitle(const std::string &title);

private:
    SDL_Window *sdl_window_;

private:
    static const std::vector<std::pair<int, int>> default_resolution_list_;

    int resolution_selection_;

    std::string window_title_;
};

