#include <cstddef>

#include <SDL.h>
#include <SDL_ttf.h>

#include "main-sdl2/prelude.hpp"
#include "main-sdl2/system.hpp"

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

Window::Window(SDL_Window* win)
    : win_(win) { }

Window Window::create(const std::string& title, const int x, const int y, const int w, const int h) {
    auto* win = SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_RESIZABLE);
    if (!win) PANIC("SDL_CreateWindow() failed");
    return Window(win);
}

Window::~Window() {
    SDL_DestroyWindow(win_);
}

SDL_Window* Window::get() const { return win_; }

Renderer::Renderer(SDL_Renderer* ren)
    : ren_(ren) { }

Renderer Renderer::with_window(const Window& win) {
    auto* ren = SDL_CreateRenderer(win.get(), -1, 0);
    if (!ren) PANIC("SDL_CreateRenderer() failed");
    return Renderer(ren);
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(ren_);
}

SDL_Renderer* Renderer::get() const { return ren_; }

void Texture::drop() {
    if (tex_) {
        SDL_DestroyTexture(tex_);
        tex_ = nullptr;
    }
}

Texture::Texture(SDL_Texture* tex)
    : tex_(tex) { }

Texture Texture::from_surface(SDL_Renderer* ren, const Surface& surf) {
    auto* tex = SDL_CreateTextureFromSurface(ren, surf.get());
    if (!tex) PANIC("SDL_CreateTextureFromSurface() failed");
    return Texture(tex);
}

Texture& Texture::operator=(Texture&& rhs) noexcept {
    drop();
    tex_ = rhs.tex_;
    rhs.tex_ = nullptr;
    return *this;
}

Texture::~Texture() { drop(); }

SDL_Texture* Texture::get() const { return tex_; }

Surface::Surface(SDL_Surface* surf)
    : surf_(surf) { }

Surface Surface::with_size_rgba(const int w, const int h) {
    auto* surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surf) PANIC("SDL_CreateRGBSurfaceWithFormat() failed");
    return Surface(surf);
}

Surface Surface::from_bmp_bytes(const u8* buf, const std::size_t len) {
    auto* rdr = SDL_RWFromConstMem(buf, len);
    if (!rdr) PANIC("SDL_RWFromConstMem() failed");

    auto* surf = SDL_LoadBMP_RW(rdr, 1);
    if (!surf) PANIC("SDL_LoadBMP_RW() failed");

    return Surface(surf);
}

Surface::~Surface() {
    SDL_FreeSurface(surf_);
}

SDL_Surface* Surface::get() const { return surf_; }

Texture Surface::to_texture(SDL_Renderer* ren) const {
    return Texture::from_surface(ren, *this);
}
