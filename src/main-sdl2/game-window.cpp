#include <algorithm>
#include <string>
#include <tuple>
#include <utility>

#include "main-sdl2/asset.hpp"
#include "main-sdl2/font.hpp"
#include "main-sdl2/game-window.hpp"
#include "main-sdl2/prelude.hpp"
#include "main-sdl2/system.hpp"

namespace {

constexpr int MAIN_WIN_MENU_H = 32;

constexpr int MAIN_WIN_NCOL_MIN = 80;
constexpr int MAIN_WIN_NROW_MIN = 24;

constexpr char FONT_NAME_DEFAULT[] = "monospace";

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

    auto game_win = GameWindow(is_main, std::move(font), std::move(win));

    game_win.set_visible(visible_);

    return game_win;
}

Texture GameWindow::init_tex_term() const {
    const auto [w, h] = font_.cr2xy(ncnr_);
    return Texture::create_target(ren_.get(), w, h);
}

Texture GameWindow::init_tex_wall() const {
    const auto surf_tile = Surface::from_bytes(WALL_BMP, std::size(WALL_BMP));
    const auto surf = Surface::create_tiled(surf_tile.get(), font_.w(), font_.h());
    return surf.to_texture(ren_.get());
}

GameWindow::GameWindow(const bool is_main, Font font, Window win)
    : is_main_(is_main)
    , font_(std::move(font))
    , win_(std::move(win))
    , ren_(Renderer::with_window(win_.get()))
    , ncnr_(term_size_for(client_area_size()))
    , tex_term_(init_tex_term())
    , tex_wall_(init_tex_wall()) {
    if (is_main_) {
        const auto [w_min, h_min] = client_area_size_for(MAIN_WIN_NCOL_MIN, MAIN_WIN_NROW_MIN);
        SDL_SetWindowMinimumSize(win_.get(), w_min, h_min);

        texs_.emplace("win_1", Surface::from_bytes(WINDOW_1_PNG, std::size(WINDOW_1_PNG)).to_texture(ren_.get()));
        texs_.emplace("win_2", Surface::from_bytes(WINDOW_2_PNG, std::size(WINDOW_2_PNG)).to_texture(ren_.get()));
        texs_.emplace("win_3", Surface::from_bytes(WINDOW_3_PNG, std::size(WINDOW_3_PNG)).to_texture(ren_.get()));
        texs_.emplace("win_4", Surface::from_bytes(WINDOW_4_PNG, std::size(WINDOW_4_PNG)).to_texture(ren_.get()));
        texs_.emplace("win_5", Surface::from_bytes(WINDOW_5_PNG, std::size(WINDOW_5_PNG)).to_texture(ren_.get()));
        texs_.emplace("win_6", Surface::from_bytes(WINDOW_6_PNG, std::size(WINDOW_6_PNG)).to_texture(ren_.get()));
        texs_.emplace("win_7", Surface::from_bytes(WINDOW_7_PNG, std::size(WINDOW_7_PNG)).to_texture(ren_.get()));
    }

    term_clear();
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
    auto [w, h] = font_.cr2xy(ncol, nrow);

    // メインウィンドウの場合はメニューバーの分を足す
    if (is_main_)
        h += MAIN_WIN_MENU_H;

    return { w, h };
}

std::pair<int, int> GameWindow::term_size_for(int w, int h) const {
    // メインウィンドウの場合はメニューバーの分を引く
    if (is_main_)
        h = std::max(1, h - MAIN_WIN_MENU_H);

    auto [ncol, nrow] = font_.xy2cr(w, h);

    // メインウィンドウの場合は最小端末サイズを下回らないようにする
    if (is_main_) {
        chmax(ncol, MAIN_WIN_NCOL_MIN);
        chmax(nrow, MAIN_WIN_NROW_MIN);
    }

    return { ncol, nrow };
}

std::pair<int, int> GameWindow::term_size_for(const std::pair<int, int>& wh) const {
    return term_size_for(wh.first, wh.second);
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

void GameWindow::set_visible(const bool visible) const {
    // メインウィンドウは非表示にできない
    if (visible || is_main_)
        SDL_ShowWindow(win_.get());
    else
        SDL_HideWindow(win_.get());
}

void GameWindow::toggle_visible() const {
    set_visible(!is_visible());
}

void GameWindow::raise() const {
    SDL_RaiseWindow(win_.get());
}

std::pair<int, int> GameWindow::term_size() const {
    return ncnr_;
}

void GameWindow::term_clear() const {
    if (SDL_SetRenderTarget(ren_.get(), tex_term_.get()) != 0)
        PANIC("SDL_SetRenderTarget() failed");

    if (SDL_SetRenderDrawColor(ren_.get(), 0, 0, 0, 0xFF) != 0)
        PANIC("SDL_SetRenderDrawColor() failed");
    if (SDL_RenderClear(ren_.get()) != 0)
        PANIC("SDL_RenderClear() failed");
}

void GameWindow::term_fill_rect(const int c, const int r, const int ncol, const int nrow, Color color) const {
    if (SDL_SetRenderTarget(ren_.get(), tex_term_.get()) != 0)
        PANIC("SDL_SetRenderTarget() failed");

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

    if (SDL_SetRenderTarget(ren_.get(), tex_term_.get()) != 0)
        PANIC("SDL_SetRenderTarget() failed");

    const auto surf = font_.render(text, fg, bg);

    const auto [x, y] = font_.cr2xy(c, r);
    const SDL_Rect rect { x, y, surf.get()->w, surf.get()->h };

    const auto tex = surf.to_texture(ren_.get());
    if (SDL_RenderCopy(ren_.get(), tex.get(), nullptr, &rect) != 0)
        PANIC("SDL_RenderCopy() failed");
}

void GameWindow::term_draw_wall(const int c, const int r, Color color) const {
    if (SDL_SetRenderTarget(ren_.get(), tex_term_.get()) != 0)
        PANIC("SDL_SetRenderTarget() failed");

    const auto rect = font_.calc_rect(c, r, 1, 1);
    if (SDL_SetTextureColorMod(tex_wall_.get(), color.r(), color.g(), color.b()) != 0)
        PANIC("SDL_SetTextureColorMod() failed");
    if (SDL_RenderCopy(ren_.get(), tex_wall_.get(), nullptr, &rect) != 0)
        PANIC("SDL_RenderCopy() failed");
}

void GameWindow::present(const PresentParam& param) const {
    if (SDL_SetRenderTarget(ren_.get(), nullptr) != 0)
        PANIC("SDL_SetRenderTarget() failed");

    // 端末画面表示
    {
        // メインウィンドウの場合、メニューバーの下に表示
        int x = 0;
        int y = 0;
        if (is_main_)
            y += MAIN_WIN_MENU_H;

        int w, h;
        if (SDL_QueryTexture(tex_term_.get(), nullptr, nullptr, &w, &h) != 0)
            PANIC("SDL_QueryTexture() failed");
        const SDL_Rect rect { x, y, w, h };
        if (SDL_RenderCopy(ren_.get(), tex_term_.get(), nullptr, &rect) != 0)
            PANIC("SDL_RenderCopy() failed");
    }

    // メインウィンドウの場合、メニューバー表示
    if (is_main_) {
        for (const auto i : IRANGE(1, 8)) {
            const int x = client_area_size().first - 32 * 7 + 32 * (i - 1);
            const SDL_Rect rect { x, 0, 32, 32 };

            const auto& tex = texs_.at(FORMAT("win_{}", i));
            if (SDL_RenderCopy(ren_.get(), tex.get(), nullptr, &rect) != 0)
                PANIC("SDL_RenderCopy() failed");

            const auto color = param.visibles[i] ? Color(0xC0, 0xC0, 0x20, 0x60) : Color(0x30, 0x30, 0x30, 0x60);
            if (SDL_SetRenderDrawBlendMode(ren_.get(), SDL_BLENDMODE_BLEND) != 0)
                PANIC("SDL_SetRenderDrawBlendMode() failed");
            if (SDL_SetRenderDrawColor(ren_.get(), color.r(), color.g(), color.b(), color.a()) != 0)
                PANIC("SDL_SetRenderDrawColor() failed");
            if (SDL_RenderFillRect(ren_.get(), &rect) != 0)
                PANIC("SDL_RenderFillRect() failed");
        }
    }

    SDL_RenderPresent(ren_.get());
}

std::pair<int, int> GameWindow::on_size_change(const int w, const int h) {
    // 端末画面サイズが変わる場合、端末画面テクスチャを作り直す
    const auto ncnr_new = term_size_for(w, h);
    if (ncnr_ != ncnr_new) {
        ncnr_ = ncnr_new;
        tex_term_ = init_tex_term();
    }

    return ncnr_new;
}

ClickResponse GameWindow::on_click(const int x, const int y) const {
    ClickResponse resp { -1 };

    if (is_main_ && y < MAIN_WIN_MENU_H) {
        const auto i = (x - (client_area_size().first - 32 * 8)) / 32;
        if (1 <= i && i < 8)
            resp.win_idx = i;
    }

    return resp;
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
