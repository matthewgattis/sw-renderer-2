#include "app.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_render.h>
#include <limits>
#include <memory>
#include <functional>

#include "camera.hpp"
#include "mesh.hpp"
#include "sdlwindow.hpp"
#include "sdlrenderer.hpp"
#include "sdltexture.hpp"
#include "triangle.hpp"
#include "gamecontroller.hpp"
#include "teapot.hpp"

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

void xform(const glm::dmat3& m, std::vector<glm::dvec3> &ns)
{
    for (auto& n : ns)
        n = m * n;
    ns.push_back(glm::dvec3(0.0, 0.0, 0.0));
}

void xform(const glm::dmat4& m, std::vector<glm::dvec4>& vs)
{
    for (auto& v : vs)
        v = m * v;
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

    int mouse_button = 0;

    Mesh mesh;
    {
        int triangle_count = teapot_indices.size() / 3;
        for (int i = 0; i < triangle_count; i++)
        {
            int index = 3 * i;
            mesh.addTriangle(
                mesh.addVertex(glm::dvec3(
                    teapot_vertices[3 * (teapot_indices[index + 0]) + 0],
                    teapot_vertices[3 * (teapot_indices[index + 0]) + 1],
                    teapot_vertices[3 * (teapot_indices[index + 0]) + 2])),
                mesh.addVertex(glm::dvec3(
                    teapot_vertices[3 * (teapot_indices[index + 1]) + 0],
                    teapot_vertices[3 * (teapot_indices[index + 1]) + 1],
                    teapot_vertices[3 * (teapot_indices[index + 1]) + 2])),
                mesh.addVertex(glm::dvec3(
                    teapot_vertices[3 * (teapot_indices[index + 2]) + 0],
                    teapot_vertices[3 * (teapot_indices[index + 2]) + 1],
                    teapot_vertices[3 * (teapot_indices[index + 2]) + 2])),
                glm::dvec3(1.0, 1.0, 1.0));
        }
    }

    constexpr double RAD = glm::pi<double>() / 180.0;
    constexpr glm::dmat4 identity = glm::identity<glm::dmat4>();

    std::vector<double> depth;
    depth.resize(sdl_texture_->getWidth()* sdl_texture_->getHeight());

    {
        int res = SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
        if (res == -1)
        {
            LOG_ERROR << "Failure in SDL_GameControllerAddMappingsFromFile. (" << SDL_GetError() << ")" << std::endl;
            throw std::exception();
        }
    }
    {
        int res = SDL_NumJoysticks();
        if (res < 0)
        {
            LOG_ERROR << "Failure in SDL_NumJoysticks. (" << SDL_GetError() << ")" << std::endl;
            throw std::exception();
        }
        for (int i = 0; i < res; i++)
            if (SDL_IsGameController(i) == SDL_TRUE)
            {
                game_controllers_.emplace(i, std::make_shared<GameController>(i));
            }
    }

    while (run)
    {
        int frame_start = SDL_GetTicks();

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            for (auto& game_controller : game_controllers_)
                game_controller.second->handleEvent(e);

            switch (e.type)
            {
            case SDL_QUIT:
                run = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse_button = e.button.button;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_button = 0;
                break;
            case SDL_MOUSEMOTION:
                if (mouse_button == 3)
                    camera_->rotate(e.motion.xrel, e.motion.yrel);
                else if (mouse_button == 2)
                    camera_->pan(e.motion.xrel, e.motion.yrel);
                break;
            case SDL_MOUSEWHEEL:
                camera_->zoom(e.wheel.preciseY);
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    sdl_texture_ = std::make_shared<SDLTexture>(
                        sdl_renderer_,
                        SDL_TEXTUREACCESS_STREAMING,
                        e.window.data1,
                        e.window.data2);
                    depth.resize(sdl_texture_->getWidth() * sdl_texture_->getHeight());
                }
                break;
            case SDL_CONTROLLERDEVICEADDED:
                game_controllers_.emplace(e.cdevice.which, std::make_shared<GameController>(e.cdevice.which));
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                game_controllers_.erase(e.cdevice.which);
                break;
            default:
                break;
            }
        }

        {
            glm::dvec2 left_stick(0.0);
            glm::dvec2 right_stick(0.0);
            double pan_z = 0.0;

            for (const auto& game_controller : game_controllers_)
            {
                const auto& a = game_controller.second;

                double z =
                    (a->getButton(SDL_CONTROLLER_BUTTON_A) ? 1.0 : 0.0) -
                    (a->getButton(SDL_CONTROLLER_BUTTON_B) ? 1.0 : 0.0);
                pan_z = glm::abs(z) > glm::abs(pan_z) ? z : pan_z;

                glm::dvec2 l(0.0);
                l.x = a->getAxis(SDL_CONTROLLER_AXIS_LEFTX);
                l.y = a->getAxis(SDL_CONTROLLER_AXIS_LEFTY);
                if (glm::length(l) > 0.1)
                    left_stick += l;

                glm::dvec2 r(0.0);
                r.x = a->getAxis(SDL_CONTROLLER_AXIS_RIGHTX);
                r.y = a->getAxis(SDL_CONTROLLER_AXIS_RIGHTY);
                if (glm::length(r) > 0.1)
                    right_stick += r;
            }

            if (glm::length(left_stick) > 1.0)
                left_stick = glm::normalize(left_stick);
            if (glm::length(right_stick) > 1.0)
                right_stick = glm::normalize(right_stick);

            camera_->rotate(right_stick.x * 8.0, right_stick.y * 8.0);
            camera_->pan(-left_stick.x * 8.0, pan_z * 8.0);
            camera_->zoom(-left_stick.y / 8.0);
        }

        sdl_texture_->clear();

        int width = sdl_texture_->getWidth();
        int height = sdl_texture_->getHeight();
        unsigned char* pixels = sdl_texture_->getPixels();
        auto ai = std::chrono::steady_clock::now();

        const glm::dmat4 viewport(
            width / 2.0, 0.0, 0.0, 0.0,
            0.0, height / 2.0, 0.0, 0.0,
            0.0, 0.0, 10.0, 0.0,
            (width - 1) / 2.0, (height - 1) / 2.0, 0.0, 1.0);
        const glm::dmat4 projection =
            glm::perspective(27.0 * RAD, (double)width / (double)height, 0.1, 400.0);

        Mesh current = mesh;
        current *= camera_->get();
        current.clip(projection, viewport);

        for (auto& d : depth)
            d = std::numeric_limits<double>::max();

        int triangle_count = current.getIndices().size() / 3;
        for (int i = 0; i < triangle_count; i++)
        {
            const int index = 3 * i;
            const glm::dvec4& a = current.getVertices()[current.getIndices()[index + 0]];
            const glm::dvec4& b = current.getVertices()[current.getIndices()[index + 1]];
            const glm::dvec4& c = current.getVertices()[current.getIndices()[index + 2]];
            const glm::dvec3& n = current.getNormals()[i];
            triangle2(
                a,
                b,
                c,
                [width, height, pixels, i, &depth, &n](int x, int y, double z)
                {
                    if (z <= depth[x + y * width])
                    {
                        depth[x + y * width] = z;
                        y = height - 1 - y;
                        int idx = 4 * (x + y * width);
                        double l = glm::mix(
                            0.2,
                            1.0,
                            glm::max(0.0, glm::dot(n, glm::dvec3(0.0, 0.0, 1.0))));
                        /*
                        pixels[idx + 1] = ((i + 0) % 3) == 0 ? 255 * l : 0;
                        pixels[idx + 2] = ((i + 1) % 3) == 0 ? 255 * l : 0;
                        pixels[idx + 3] = ((i + 2) % 3) == 0 ? 255 * l : 0;
                        */
                        pixels[idx + 1] = 255 * l;
                        pixels[idx + 2] = 255 * l;
                        pixels[idx + 3] = 255 * l;
                    }
                });
        }

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

        /*
        sdl_window_->setWindowTitle(
            std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(bi - ai).count() / 1000.0));
        */
    }
}

void App::init()
{
    int result;
    result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
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
    camera_ = std::make_shared<Camera>(20.0, 0.1, 400.0);
}

