#include "app.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_render.h>
#include <limits>
#include <memory>

#include "camera.hpp"
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

    int mouse_button = 0;

	constexpr double RAD = glm::pi<double>() / 180.0;
	constexpr glm::dmat4 identity = glm::identity<glm::dmat4>();

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
        
        const glm::dmat4 viewport(
            width / 2.0, 0.0, 0.0, 0.0,
            0.0, height / 2.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            (width - 1) / 2.0, (height - 1) / 2.0, 0.0, 1.0);

        const glm::dmat4 projection =
            glm::perspective(27.0 * RAD, (double)width / (double)height, 0.1, 100.0);

        std::vector<glm::dvec4> vertices;
        vertices.reserve(3);
        vertices.push_back(glm::dvec4(1.0, 0.0, 0.0, 1.0));
        vertices.push_back(glm::dvec4(cos(120.0 * RAD), -sin(120.0 * RAD), 0.0, 1.0));
        vertices.push_back(glm::dvec4(cos(120.0 * RAD), sin(120.0 * RAD), 0.0, 1.0));
        std::vector<int> indices;
        indices.reserve(3);
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        auto view_projection = projection * camera_->get();
        std::vector<glm::dvec4> clip_vertices;
        clip_vertices.reserve(vertices.size());
        for (auto& v : vertices)
            clip_vertices.push_back(view_projection * v);

        std::vector<glm::dvec4> flat_vertices;
        flat_vertices.reserve(indices.size());
        for (const auto index : indices)
            flat_vertices.push_back(clip_vertices[index]);

        for (int i = 0; i < 6; i++)
        {
            std::vector<glm::dvec4> next_vertices;
            next_vertices.reserve(next_vertices.size());

            double sign = i < 3 ? -1.0 : 1.0;
            int component = i % 3;
            int triangle_count = clip_vertices.size() / 3;

            for (int j = 0; j < triangle_count; j++)
            {
                int triangle = 3 * j;
                std::vector<int> out;
                std::vector<int> in;
                for (int k = 0; k < 3; k++)
                {
                    int index = triangle + k;
                    const auto& v = clip_vertices[index];
                    if (sign * v[component] > v.w)
                        out.push_back(index);
                    else
                        in.push_back(index);
                }

                if (in.size() > 0)
                {
                    const int in_count = in.size();
                    if (in_count == 1)
                    {
                        const auto& a = clip_vertices[in[0]];
                        const auto& b = clip_vertices[out[0]];
                        const auto& c = clip_vertices[out[1]];
                        glm::dvec4 d;
                        glm::dvec4 e;
                        if (sign > 0.0)
                        {
                            d = a + ((a.w - a[component]) / (a.w - b.w - a[component] + b[component])) * (b - a);
                            e = a + ((a.w - a[component]) / (a.w - c.w - a[component] + c[component])) * (c - a);
                        }
                        else
                        {
                            d = a + ((a.w + a[component]) / (a.w - b.w + a[component] - b[component])) * (b - a);
                            e = a + ((a.w + a[component]) / (a.w - c.w + a[component] - c[component])) * (c - a);
                        }
                        next_vertices.push_back(a);
                        next_vertices.push_back(d);
                        next_vertices.push_back(e);
                    }
                    else if (in_count == 2)
                    {
                        const auto& a = clip_vertices[in[0]];
                        const auto& b = clip_vertices[in[1]];
                        const auto& c = clip_vertices[out[0]];
                        glm::dvec4 d;
                        glm::dvec4 e;
                        if (sign > 0.0)
                        {
                            d = a + ((a.w - a[component]) / (a.w - c.w - a[component] + c[component])) * (c - a);
                            e = b + ((b.w - b[component]) / (b.w - c.w - b[component] + c[component])) * (c - b);
                        }
                        else
                        {
                            d = a + ((a.w + a[component]) / (a.w - c.w + a[component] - c[component])) * (c - a);
                            e = b + ((b.w + b[component]) / (b.w - c.w + b[component] - c[component])) * (c - b);
                        }
                        next_vertices.push_back(a);
                        next_vertices.push_back(b);
                        next_vertices.push_back(d);
                        next_vertices.push_back(b);
                        next_vertices.push_back(e);
                        next_vertices.push_back(d);
                    }
                    else
                    {
                        const auto& a = clip_vertices[in[0]];
                        const auto& b = clip_vertices[in[1]];
                        const auto& c = clip_vertices[in[2]];
                        next_vertices.push_back(a);
                        next_vertices.push_back(b);
                        next_vertices.push_back(c);
                    }
                }
            }
            clip_vertices = next_vertices;
        }

        for (auto& clip_vertex : clip_vertices)
            clip_vertex = viewport * (clip_vertex / clip_vertex.w);

        int triangle_count = clip_vertices.size() / 3;
        for (int i = 0; i < triangle_count; i++)
        {
            int index = 3 * i;
            const auto& a = clip_vertices[index + 0];
            const auto& b = clip_vertices[index + 1];
            const auto& c = clip_vertices[index + 2];
            triangle2(
                a,
                b,
                c,
                [width, height, pixels, i](int x, int y, double z)
                {
                    if (
                        x >= 0 && x < width &&
                        y >= 0 && y < height)
                    {
                        y = height - 1 - y;
                        int idx = 4 * (x + y * width);
                        /*
                        pixels[idx + 1] = ((i + 0) % 3) == 0 ? 255 : 0;
                        pixels[idx + 2] = ((i + 1) % 3) == 0 ? 255 : 0;
                        pixels[idx + 3] = ((i + 2) % 3) == 0 ? 255 : 0;
                        */
                        pixels[idx + 1] = 255;
                        pixels[idx + 2] = 255;
                        pixels[idx + 3] = 255;
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

        sdl_window_->setWindowTitle(
            std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(bi - ai).count() / 1000.0));
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
    camera_ = std::make_shared<Camera>(10.0, 0.1, 100.0);
}

