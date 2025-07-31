#pragma once

#include <memory>
#include <vector>
#include <span>
#include <cstdint>

#include <SDL.h>

class SDLRenderer;

class SDLTexture
{
public:
    struct Color
    {
        unsigned char r, g, b, a;

        Color() : r(0), g(0), b(0), a(255) {}
        Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) :
            r(r), g(g), b(b), a(a) {}

        operator uint32_t() const
        {
            return
                (static_cast<uint32_t>(r) << 24) |
                (static_cast<uint32_t>(g) << 16) |
                (static_cast<uint32_t>(b) << 8) |
                static_cast<uint32_t>(a);
        }
    };

public:
    SDLTexture(
        const std::shared_ptr<SDLRenderer> &sdl_renderer,
        int width,
        int height,
        uint32_t clear_color = 0xff); // Default clear color is opaque black (0xff000000)

    ~SDLTexture();

    SDL_Texture *get() const { return sdl_texture_; }

    std::span<uint32_t> getPixels() { return std::span{ pixels_ }; }

    void update();

    void clear();

private:
    SDL_Texture *sdl_texture_;
    int width_;
    uint32_t clear_color_;

    std::vector<uint32_t> pixels_;
};
