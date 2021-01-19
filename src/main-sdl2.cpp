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

constexpr char FONT_PATH[] = "/usr/share/fonts/truetype/vlgothic/VL-Gothic-Regular.ttf";
constexpr int FONT_PT = 16;

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

constexpr int TERM_COUNT = 8;

// clang-format off
constexpr std::array<int, TERM_COUNT> TERM_IDS { 0, 1, 2, 3, 4, 5, 6, 7 };
// clang-format on

// (ncol, nrow)
constexpr std::array<std::pair<int, int>, TERM_COUNT> TERM_SIZES_INI { {
    { 100, 30 },
    { 20, 20 },
    { 20, 20 },
    { 20, 20 },
    { 20, 20 },
    { 20, 20 },
    { 20, 20 },
    { 20, 20 },
} };

class Font {
private:
    TTF_Font* font_;
    int w_;
    int h_;

public:
    Font(const std::string& path, const int pt) {
        ENSURE(font_ = TTF_OpenFont(path.c_str(), pt));
        ENSURE(TTF_SizeUTF8(font_, "X", &w_, &h_) == 0);
    }

    [[nodiscard]] int w() const { return w_; }
    [[nodiscard]] int h() const { return h_; }

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

public:
    Window(
        const std::string& title,
        const int x, const int y, const int ncol, const int nrow,
        Font& font, SDL_Surface* surf_wall)
        : font_(font) {
        const auto w = font_.w() * ncol;
        const auto h = font_.h() * nrow;
        ENSURE(win_ = SDL_CreateWindow(title.c_str(), x, y, w, h, SDL_WINDOW_RESIZABLE));
        ENSURE(ren_ = SDL_CreateRenderer(win_, -1, 0));
        ENSURE(tex_wall_ = SDL_CreateTextureFromSurface(ren_, surf_wall));
    }

    [[nodiscard]] u32 id() const {
        u32 res = SDL_GetWindowID(win_);
        ENSURE(res != 0);
        return res;
    }

    [[nodiscard]] const Font& font() const { return font_; }

    void clear() const {
        ENSURE(SDL_RenderClear(ren_) == 0);
    }

    void draw_text(const int c, const int r, const std::string& text, SDL_Color fg, SDL_Color bg) const {
        auto* surf = font_.render(text, fg, bg);
        SDL_Texture* tex;
        ENSURE(tex = SDL_CreateTextureFromSurface(ren_, surf));
        SDL_Rect rect { font_.w() * c, font_.h() * r, surf->w, surf->h };
        ENSURE(SDL_RenderCopy(ren_, tex, nullptr, &rect) == 0);
    }

    void draw_wall(const int c, const int r, SDL_Color color) const {
        SDL_Rect rect { font_.w() * c, font_.h() * r, font_.w(), font_.h() };
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
    const auto sym = ev.keysym.sym;

    if (0x20 <= sym && sym <= 0x7E) {
        term_key_push(sym);
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
    const auto font_w = win->font().w();
    const auto font_h = win->font().h();

    const auto ncol = ev.data1 / font_w;
    const auto nrow = ev.data2 / font_h;
    term_activate(&terms[term_id]);
    term_resize(ncol, nrow);

    return 0;
}

errr on_window(const SDL_WindowEvent& ev) {
    const auto term_id = window_id_to_term_id(ev.windowID);

    errr res = 0;

    switch (ev.event) {
    //case SDL_WINDOWEVENT_SHOWN:
    case SDL_WINDOWEVENT_EXPOSED:
        //case SDL_WINDOWEVENT_FOCUS_GAINED:
        //window_redraw(term_id);
        break;
#if 0
    case SDL_WINDOWEVENT_HIDDEN:
        EPRINTLN("SDL_WINDOWEVENT_HIDDEN {}", term_id);
        break;
    case SDL_WINDOWEVENT_MOVED:
        EPRINTLN("SDL_WINDOWEVENT_MOVED {}", term_id);
        break;
    case SDL_WINDOWEVENT_RESIZED:
        EPRINTLN("SDL_WINDOWEVENT_RESIZED {}", term_id);
        break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
        EPRINTLN("SDL_WINDOWEVENT_TAKE_FOCUS {}", term_id);
        break;
#endif
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
    case SDL_QUIT:
        PANIC("SDL_QUIT");
        break;
    default:
        break;
    }

    term_activate(&terms[term_id_orig]);

    return res;
}

errr poll_event() {
    SDL_Event ev;
    if (SDL_PollEvent(&ev) == 0) return 1;
    //if (SDL_WaitEventTimeout(&ev, 10) == 0) return 1;
    return handle_event(ev);
}

errr wait_event() {
    SDL_Event ev;
    ENSURE(SDL_WaitEvent(&ev) != 0);
    return handle_event(ev);
}

errr flush_events() {
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    return 0;
}

errr term_xtra_sdl2(const int name, const int value) {
    errr res = 0;

    switch (name) {
    case TERM_XTRA_EVENT:
        res = value == 0 ? wait_event() : poll_event();
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
        //win->present();
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
    case TERM_XTRA_REACT:
        break;
    default:
        break;
    }

    return res;
}

errr term_curs_sdl2(const int c, const int r) {
    // TODO: stub
    EPRINTLN("curs {} {}", c, r);
    return 0;
}

errr term_bigcurs_sdl2(const int c, const int r) {
    // TODO: stub
    EPRINTLN("bigcurs {} {}", c, r);
    return 0;
}

errr term_wipe_sdl2(const int c, const int r, const int n) {
    // TODO: stub
    EPRINTLN("wipe {} {} {}", c, r, n);
    return 0;
}

errr term_text_sdl2(const TERM_LEN c, const TERM_LEN r, const int n, const TERM_COLOR attr, const char* euc_arg) {
    using std::begin, std::end;

    constexpr char CH_WALL = 0x7F;

    const auto* win = wins[current_term_id()];

    std::string euc(euc_arg, n);
    std::vector<int> offs_wall;
    {
        for (auto it = begin(euc); it != end(euc);) {
            if (*it == CH_WALL) {
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
    const SDL_Color fg { angband_color_table[attr][1], angband_color_table[attr][2], angband_color_table[attr][3], 255 };
    const SDL_Color bg { 0, 0, 0, 255 };
    win->draw_text(c, r, utf8, fg, bg);

    for (const auto off : offs_wall) {
        win->draw_wall(c + off, r, fg);
    }

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
        const auto [ncol, nrow] = TERM_SIZES_INI[i];

        const auto* win = wins[i] = new Window(FORMAT("Term-{}", i), 0, 0, ncol, nrow, *font, surf_wall);
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

    term_activate(angband_term[0]);
}
