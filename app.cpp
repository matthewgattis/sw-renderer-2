#include "app.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_render.h>
#include <limits>
#include <memory>

#include "sdlwindow.hpp"
#include "sdlrenderer.hpp"
#include "sdltexture.hpp"
#include "triangle.hpp"

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

void inline triangle2(
    const glm::dvec3& a,
    const glm::dvec3& b,
    const glm::dvec3& c,
    std::function<void(int x, int y, double z)> dotproc)
{
    triangle(
        a.x, a.y, a.z,
        b.x, b.y, b.z,
        c.x, c.y, c.z,
        dotproc);
}

void App::run(const std::vector<std::string> &args)
{
    init();

    bool run = true;
    int ellapsed = 0;

	int mouse_x = 0;
	int mouse_y = 0;

    int frame_delay = 1;
    int frame_count = 0;

    bool mouse_button_down = false;

	constexpr double RAD = glm::pi<double>() / 180.0;
	constexpr glm::dmat4 identity = glm::identity<glm::dmat4>();

	const glm::dmat4 m1 = glm::translate(identity, glm::dvec3(0.0, 0.0, 5.0));
    glm::dmat4 m2 = identity;

    while (run)
    {
        int frame_start = SDL_GetTicks();

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                run = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == 3)
					mouse_button_down = e.button.state == SDL_PRESSED;
                break;
            case SDL_MOUSEMOTION:
                if (mouse_button_down)
                {
                    m2 = glm::rotate(identity, -e.motion.yrel / 8.0 * RAD, glm::dvec3(1.0, 0.0, 0.0)) * m2;
                    m2 = glm::rotate(identity, e.motion.xrel / 8.0 * RAD, glm::dvec3(0.0, 1.0, 0.0)) * m2;
                }
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

        sdl_texture_->clear();

        int width = sdl_texture_->getWidth();
        int height = sdl_texture_->getHeight();
        unsigned char* pixels = sdl_texture_->getPixels();
        auto ai = std::chrono::steady_clock::now();
        glm::dmat4 v(
            width / 2.0, 0.0, 0.0, 0.0,
            0.0, height / 2.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            (width - 1) / 2.0, (height - 1) / 2.0, 0.0, 1.0);
        glm::dmat4 p = glm::perspective(27.0 * RAD, (double)width / (double)height, 0.1, 10.0);
        //glm::dmat4 m2 = glm::rotate(identity, ellapsed / 1000.0, glm::normalize(glm::dvec3(1.0, 1.0, 1.0)));
        glm::mat<3, 4, glm::f64, glm::packed_highp> vs;
        vs[0] = glm::dvec4(1.0, 0.0, 0.0, 1.0);
        vs[1] = glm::dvec4(cos(120.0 * RAD), -sin(120.0 * RAD), 0.0, 1.0);
        vs[2] = glm::dvec4(cos(120.0 * RAD), sin(120.0 * RAD), 0.0, 1.0);
        vs = p * m1 * m2 * vs;
        for (int i = 0; i < vs.length(); i++)
        {
            vs[i] /= vs[i].w;
        }
        vs = v * vs;
        triangle2(
            vs[0],
            vs[1],
            vs[2],
            [width, height, pixels](int x, int y, double z)
            {
                if (
                    x >= 0 && x < width &&
                    y >= 0 && y < height)
                {
                    int idx = 4 * (x + y * width);
                    pixels[idx + 1] = 255;
                    pixels[idx + 2] = 255;
                    pixels[idx + 3] = 255;
                }
            });
        auto bi = std::chrono::steady_clock::now();

        sdl_texture_->updateTexture();
        sdl_renderer_->renderCopy(sdl_texture_);
        sdl_renderer_->renderPresent();

        const int MIN_FRAME = 1;
        int frame_end = SDL_GetTicks();
        SDL_Delay(std::max(0, MIN_FRAME - (frame_end - frame_start)));
        frame_delay = SDL_GetTicks() - frame_start;
        ellapsed += frame_delay;

        frame_count++;

        sdl_window_->setWindowTitle(std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(bi - ai).count() / 1000.0));
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

