#pragma once

#include <boost/core/noncopyable.hpp>

#include <SDL.h>

#include "prelude.hpp"

class System : private boost::noncopyable {
public:
    System();

    ~System();
};

class Color {
private:
    SDL_Color color_;

public:
    Color(u8 r, u8 g, u8 b, u8 a);

    static Color from_sdl_color(SDL_Color color);

    [[nodiscard]] u8 r() const;
    [[nodiscard]] u8 g() const;
    [[nodiscard]] u8 b() const;
    [[nodiscard]] u8 a() const;

    [[nodiscard]] SDL_Color to_sdl_color() const;
};

class Surface;

class Texture : private boost::noncopyable {
private:
    SDL_Texture* tex_;

public:
    explicit Texture(SDL_Texture* tex);

    static Texture from_surface(SDL_Renderer* ren, const Surface& surf);

    ~Texture();

    [[nodiscard]] SDL_Texture* get() const;
};

class Surface : private boost::noncopyable {
private:
    SDL_Surface* surf_;

public:
    explicit Surface(SDL_Surface* surf);

    ~Surface();

    [[nodiscard]] SDL_Surface* get() const;

    [[nodiscard]] Texture to_texture(SDL_Renderer* ren) const;
};
