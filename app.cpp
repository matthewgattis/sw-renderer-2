#include "app.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_render.h>
#include <limits>
#include <memory>

#include "camera.hpp"
#include "mesh.hpp"
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

    const std::vector<double> arwing_vertices
    {
        -1.000000, 1.000000, -0.150000,
        3.250000, -0.000000, 0.000000,
        -1.000000, 0.000000, 1.000000,
        -1.000000, 0.000000, -1.000000,
        -1.500000, 0.500000, 0.000000,
        0.000000, 0.500000, -0.100000,
        1.179421, 0.000000, -0.500000,
        1.179421, 0.000000, 0.500000,
        0.089711, 0.000000, 0.750000,
        0.089711, 0.000000, -0.750000,
        -1.000000, 1.000000, 0.150000,
        0.000000, 0.500000, 0.100000,
        0.500000, -0.450000, 0.000000,

        -1.000000, 0.000000, -1.000000,
        -3.926414, 2.057791, -3.265626,
        0.180935, -1.000000, -1.553861,
        -1.122586, 0.558973, -1.796001,
        -1.000000, 0.000000, 1.000000,
        -3.926414, 2.057791, 3.265626,
        0.180935, -1.000000, 1.553861,
        -1.122586, 0.558973, 1.796001,

        -1.000000, 0.000000, -1.000000,
        -3.926414, -0.100000, -2.500000,
        -6.006952, -0.250000, -4.962239,
        -1.293253, 0.031070, -1.796001,
        -1.000000, 0.000000, 1.000000,
        -3.926414, -0.100000, 2.500000,
        -6.006952, -0.250000, 4.962239,
        -1.293253, 0.031070, 1.796001,

        -1.000000, 1.000000, -0.150000,
        -1.000000, 0.000000, 1.000000,
        -1.000000, 0.000000, -1.000000,
        -1.000000, 1.000000, 0.150000,
    };

    const std::vector<int> arwing_indices
    {
        6, 12, 2,
		5, 1, 4,
		5, 11, 1,
		11, 5, 3,
		11, 3, 12,
		12, 1, 11,
		6, 4, 1,
		3, 9, 8,
		3, 8, 2,
		3, 2, 12,
		12, 6, 1,
		6, 2, 7,
		6, 7, 10,
		6, 10, 4,
		4, 10, 7,
		4, 7, 2,
		4, 2, 13,
		3, 13, 2,
		3, 2, 8,
		3, 8, 9,
		4, 13, 3,

		15, 17, 16,
		15, 16, 14,
		14, 16, 17,
		14, 17, 15,
		19, 20, 21,
		19, 18, 20,
		18, 21, 20,
		18, 19, 21,

		23, 25, 24,
		23, 24, 22,
		22, 24, 25,
		22, 25, 23,
		27, 28, 29,
		27, 26, 28,
		26, 29, 28,
		26, 27, 29,

		30, 32, 31,
		30, 31, 33,
    };

    Mesh mesh;
    {
        int triangle_count = arwing_indices.size() / 3;
        for (int i = 0; i < triangle_count; i++)
        {
            int index = 3 * i;
            mesh.addTriangle(
                mesh.addVertex(glm::dvec3(
                    arwing_vertices[3 * (arwing_indices[index + 0] - 1) + 0],
                    arwing_vertices[3 * (arwing_indices[index + 0] - 1) + 1],
                    arwing_vertices[3 * (arwing_indices[index + 0] - 1) + 2])),
                mesh.addVertex(glm::dvec3(
                    arwing_vertices[3 * (arwing_indices[index + 1] - 1) + 0],
                    arwing_vertices[3 * (arwing_indices[index + 1] - 1) + 1],
                    arwing_vertices[3 * (arwing_indices[index + 1] - 1) + 2])),
                mesh.addVertex(glm::dvec3(
                    arwing_vertices[3 * (arwing_indices[index + 2] - 1) + 0],
                    arwing_vertices[3 * (arwing_indices[index + 2] - 1) + 1],
                    arwing_vertices[3 * (arwing_indices[index + 2] - 1) + 2])),
                glm::dvec3(1.0, 1.0, 1.0));
        }
    }

	constexpr double RAD = glm::pi<double>() / 180.0;
	constexpr glm::dmat4 identity = glm::identity<glm::dmat4>();

    std::vector<double> depth;
    depth.resize(sdl_texture_->getWidth()* sdl_texture_->getHeight());

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
                {
                    sdl_texture_ = std::make_shared<SDLTexture>(
                        sdl_renderer_,
                        SDL_TEXTUREACCESS_STREAMING,
                        e.window.data1,
                        e.window.data2);
                    depth.resize(sdl_texture_->getWidth() * sdl_texture_->getHeight());
                }
                break;
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
                    if (
                        x >= 0 && x < width &&
                        y >= 0 && y < height)
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
    camera_ = std::make_shared<Camera>(50.0, 0.1, 400.0);
}

