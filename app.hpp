#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class Camera;
class SDLWindow;
class SDLRenderer;
class SDLTexture;
class GameController;

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
    std::shared_ptr<Camera> camera_;
    std::unordered_map<int, std::shared_ptr<GameController>> game_controllers_;
};

