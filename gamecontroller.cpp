#include "gamecontroller.hpp"

#define LOG_MODULE_NAME ("App")
#include "log.hpp"

GameController::GameController(int id) :
	id_(id)
{
	if (!SDL_IsGameController(id))
	{
        LOG_ERROR << "Failure in SDL_IsGameController. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
	}

	game_controller_ = SDL_GameControllerOpen(id);
	if (game_controller_ == nullptr)
	{
        LOG_ERROR << "Failure in SDL_GameControllerOpen. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
	}

	joystick_ = SDL_GameControllerGetJoystick(game_controller_);
	if (game_controller_ == nullptr)
	{
        LOG_ERROR << "Failure in SDL_GameControllerGetJoystick. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
	}

	joystick_instance_id_ = SDL_JoystickInstanceID(joystick_);
	if (joystick_instance_id_ < 0)
	{
        LOG_ERROR << "Failure in SDL_JoystickInstanceID. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
	}
}

void GameController::handleEvent(const SDL_Event& e)
{
	switch (e.type)
	{
	case SDL_CONTROLLERBUTTONDOWN:
		if (e.cdevice.which == id_)
			buttons_[e.cbutton.button] = true;
		break;
	case SDL_CONTROLLERBUTTONUP:
		if (e.cdevice.which == id_)
			buttons_[e.cbutton.button] = false;
		break;
	case SDL_CONTROLLERAXISMOTION:
		if (e.cdevice.which == id_)
			axis_[e.caxis.axis] = (double)e.caxis.value / 32768.0;
	default:
		break;
	}
}

bool GameController::getButton(SDL_GameControllerButton button)
{
	if (buttons_.count(button))
		return buttons_.at(button);
	return false;
}

double GameController::getAxis(SDL_GameControllerAxis axis)
{
	if (axis_.count(axis))
		return axis_.at(axis);
	return 0.0;
}

GameController::~GameController()
{
	SDL_GameControllerClose(game_controller_);
}

