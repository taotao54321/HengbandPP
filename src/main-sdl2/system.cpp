#include <SDL.h>
#include <SDL_ttf.h>

#include "main-sdl2/prelude.hpp"
#include "main-sdl2/system.hpp"

System::System() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        PANIC("SDL_Init() failed");

    if (TTF_Init() != 0)
        PANIC("TTF_Init() failed");
}

System::~System() {
    TTF_Quit();

    SDL_Quit();
}

Color::Color(const u8 r, const u8 g, const u8 b, const u8 a)
    : color_ { r, g, b, a } { }

Color Color::from_sdl_color(SDL_Color color) {
    return Color(color.r, color.g, color.b, color.a);
}

u8 Color::r() const { return color_.r; }
u8 Color::g() const { return color_.g; }
u8 Color::b() const { return color_.b; }
u8 Color::a() const { return color_.a; }

SDL_Color Color::to_sdl_color() const { return color_; }

Texture::Texture(SDL_Texture* tex)
    : tex_(tex) { }

Texture::~Texture() {
    SDL_DestroyTexture(tex_);
}

SDL_Texture* Texture::get() const { return tex_; }

Surface::Surface(SDL_Surface* surf)
    : surf_(surf) { }

Surface::~Surface() {
    SDL_FreeSurface(surf_);
}

SDL_Surface* Surface::get() const { return surf_; }

Texture Surface::to_texture(SDL_Renderer* ren) const {
    auto* tex = SDL_CreateTextureFromSurface(ren, surf_);
    if (!tex) PANIC("SDL_CreateTextureFromSurface() failed");
    return Texture(tex);
}
