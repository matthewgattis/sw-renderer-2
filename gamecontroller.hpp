#pragma once

#include <unordered_map>

#include <SDL.h>

class GameController
{
public:
    GameController(int id);
    ~GameController();

    void handleEvent(const SDL_Event& e);

    bool getButton(SDL_GameControllerButton button);
    
    double getAxis(SDL_GameControllerAxis axis);

private:
    int id_;
    SDL_GameController* game_controller_;
    SDL_Joystick* joystick_;
    int joystick_instance_id_;

    std::unordered_map<int, bool> buttons_;
    std::unordered_map<int, double> axis_;
};

