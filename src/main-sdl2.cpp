#include <algorithm>
#include <array>
#include <string>
#include <utility>

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

constexpr int TERM_COUNT = 8;

constexpr char FONT_PATH[] = "/usr/share/fonts/truetype/vlgothic/VL-Gothic-Regular.ttf";
constexpr int FONT_PT = 16;

constexpr std::array<int, TERM_COUNT> TERM_IDS {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
};

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
        return TTF_RenderUTF8_Shaded(font_, text.c_str(), fg, bg);
    }
};

class Window {
private:
    SDL_Window* win_;
    SDL_Renderer* ren_;
    Font& font_;

public:
    Window(const std::string& title, const int x, const int y, const int ncol, const int nrow, Font& font)
        : font_(font) {
        const auto w = font_.w() * ncol;
        const auto h = font_.h() * nrow;
        ENSURE(win_ = SDL_CreateWindow(title.c_str(), x, y, w, h, 0));
        ENSURE(ren_ = SDL_CreateRenderer(win_, -1, 0));
    }

    [[nodiscard]] u32 id() const {
        u32 res = SDL_GetWindowID(win_);
        ENSURE(res != 0);
        return res;
    }

    void clear() const {
        ENSURE(SDL_RenderClear(ren_) == 0);
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
    return 0;
}

errr on_window_expose(const SDL_WindowEvent&, const int term_id) {
    // FIXME: for debug
    EPRINTLN("window expose: term_id={}", term_id);
    auto* term = &terms[term_id];
    term_activate(term);
    return 0;
}

errr on_window(const SDL_WindowEvent& ev) {
    const auto term_id = window_id_to_term_id(ev.windowID);

    errr res = 0;

    switch (ev.type) {
    case SDL_WINDOWEVENT_EXPOSED:
        res = on_window_expose(ev, term_id);
    default:
        break;
    }

    return res;
}

errr handle_event(const SDL_Event& ev) {
    // FIXME: for debug
    EPRINTLN("handle_event");
    const auto term_id_orig = current_term_id();

    errr res = 0;

    switch (ev.type) {
    case SDL_KEYDOWN:
        res = on_keydown(ev.key);
        break;
    case SDL_WINDOWEVENT:
        res = on_window(ev.window);
        break;
    default:
        break;
    }

    term_activate(&terms[term_id_orig]);

    return res;
}

errr poll_event() {
    SDL_Event ev;
    //if (SDL_PollEvent(&ev) == 0) return 1;
    // FIXME: for debug
    if (SDL_PollEvent(&ev) == 0) {
        EPRINTLN("event queue empty");
        return 1;
    }
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
        // FIXME: for debug
        EPRINTLN("TERM_XTRA_EVENT {}", value);
        res = value == 0 ? wait_event() : poll_event();
        break;
    case TERM_XTRA_BORED:
        // FIXME: for debug
        EPRINTLN("TERM_XTRA_BORED");
        res = poll_event();
        break;
    case TERM_XTRA_FLUSH:
        // FIXME: for debug
        EPRINTLN("TERM_XTRA_FLUSH");
        res = flush_events();
        break;
    case TERM_XTRA_CLEAR: {
        // FIXME: for debug
        EPRINTLN("TERM_XTRA_CLEAR");
        const auto* win = wins[current_term_id()];
        win->clear();
        win->present();
        break;
    }
    case TERM_XTRA_DELAY:
        // FIXME: for debug
        EPRINTLN("TERM_XTRA_DELAY");
        SDL_Delay(value);
        break;
    case TERM_XTRA_REACT:
        // FIXME: for debug
        EPRINTLN("TERM_XTRA_REACT");
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

errr term_text_sdl2(const TERM_LEN c, const TERM_LEN r, const int n, const TERM_COLOR attr, const char* text) {
    // TODO: stub
    //EPRINTLN("text {} {} {} {} {}", c, r, n, attr, text);
    return 0;
}

} // anonymous namespace

void init_sdl2(int /*argc*/, char** /*argv*/) {
    sys = new System;
    font = new Font(FONT_PATH, FONT_PT);

    for (const auto i : IRANGE(TERM_COUNT)) {
        const auto [ncol, nrow] = TERM_SIZES_INI[i];

        wins[i] = new Window(FORMAT("Term-{}", i), 0, 0, ncol, nrow, *font);

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
