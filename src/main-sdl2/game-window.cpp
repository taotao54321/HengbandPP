#include <string>
#include <tuple>
#include <utility>

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
    , font_pt_(16)
    , visible_(true) { }

const std::string& GameWindowDesc::title() const { return title_; }

GameWindowDesc& GameWindowDesc::title(std::string title) {
    title_ = std::move(title);
    return *this;
}

int GameWindowDesc::x() const { return x_; }

GameWindowDesc& GameWindowDesc::x(const int x) {
    x_ = x;
    return *this;
}

int GameWindowDesc::y() const { return y_; }

GameWindowDesc& GameWindowDesc::y(const int y) {
    y_ = y;
    return *this;
}

int GameWindowDesc::w() const { return w_; }

GameWindowDesc& GameWindowDesc::w(const int w) {
    w_ = w;
    return *this;
}

int GameWindowDesc::h() const { return h_; }

GameWindowDesc& GameWindowDesc::h(const int h) {
    h_ = h;
    return *this;
}

const std::string& GameWindowDesc::font_path() const { return font_path_; }

GameWindowDesc& GameWindowDesc::font_path(std::string path) {
    font_path_ = std::move(path);
    return *this;
}

int GameWindowDesc::font_pt() const { return font_pt_; }

GameWindowDesc& GameWindowDesc::font_pt(const int pt) {
    font_pt_ = pt;
    return *this;
}

bool GameWindowDesc::visible() const { return visible_; }

GameWindowDesc& GameWindowDesc::visible(const bool visible) {
    visible_ = visible;
    return *this;
}

GameWindow GameWindowDesc::build(const bool is_main) const {
    // フォント生成
    // フォント名が未指定ならデフォルトフォント名を使う。
    auto font_path = font_path_;
    if (font_path.empty()) {
        const auto path_monospace = get_font_path(FONT_NAME_DEFAULT);
        if (!path_monospace) PANIC(R"(font "{}" not found)", FONT_NAME_DEFAULT);
        font_path = *path_monospace;
    }
    Font font(font_path, font_pt_);

    // ウィンドウ生成
    // メインウィンドウの場合、最小端末サイズを下回っていたら補正。
    // 起動時に非表示ウィンドウが見切れるのを防ぐため、最初は hidden 状態で生成する。
    auto w = w_;
    auto h = h_;
    auto [ncol, nrow] = font.xy2cr(w, h);
    if (is_main) {
        if (chmax(ncol, MAIN_WIN_NCOL_MIN))
            w = font.c2x(ncol);
        if (chmax(nrow, MAIN_WIN_NROW_MIN))
            h = font.r2y(nrow);
    }
    auto win = Window::create(title_, x_, y_, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);

    auto game_win = GameWindow(std::move(font), std::move(win));

    // メインウィンドウは非表示にできない
    game_win.set_visible(visible_ || is_main);

    return game_win;
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
    , tex_wall_(init_tex_wall()) {
    // TODO: 再描画関数を設けるべきでは
    term_clear();
    present();
}

std::tuple<int, int, int, int> GameWindow::borders_size() const {
    int top;
    int left;
    int bottom;
    int right;
    // X11 以外ではサポートされていないため失敗する
    if (SDL_GetWindowBordersSize(win_.get(), &top, &left, &bottom, &right) != 0)
        return { 0, 0, 0, 0 };
    return { top, left, bottom, right };
}

std::pair<int, int> GameWindow::pos() const {
    int x, y;
    SDL_GetWindowPosition(win_.get(), &x, &y);
    return { x, y };
}

std::pair<int, int> GameWindow::client_area_size() const {
    // TODO:
    //   全ての環境でうまくいくかは未確認。
    //   X11 環境では
    //
    //     * SDL_CreateWindow() に指定したサイズ
    //     * SDL_GetWindowSize() で得られるサイズ
    //     * SDL_RendererOutputSize() で得られるサイズ
    //
    //   は全て一致し、クライアント領域サイズとなるようだ。

    int w, h;
    SDL_GetWindowSize(win_.get(), &w, &h);
    return { w, h };
}

std::pair<int, int> GameWindow::client_area_size_for(const int ncol, const int nrow) const {
    return font_.cr2xy(ncol, nrow);
}

u32 GameWindow::id() const {
    const auto res = SDL_GetWindowID(win_.get());
    if (res == 0) PANIC("SDL_GetWindowID() failed");
    return res;
}

bool GameWindow::is_visible() const {
    // SDL_WINDOW_SHOWN と SDL_WINDOW_HIDDEN を両方指定したウィンドウは hidden
    // になるようなので、後者で判定する
    return !bool(SDL_GetWindowFlags(win_.get()) & SDL_WINDOW_HIDDEN);
}

void GameWindow::set_visible(const bool visible) {
    if (visible)
        SDL_ShowWindow(win_.get());
    else
        SDL_HideWindow(win_.get());
}

std::pair<int, int> GameWindow::term_size() const {
    // TODO: メインウィンドウの場合は最小端末サイズを下回らない措置を設けたい

    const auto [w, h] = client_area_size();
    return term_size_for(w, h);
}

std::pair<int, int> GameWindow::term_size_for(const int w, const int h) const {
    return { w / font_.w(), h / font_.h() };
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

void GameWindow::present() const {
    SDL_RenderPresent(ren_.get());
}

GameWindowDesc GameWindow::desc() const {
    // TODO:
    //   X11 環境以外での動作は未検証。
    //   X11 環境では SDL_GetWindowPosition() と SDL_GetWindowBordersSize() を
    //   組み合わせることで外枠も含めたウィンドウ位置が得られる。

    int border_top, border_left;
    std::tie(border_top, border_left, std::ignore, std::ignore) = borders_size();

    const auto [x, y] = pos();
    const auto [w, h] = client_area_size();

    return GameWindowDesc()
        .title(SDL_GetWindowTitle(win_.get()))
        .x(x - border_left)
        .y(y - border_top)
        .w(w)
        .h(h)
        .font_path(font_.path())
        .font_pt(font_.pt())
        .visible(is_visible());
}
