#include "app.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_render.h>
#include <limits>
#include <memory>
#include <functional>
#include <ranges>

#include "camera.hpp"
#include "mesh.hpp"
#include "sdlwindow.hpp"
#include "sdlrenderer.hpp"
#include "sdltexture.hpp"
#include "triangle.hpp"
#include "gamecontroller.hpp"
#include "teapot.hpp"
#include "depthbuffer.hpp"

#define LOG_MODULE_NAME ("App")
#include "log.hpp"

App::App() :
    mouse_button_(0),
    run_(true)
{
    LOG_INFO << "Instance created." << std::endl;

    int result;
    result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    if (result)
    {
        LOG_ERROR << "Failure in SDL_Init. (" << SDL_GetError() << ")" << std::endl;
        throw std::exception();
    }

    sdl_window_ = std::make_shared<SDLWindow>("sw-renderer");

    sdl_renderer_ = std::make_shared<SDLRenderer>(sdl_window_);
    auto [width, height] = sdl_renderer_->getResolution();

    sdl_texture_ = std::make_shared<SDLTexture>(
        sdl_renderer_,
        width,
        height);

    depth_buffer_ = std::make_shared<DepthBuffer>(width, height);

    camera_ = std::make_shared<Camera>(20.0, 0.1, 400.0);

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

}

App::~App()
{
    SDL_Quit();
}

void App::run(const std::vector<std::string> &args)
{
    int ellapsed = 0;

    int frame_delay = 1;
    int frame_count = 0;

    constexpr double RAD = glm::pi<double>() / 180.0;
    constexpr glm::dmat4 identity = glm::identity<glm::dmat4>();

    const Mesh mesh(teapot_vertices, teapot_indices);

    while (run_)
    {
        int frame_start = SDL_GetTicks();

        handleEvents();

        sdl_texture_->clear();
        depth_buffer_->clear();

        auto [width, height] = sdl_renderer_->getResolution();
        std::span<uint32_t> pixels = sdl_texture_->getPixels();
        std::span<double> depth = depth_buffer_->get();
        auto ai = std::chrono::steady_clock::now();

        const glm::dmat4 viewport(
            width / 2.0, 0.0, 0.0, 0.0,
            0.0, height / 2.0, 0.0, 0.0,
            0.0, 0.0, 10.0, 0.0,
            (width - 1) / 2.0, (height - 1) / 2.0, 0.0, 1.5);

        const glm::dmat4 projection =
            glm::perspective(27.0 * RAD, (double)width / (double)height, 0.1, 400.0);
        
        // Vertex shader
        // Transform to clip space
        // Clip triangles
        // Perspective divide
        // Backface culling
        // Viewport transform
        // Fragment shader

        Mesh current = mesh; // Copy mesh
        const auto to_clip_space = projection * camera_->get();
        current *= to_clip_space;

        const auto &vertices = current.getVertices();
        const auto &indices = current.getIndices();
        const auto &normals = current.getNormals();

        std::array<std::vector<std::array<glm::dvec4, 3>>, 2> triangles{
            std::vector<std::array<glm::dvec4, 3>>{},
            std::vector<std::array<glm::dvec4, 3>>{}
        };
        triangles[0].reserve(64);
        triangles[1].reserve(64);

        for (int i = 0; i < indices.size(); i++)
        {
            const auto &index = indices[i];
            const auto &normal = normals[i];

            triangles[0].clear();
            triangles[0].push_back({
                vertices[index[0]],
                vertices[index[1]],
                vertices[index[2]]
            });

            {
                std::array<glm::dvec3, 3> triangle;

                for (int i = 0; i < 3; i++)
                {
                    const auto &v = triangles[0][0][i];
                    triangle[i] = glm::dvec3{ v } / v.w;
                }
                
                const auto &a = triangle[0];
                const auto &b = triangle[1];
                const auto &c = triangle[2];

                // Backface culling
                if (glm::cross(b - a, c - a).z < 0.0)
                    continue;
            }

            for (int j = 0; j < 6; j++)
            {
                int sign = j % 2 == 0 ? -1 : 1;
                int component = j / 2;

                auto &input_triangles = triangles[j % 2];
                auto &output_triangles = triangles[(j + 1) % 2];
                output_triangles.clear();

                for (const auto &triangle : input_triangles)
                {
                    std::vector<glm::dvec4> inside_vertices;
                    std::vector<glm::dvec4> outside_vertices;

                    for (int k = 0; k < 3; k++)
                    {
                        const glm::dvec4 &v = triangle[k];

                        if (sign * v[component] > v.w)
                            outside_vertices.push_back(v);
                        else
                            inside_vertices.push_back(v);
                    }

                    if (inside_vertices.size() + outside_vertices.size() != 3)
                    {
                        LOG_ERROR << "Invalid triangle: " << inside_vertices.size() << " inside, "
                                  << outside_vertices.size() << " outside." << std::endl;
                        continue;
                    }

                    if (inside_vertices.empty())
                        continue;
                    else if (inside_vertices.size() == 1)
                    {
                        const glm::dvec4 &a = inside_vertices[0];
                        const glm::dvec4 &b = outside_vertices[0];
                        const glm::dvec4 &c = outside_vertices[1];

                        glm::dvec4 d, e;
                        if (sign > 0)
                        {
                            d = a + ((a.w - a[component]) / (a.w - b.w - a[component] + b[component])) * (b - a);
                            e = a + ((a.w - a[component]) / (a.w - c.w - a[component] + c[component])) * (c - a);
                        }
                        else
                        {
                            d = a + ((a.w + a[component]) / (a.w - b.w + a[component] - b[component])) * (b - a);
                            e = a + ((a.w + a[component]) / (a.w - c.w + a[component] - c[component])) * (c - a);
                        }

                        output_triangles.push_back({ a, d, e });
                    }
                    else if (inside_vertices.size() == 2)
                    {
                        const glm::dvec4 &a = inside_vertices[0];
                        const glm::dvec4 &b = inside_vertices[1];
                        const glm::dvec4 &c = outside_vertices[0];

                        glm::dvec4 d, e;
                        if (sign > 0)
                        {
                            d = a + ((a.w - a[component]) / (a.w - c.w - a[component] + c[component])) * (c - a);
                            e = b + ((b.w - b[component]) / (b.w - c.w - b[component] + c[component])) * (c - b);
                        }
                        else
                        {
                            d = a + ((a.w + a[component]) / (a.w - c.w + a[component] - c[component])) * (c - a);
                            e = b + ((b.w + b[component]) / (b.w - c.w + b[component] - c[component])) * (c - b);
                        }

                        output_triangles.push_back({ a, b, d });
                        output_triangles.push_back({ b, e, d });
                    }
                    else
                        output_triangles.push_back(triangle);
                }
            }

            auto &output_triangles = triangles[0];
            bool first = true;
            uint32_t color;

            for (auto &t : output_triangles)
            {
                glm::dvec4 &a = t[0];
                glm::dvec4 &b = t[1];
                glm::dvec4 &c = t[2];

                // Perspective divide
                for (auto &v : t)
                    v = v / v.w;

                if (first)
                {
                    constexpr glm::dvec3 back = { 0.0, 0.0, -1.0 };
                    unsigned char v =
                        255 * glm::pow(
                            glm::max(0.0, glm::dot(normal, back)),
                            1.0 / 2.2);
                    color = SDLTexture::Color{ v, v, v, 255 };

                    first = false;
                }

                // Viewport transform
                for (auto &v : t)
                    v = viewport * v;
                
                triangle(
                    a.x, a.y, a.z,
                    b.x, b.y, b.z,
                    c.x, c.y, c.z,
                    [&](int x, int y, double z)
                    {
                        int idx = x + width * y;
                        if (z <= depth[idx])
                        {
                            depth[idx] = z;
                            pixels[idx] = color;
                        }
                    });
            }
        }

        auto bi = std::chrono::steady_clock::now();

        sdl_texture_->update();
        sdl_renderer_->copy(sdl_texture_);
        sdl_renderer_->present();

        const int MIN_FRAME = 1;
        int frame_end = SDL_GetTicks();
        SDL_Delay(std::max(0, MIN_FRAME - (frame_end - frame_start)));
        frame_delay = SDL_GetTicks() - frame_start;
        ellapsed += frame_delay;

        frame_count++;
    }
}

void App::handleEvents()
{
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        for (auto& game_controller : game_controllers_)
            game_controller.second->handleEvent(e);

        switch (e.type)
        {
        case SDL_QUIT:
            run_ = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouse_button_ = e.button.button;
            break;
        case SDL_MOUSEBUTTONUP:
            mouse_button_ = 0;
            break;
        case SDL_MOUSEMOTION:
            if (mouse_button_ == 3)
                camera_->rotate(e.motion.xrel, -e.motion.yrel);
            else if (mouse_button_ == 2)
                camera_->pan(e.motion.xrel, e.motion.yrel);
            break;
        case SDL_MOUSEWHEEL:
            camera_->zoom(e.wheel.preciseY);
            break;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                auto [width, height] = sdl_renderer_->getResolution();

                sdl_texture_ = std::make_shared<SDLTexture>(
                    sdl_renderer_,
                    width,
                    height);

                depth_buffer_ = std::make_shared<DepthBuffer>(width, height);
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
}

