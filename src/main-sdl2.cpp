#include <algorithm>
#include <array>
#include <cerrno>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <iconv.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "system/angband.h"
#include "term/gameterm.h"
#include "term/term-color-types.h"
#include "term/z-term.h"

#include "prelude.hpp"

namespace {

// clang-format off
#define ENSURE(cond) do { if (!(cond)) { detail::PANIC_IMPL(__FILE__, __LINE__, "{}", "`" #cond "` is not satisfied"); } } while (false)
// clang-format on

template <class InputIt>
u16 euc_next(InputIt& it, InputIt last) {
    ENSURE(it != last);

    const u8 b1 = *it++;

    if (b1 <= 0x7F) {
        return b1;
    }

    if (0xA1 <= b1 && b1 <= 0xFE) {
        if (it == last) PANIC("incomplete euc character");
        const u8 b2 = *it++;
        return b1 | (b2 << 8);
    }

    PANIC("invalid euc character");
}

std::string euc_to_utf8(const std::string& euc) {
    constexpr std::size_t BUF_SIZE = 1024;

    static iconv_t conv = iconv_open("UTF-8", "EUC-JP");
    ENSURE(conv != iconv_t(-1));

    const auto* src = euc.data();
    auto src_size = euc.size();
    std::string utf8;
    utf8.reserve(src_size);

    char buf[BUF_SIZE];
    while (src_size > 0) {
        auto* dst = buf;
        auto dst_size = BUF_SIZE;

        const auto n = iconv(conv, const_cast<char**>(&src), &src_size, &dst, &dst_size);
        ENSURE(n != std::size_t(-1) || errno == E2BIG);

        utf8.append(buf, BUF_SIZE - dst_size);
    }

    return utf8;
}

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

constexpr char FONT_PATH[] = "/usr/share/fonts/truetype/vlgothic/VL-Gothic-Regular.ttf";
constexpr int FONT_PT = 18;

constexpr int TERM_COUNT = 8;
constexpr int TERM_ENABLE_COUNT = 5;

// clang-format off
constexpr std::array<int, TERM_COUNT> TERM_IDS { 0, 1, 2, 3, 4, 5, 6, 7 };

// (x, y)
constexpr std::array<std::pair<int, int>, TERM_COUNT> TERM_POSS_INI { {
    {    0,   0 },
    {    0, 740 },
    { 1360,   0 },
    { 1360, 260 },
    { 1360, 740 },
    {    0,   0 },
    {    0,   0 },
    {    0,   0 },
} };

// (ncol, nrow)
constexpr std::array<std::pair<int, int>, TERM_COUNT> TERM_SIZES_INI { {
    { 150, 40 },
    { 150, 16 },
    {  61, 12 },
    {  61, 23 },
    {  61, 15 },
    {  10, 10 },
    {  10, 10 },
    {  10, 10 },
} };
// clang-format on

class Font {
private:
    TTF_Font* font_;
    int w_;
    int h_;

public:
    Font(const std::string& path, const int pt) {
        ENSURE(font_ = TTF_OpenFont(path.c_str(), pt));

        // 半角文字の幅を得る (等幅フォントを仮定)
        ENSURE(TTF_SizeUTF8(font_, "X", &w_, nullptr) == 0);

        // 文字の高さの最大値を得る。
        // TTF_FontHeight() は最大値より小さい値を返すことがあるため、
        // TTF_SizeUTF8() に全ての ASCII 文字からなる文字列を渡す方法をとる。
        // 非 ASCII 文字については全て調べるのが非現実的なので妥協する。
        ENSURE(TTF_SizeUTF8(font_, " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", nullptr, &h_) == 0);
    }

    [[nodiscard]] int w() const { return w_; }
    [[nodiscard]] int h() const { return h_; }

    [[nodiscard]] std::pair<int, int> xy2cr(const int x, const int y) const {
        return { x / w_, y / h_ };
    }

    [[nodiscard]] std::pair<int, int> cr2xy(const int c, const int r) const {
        return { w_ * c, h_ * r };
    }

    [[nodiscard]] SDL_Surface* render(const std::string& text, SDL_Color fg, SDL_Color bg) const {
        SDL_Surface* surf;
        ENSURE(surf = TTF_RenderUTF8_Shaded(font_, text.c_str(), fg, bg));
        return surf;
    }
};

class Window {
private:
    SDL_Window* win_;
    SDL_Renderer* ren_;
    Font& font_;
    SDL_Texture* tex_wall_;

    // 元画像 surf_orig をサイズ (w,h) の領域にリピートしたテクスチャを作る。
    static SDL_Texture* make_wall_texture(SDL_Renderer* ren, SDL_Surface* surf_orig, const int w, const int h) {
        const int w_orig = surf_orig->w;
        const int h_orig = surf_orig->h;

        SDL_Surface* surf;
        ENSURE(surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32));
        for (int y = 0; y < h; y += h_orig) {
            for (int x = 0; x < w; x += w_orig) {
                SDL_Rect rect { x, y, w_orig, h_orig };
                ENSURE(SDL_BlitSurface(surf_orig, nullptr, surf, &rect) == 0);
            }
        }

        SDL_Texture* tex;
        ENSURE(tex = SDL_CreateTextureFromSurface(ren, surf));
        return tex;
    }

public:
    Window(
        const std::string& title,
        const int x, const int y, const int ncol, const int nrow,
        Font& font, SDL_Surface* surf_wall)
        : font_(font) {
        const auto [w, h] = font_.cr2xy(ncol, nrow);

        ENSURE(win_ = SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_RESIZABLE));
        ENSURE(ren_ = SDL_CreateRenderer(win_, -1, 0));

        tex_wall_ = make_wall_texture(ren_, surf_wall, font_.w(), font_.h());
    }

    [[nodiscard]] u32 id() const {
        u32 res = SDL_GetWindowID(win_);
        ENSURE(res != 0);
        return res;
    }

    [[nodiscard]] const Font& font() const { return font_; }

    void hide() const {
        SDL_HideWindow(win_);
    }

    void clear() const {
        ENSURE(SDL_SetRenderDrawColor(ren_, 0, 0, 0, 0xFF) == 0);
        ENSURE(SDL_RenderClear(ren_) == 0);
    }

    void draw_blanks(const int c, const int r, const int n) const {
        const auto [x, y] = font_.cr2xy(c, r);
        const auto [w, h] = font_.cr2xy(n, 1);
        const SDL_Rect rect { x, y, w, h };

        ENSURE(SDL_SetRenderDrawColor(ren_, 0, 0, 0, 0xFF) == 0);
        ENSURE(SDL_RenderFillRect(ren_, &rect) == 0);
    }

    void draw_curs(const int c, const int r) const {
        const auto [x, y] = font_.cr2xy(c, r);
        const auto [w, h] = font_.cr2xy(1, 1);
        const SDL_Rect rect { x, y, w, h };

        ENSURE(SDL_SetRenderDrawColor(ren_, 0xFF, 0xFF, 0xFF, 0xFF) == 0);
        ENSURE(SDL_RenderFillRect(ren_, &rect) == 0);
    }

    void draw_bigcurs(const int c, const int r) const {
        const auto [x, y] = font_.cr2xy(c, r);
        const auto [w, h] = font_.cr2xy(2, 1);
        const SDL_Rect rect { x, y, w, h };

        ENSURE(SDL_SetRenderDrawColor(ren_, 0xFF, 0xFF, 0xFF, 0xFF) == 0);
        ENSURE(SDL_RenderFillRect(ren_, &rect) == 0);
    }

    void draw_text(const int c, const int r, const std::string& text, SDL_Color fg, SDL_Color bg) const {
        auto* surf = font_.render(text, fg, bg);

        const auto [x, y] = font_.cr2xy(c, r);
        const SDL_Rect rect { x, y, surf->w, surf->h };

        SDL_Texture* tex;
        ENSURE(tex = SDL_CreateTextureFromSurface(ren_, surf));
        ENSURE(SDL_RenderCopy(ren_, tex, nullptr, &rect) == 0);
    }

    void draw_wall(const int c, const int r, SDL_Color color) const {
        const auto [x, y] = font_.cr2xy(c, r);
        const auto [w, h] = font_.cr2xy(1, 1);
        const SDL_Rect rect { x, y, w, h };

        ENSURE(SDL_SetTextureColorMod(tex_wall_, color.r, color.g, color.b) == 0);
        ENSURE(SDL_RenderCopy(ren_, tex_wall_, nullptr, &rect) == 0);
    }

    void present() const {
        SDL_RenderPresent(ren_);
    }
};

class System {
public:
    System() {
        ENSURE(SDL_Init(SDL_INIT_VIDEO) == 0);
        ENSURE(TTF_Init() == 0);
    }
};

System* sys {};
Font* font {};

std::array<Window*, TERM_COUNT> wins {};

std::array<term_type, TERM_COUNT> terms {};

int current_term_id() {
    return *static_cast<int*>(Term->data);
}

int window_id_to_term_id(const u32 win_id) {
    for (const auto i : IRANGE(TERM_COUNT)) {
        if (wins[i]->id() == win_id)
            return i;
    }
    PANIC("invalid window id: {}", win_id);
}

errr on_keydown(const SDL_KeyboardEvent& ev) {
    constexpr int TERM_KEY_BS = '\x08';
    constexpr int TERM_KEY_TAB = '\x09';
    constexpr int TERM_KEY_CR = '\x0D';
    constexpr int TERM_KEY_ESC = '\x1B';
    constexpr int TERM_KEY_DEL = '\x7F';

    const auto TERM_KEY_CTRL = [](const int key) { return key & 0x1F; };

    const auto sym = ev.keysym.sym;
    const auto shift = bool(ev.keysym.mod & KMOD_SHIFT);
    const auto ctrl = bool(ev.keysym.mod & KMOD_CTRL);

    if (ctrl) {
        switch (sym) {
        case '[': term_key_push(TERM_KEY_ESC); break;
        default:
            if ('a' <= sym && sym <= 'z')
                term_key_push(TERM_KEY_CTRL(sym));
            break;
        }
    }
    else if (shift) {
        switch (sym) {
        case '1': term_key_push('!'); break;
        case '2': term_key_push('"'); break;
        case '3': term_key_push('#'); break;
        case '4': term_key_push('$'); break;
        case '5': term_key_push('%'); break;
        case '6': term_key_push('&'); break;
        case '7': term_key_push('\''); break;
        case '8': term_key_push('('); break;
        case '9': term_key_push(')'); break;
        case '-': term_key_push('='); break;
        case '^': term_key_push('~'); break;
        case '@': term_key_push('`'); break;
        case '[': term_key_push('{'); break;
        case ';': term_key_push('+'); break;
        case ':': term_key_push('*'); break;
        case ']': term_key_push('}'); break;
        case ',': term_key_push('<'); break;
        case '.': term_key_push('>'); break;
        case '/': term_key_push('?'); break;
        case '\\':
            if (ev.keysym.scancode == SDL_SCANCODE_INTERNATIONAL3)
                term_key_push('|');
            else if (ev.keysym.scancode == SDL_SCANCODE_INTERNATIONAL1)
                term_key_push('_');
            break;
        default:
            if ('a' <= sym && sym <= 'z')
                term_key_push(sym & ~0x20);
            break;
        }
    }
    else {
        switch (sym) {
        case SDLK_BACKSPACE: term_key_push(TERM_KEY_BS); break;
        case SDLK_TAB: term_key_push(TERM_KEY_TAB); break;
        case SDLK_RETURN: term_key_push(TERM_KEY_CR); break;
        case SDLK_ESCAPE: term_key_push(TERM_KEY_ESC); break;
        case SDLK_DELETE: term_key_push(TERM_KEY_DEL); break;
        default:
            if (0x20 <= sym && sym <= 0x7E)
                term_key_push(sym);
            break;
        }
    }

    return 0;
}

void window_redraw(const int term_id) {
    const auto* win = wins[term_id];
    win->clear();

    auto* term = &terms[term_id];
    term_activate(term);
    term_redraw();

    win->present();
}

errr on_window_size_change(const SDL_WindowEvent& ev, const int term_id) {
    const auto* win = wins[term_id];

    const auto [ncol, nrow] = win->font().xy2cr(ev.data1, ev.data2);

    term_activate(&terms[term_id]);
    term_resize(ncol, nrow);

    return 0;
}

errr on_window(const SDL_WindowEvent& ev) {
    const auto term_id = window_id_to_term_id(ev.windowID);

    errr res = 0;

    switch (ev.event) {
    case SDL_WINDOWEVENT_EXPOSED:
        window_redraw(term_id);
        break;
    case SDL_WINDOWEVENT_SIZE_CHANGED: {
        res = on_window_size_change(ev, term_id);
        break;
    }
    default:
        break;
    }

    return res;
}

errr handle_event(const SDL_Event& ev) {
    const auto term_id_orig = current_term_id();

    errr res = 0;

    switch (ev.type) {
    case SDL_KEYDOWN:
        res = on_keydown(ev.key);
        break;
    case SDL_WINDOWEVENT:
        res = on_window(ev.window);
        break;
    // SDL_QUIT は無視する
    default:
        break;
    }

    term_activate(&terms[term_id_orig]);

    return res;
}

errr poll_event() {
    SDL_Event ev;
    if (SDL_PollEvent(&ev) == 0) return 1;
    return handle_event(ev);
}

errr wait_event() {
    SDL_Event ev;
    ENSURE(SDL_WaitEvent(&ev) != 0);
    return handle_event(ev);
}

errr flush_events() {
    while (poll_event() == 0)
        ;
    return 0;
}

errr term_xtra_sdl2(const int name, const int value) {
    errr res = 0;

    switch (name) {
    case TERM_XTRA_EVENT:
        res = value == 0 ? poll_event() : wait_event();
        break;
    case TERM_XTRA_BORED:
        res = poll_event();
        break;
    case TERM_XTRA_FLUSH:
        res = flush_events();
        break;
    case TERM_XTRA_CLEAR: {
        const auto* win = wins[current_term_id()];
        win->clear();
        break;
    }
    case TERM_XTRA_FRESH: {
        const auto* win = wins[current_term_id()];
        win->present();
        break;
    }
    case TERM_XTRA_DELAY:
        SDL_Delay(value);
        break;
    default:
        break;
    }

    return res;
}

errr term_curs_sdl2(const int c, const int r) {
    const auto* win = wins[current_term_id()];
    win->draw_curs(c, r);

    return 0;
}

errr term_bigcurs_sdl2(const int c, const int r) {
    const auto* win = wins[current_term_id()];
    win->draw_bigcurs(c, r);

    return 0;
}

errr term_wipe_sdl2(const int c, const int r, const int n) {
    const auto* win = wins[current_term_id()];
    win->draw_blanks(c, r, n);

    return 0;
}

errr term_text_sdl2(const TERM_LEN c, const TERM_LEN r, const int n, const TERM_COLOR attr, const char* euc_arg) {
    using std::begin, std::end;

    // 壁の内部コード。lib/pref/font-sdl.prf で指定されている。
    // この値は EUC-JP と干渉しない。
    constexpr char CH_WALL = 0x7F;

    const auto* win = wins[current_term_id()];

    // Byte 数 == 文字幅と仮定

    std::string euc(euc_arg, n);
    std::vector<int> offs_wall; // 壁描画位置たち(文字単位)
    {
        for (auto it = begin(euc); it != end(euc);) {
            if (*it == CH_WALL) {
                // 壁の内部コードが現れたら位置を記録し、通常の文字に置換する
                offs_wall.emplace_back(std::distance(begin(euc), it));
                *it = '#';
                ++it;
            }
            else {
                euc_next(it, end(euc));
            }
        }
    }
    const auto utf8 = euc_to_utf8(euc);

    const SDL_Color fg {
        angband_color_table[attr][1],
        angband_color_table[attr][2],
        angband_color_table[attr][3],
        0xFF
    };
    const SDL_Color bg { 0, 0, 0, 0xFF };

    // 最初に draw_blanks() を行わないと画面にゴミが残ることがある。
    // draw_text() の描画範囲はテキスト内の文字によって変動するため。
    // (等幅フォントであっても高さは文字ごとに異なる)
    win->draw_blanks(c, r, n);

    // 先にテキストを描画
    win->draw_text(c, r, utf8, fg, bg);

    // 後から壁を描画
    for (const auto off : offs_wall)
        win->draw_wall(c + off, r, fg);

    return 0;
}

SDL_Surface* make_wall_surface() {
    using std::begin, std::size;

    SDL_RWops* rdr;
    ENSURE(rdr = SDL_RWFromConstMem(begin(WALL_BMP), size(WALL_BMP)));

    SDL_Surface* surf;
    ENSURE(surf = SDL_LoadBMP_RW(rdr, 1));

    return surf;
}

} // anonymous namespace

void init_sdl2(int /*argc*/, char** /*argv*/) {
    sys = new System;
    font = new Font(FONT_PATH, FONT_PT);

    auto* surf_wall = make_wall_surface();

    for (const auto i : IRANGE(TERM_COUNT)) {
        const auto [x, y] = TERM_POSS_INI[i];
        const auto [ncol, nrow] = TERM_SIZES_INI[i];

        const auto* win = wins[i] = new Window(FORMAT("Term-{}", i), x, y, ncol, nrow, *font, surf_wall);
        win->clear();
        win->present();

        auto* term = &terms[i];
        term_init(term, ncol, nrow, 1024);
        term->attr_blank = TERM_WHITE;
        term->char_blank = ' ';
        term->xtra_hook = term_xtra_sdl2;
        term->curs_hook = term_curs_sdl2;
        term->bigcurs_hook = term_bigcurs_sdl2;
        term->wipe_hook = term_wipe_sdl2;
        term->text_hook = term_text_sdl2;
        term->data = const_cast<void*>(static_cast<const void*>(&TERM_IDS[i]));
        angband_term[i] = term;
    }

    for (const auto i : IRANGE(TERM_ENABLE_COUNT, TERM_COUNT))
        wins[i]->hide();

    term_activate(angband_term[0]);
}
