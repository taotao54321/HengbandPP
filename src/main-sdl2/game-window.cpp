#include <string>

#include "main-sdl2/font.hpp"
#include "main-sdl2/game-window.hpp"
#include "main-sdl2/prelude.hpp"
#include "main-sdl2/system.hpp"

namespace {

constexpr int MAIN_WIN_NCOL_MIN = 80;
constexpr int MAIN_WIN_NROW_MIN = 24;

constexpr char FONT_NAME_DEFAULT[] = "monospace";

// src/wall.bmp と同一
constexpr u8 WALL_BMP[] = {
    0x42, 0x4d, 0x5e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00,
    0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
    0xff, 0x00, 0x44, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x96, 0x00,
    0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00
};

} // anonymous namespace

GameWindowDesc::GameWindowDesc()
    : title_("Hengband")
    , x_(0)
    , y_(0)
    , w_(400)
    , h_(400)
    , font_path_()
    , font_pt_(16) { }

GameWindowDesc& GameWindowDesc::title(std::string title) {
    title_ = std::move(title);
    return *this;
}

GameWindowDesc& GameWindowDesc::x(const int x) {
    x_ = x;
    return *this;
}

GameWindowDesc& GameWindowDesc::y(const int y) {
    y_ = y;
    return *this;
}

GameWindowDesc& GameWindowDesc::w(const int w) {
    w_ = w;
    return *this;
}

GameWindowDesc& GameWindowDesc::h(const int h) {
    h_ = h;
    return *this;
}

GameWindowDesc& GameWindowDesc::font_path(std::string path) {
    font_path_ = std::move(path);
    return *this;
}

GameWindowDesc& GameWindowDesc::font_pt(const int pt) {
    font_pt_ = pt;
    return *this;
}

GameWindow GameWindowDesc::build(const bool is_main) const {
    // フォント生成
    // フォント名が未指定ならデフォルトフォント名を使う
    auto font_path = font_path_;
    if (font_path.empty()) {
        const auto path_monospace = get_font_path(FONT_NAME_DEFAULT);
        if (!path_monospace) PANIC(R"(font "{}" not found)", FONT_NAME_DEFAULT);
        font_path = *path_monospace;
    }
    Font font(font_path, font_pt_);

    // ウィンドウ生成
    // メインウィンドウの場合、最小端末サイズを下回っていたら補正
    auto w = w_;
    auto h = h_;
    auto [ncol, nrow] = font.xy2cr(w, h);
    if (is_main) {
        if (chmax(ncol, MAIN_WIN_NCOL_MIN))
            w = font.c2x(ncol);
        if (chmax(nrow, MAIN_WIN_NROW_MIN))
            h = font.r2y(nrow);
    }
    auto win = Window::create(title_, x_, y_, w, h);

    return GameWindow(std::move(font), std::move(win));
}

Texture GameWindow::init_tex_wall() const {
    const auto surf_tile = Surface::from_bmp_bytes(WALL_BMP, std::size(WALL_BMP));
    const auto surf = Surface::create_tiled(surf_tile.get(), font_.w(), font_.h());
    return surf.to_texture(ren_.get());
}

GameWindow::GameWindow(Font font, Window win)
    : font_(std::move(font))
    , win_(std::move(win))
    , ren_(Renderer::with_window(win_.get()))
    , tex_wall_(init_tex_wall()) { }

u32 GameWindow::id() const {
    const auto res = SDL_GetWindowID(win_.get());
    if (res == 0) PANIC("SDL_GetWindowID() failed");
    return res;
}

void GameWindow::hide() const {
    SDL_HideWindow(win_.get());
}

void GameWindow::term_clear() const {
    if (SDL_SetRenderDrawColor(ren_.get(), 0, 0, 0, 0xFF) != 0)
        PANIC("SDL_SetRenderDrawColor() failed");
    if (SDL_RenderClear(ren_.get()) != 0)
        PANIC("SDL_RenderClear() failed");
}

void GameWindow::term_fill_rect(const int c, const int r, const int ncol, const int nrow, Color color) const {
    const auto rect = font_.calc_rect(c, r, ncol, nrow);
    if (SDL_SetRenderDrawColor(ren_.get(), color.r(), color.g(), color.b(), color.a()) != 0)
        PANIC("SDL_SetRenderDrawColor() failed");
    if (SDL_RenderFillRect(ren_.get(), &rect) != 0)
        PANIC("SDL_RenderFillRect() failed");
}

void GameWindow::term_draw_text(const int c, const int r, const std::string& text, Color fg, Color bg) const {
    // TODO:
    //   ナイーブな実装では毎回 Surface と Texture を作るため、複雑な地形(アン
    //   グウィルなど)ではそれなりに重くなる。
    //   ASCII 文字のみについてテクスチャキャッシュを持つなど工夫すべきか。

    const auto surf = font_.render(text, fg, bg);

    const auto [x, y] = font_.cr2xy(c, r);
    const SDL_Rect rect { x, y, surf.get()->w, surf.get()->h };

    const auto tex = surf.to_texture(ren_.get());
    if (SDL_RenderCopy(ren_.get(), tex.get(), nullptr, &rect) != 0)
        PANIC("SDL_RenderCopy() failed");
}

void GameWindow::term_draw_wall(const int c, const int r, Color color) const {
    const auto rect = font_.calc_rect(c, r, 1, 1);
    if (SDL_SetTextureColorMod(tex_wall_.get(), color.r(), color.g(), color.b()) != 0)
        PANIC("SDL_SetTextureColorMod() failed");
    if (SDL_RenderCopy(ren_.get(), tex_wall_.get(), nullptr, &rect) != 0)
        PANIC("SDL_RenderCopy() failed");
}
