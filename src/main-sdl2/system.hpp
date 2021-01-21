#pragma once

#include <cstddef>

#include <boost/core/noncopyable.hpp>

#include <SDL.h>

#include "prelude.hpp"

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

class System : private boost::noncopyable {
public:
    System();

    ~System();
};

class Window : private boost::noncopyable {
private:
    SDL_Window* win_;

public:
    explicit Window(SDL_Window* win);

    static Window create(const std::string& title, int x, int y, int w, int h);

    ~Window();

    [[nodiscard]] SDL_Window* get() const;
};

class Renderer : private boost::noncopyable {
private:
    SDL_Renderer* ren_;

public:
    explicit Renderer(SDL_Renderer* ren);

    static Renderer with_window(const Window& win);

    ~Renderer();

    [[nodiscard]] SDL_Renderer* get() const;
};

class Surface;

class Texture : private boost::noncopyable {
private:
    SDL_Texture* tex_;

    void drop();

public:
    explicit Texture(SDL_Texture* tex);

    static Texture from_surface(SDL_Renderer* ren, const Surface& surf);

    // Texture は動的な変更がありうるのでムーブ代入を書いておく。
    // 解放処理がバグりやすいのであまり書きたくないが仕方ない。
    Texture& operator=(Texture&& rhs) noexcept;

    ~Texture();

    [[nodiscard]] SDL_Texture* get() const;
};

class Surface : private boost::noncopyable {
private:
    SDL_Surface* surf_;

public:
    explicit Surface(SDL_Surface* surf);

    static Surface with_size_rgba(int w, int h);

    static Surface from_bmp_bytes(const u8* buf, std::size_t len);

    ~Surface();

    [[nodiscard]] SDL_Surface* get() const;

    [[nodiscard]] Texture to_texture(SDL_Renderer* ren) const;
};
