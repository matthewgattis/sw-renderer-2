#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class SDLWindow;
class SDLRenderer;
class SDLTexture;

class App
{
public:
    App();
    ~App();

    void run(const std::vector<std::string> &args);

private:
    void init();

private:
    std::shared_ptr<SDLWindow> sdl_window_;
    std::shared_ptr<SDLRenderer> sdl_renderer_;
    std::shared_ptr<SDLTexture> sdl_texture_;
};

