// SDL2 インターフェース
//
// エラー処理は基本的に「何か変なことが起きたら直ちにクラッシュする」方針で書いている。
// 実装が容易なことと、バグらせたとき即座にフィードバックが得られることが理由。
//
// XXX:
//   EUC-JP 文字列を扱う際、バイト数 == 文字幅 と仮定している箇所があるが、これ
//   は厳密には正しくない(3バイト文字や半角カナがあると崩れる)。この件について
//   は開発コミュニティの判断に委ねたい。

#include <array>
#include <string>
#include <utility>
#include <vector>

#include <SDL.h>

#include "system/angband.h"
#include "term/gameterm.h"
#include "term/term-color-types.h"
#include "term/z-term.h"

#include "main-sdl2/encoding.hpp"
#include "main-sdl2/game-window.hpp"
#include "main-sdl2/prelude.hpp"
#include "main-sdl2/system.hpp"

namespace {

constexpr int TERM_COUNT = 8;

// clang-format off
constexpr std::array<int, TERM_COUNT> TERM_IDS { 0, 1, 2, 3, 4, 5, 6, 7 };
// clang-format on

System* sys {};

std::vector<GameWindow> wins {};

std::array<term_type, TERM_COUNT> terms {};

int current_term_id() {
    return *static_cast<int*>(Term->data);
}

int window_id_to_term_id(const u32 win_id) {
    for (const auto i : IRANGE(TERM_COUNT)) {
        if (wins[i].id() == win_id)
            return i;
    }
    PANIC("invalid window id: {}", win_id);
}

// Shift や Ctrl は適宜変換する(とりあえず日本語キーボード決め打ち)。
// SDL_TextInputEvent で済ませられればよいのだが、それだと Ctrl などの変換を制御できないので。
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
    const auto& win = wins[term_id];
    win.term_clear();

    auto* term = &terms[term_id];
    term_activate(term);
    term_redraw();

    win.present();
}

errr on_window_size_change(const SDL_WindowEvent& ev, const int term_id) {
    const auto& win = wins[term_id];

    const auto [ncol, nrow] = win.font().xy2cr(ev.data1, ev.data2);

    term_activate(&terms[term_id]);
    term_resize(ncol, nrow);

    return 0;
}

errr on_window(const SDL_WindowEvent& ev) {
    const auto term_id = window_id_to_term_id(ev.windowID);

    errr res = 0;

    switch (ev.event) {
    case SDL_WINDOWEVENT_EXPOSED:
        // これを行わないとリサイズ中に真っ黒になることがある
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
    // SDL_QUIT は無視する。これにより端末からの Ctrl+C も無視される
    default:
        break;
    }

    // イベント処理中に Term が切り替わることがあるので元に戻す
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
    if (SDL_WaitEvent(&ev) == 0)
        PANIC("SDL_WaitEvent() failed");
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
        // UI 側イベントを1つ処理
        // 引数が 0 なら poll, 非0 なら wait
        res = value == 0 ? poll_event() : wait_event();
        break;
    case TERM_XTRA_BORED:
        // UI 側イベントを1つ処理 (poll)
        res = poll_event();
        break;
    case TERM_XTRA_FLUSH:
        // UI 側イベントを全て処理
        res = flush_events();
        break;
    case TERM_XTRA_CLEAR: {
        // 現在のウィンドウの内容をクリア
        const auto& win = wins[current_term_id()];
        win.term_clear();
        break;
    }
    case TERM_XTRA_FRESH: {
        // 現在のウィンドウの描画内容を反映
        const auto& win = wins[current_term_id()];
        win.present();
        break;
    }
    case TERM_XTRA_DELAY:
        // ディレイ (引数: ミリ秒)
        SDL_Delay(value);
        break;
    default:
        break;
    }

    return res;
}

errr term_curs_sdl2(const int c, const int r) {
    const auto& win = wins[current_term_id()];
    win.term_fill_rect(c, r, 1, 1, Color(0xFF, 0xFF, 0xFF, 0xFF));

    return 0;
}

errr term_bigcurs_sdl2(const int c, const int r) {
    const auto& win = wins[current_term_id()];
    win.term_fill_rect(c, r, 2, 1, Color(0xFF, 0xFF, 0xFF, 0xFF));

    return 0;
}

errr term_wipe_sdl2(const int c, const int r, const int n) {
    const auto& win = wins[current_term_id()];
    win.term_fill_rect(c, r, n, 1, Color(0, 0, 0, 0xFF));

    return 0;
}

errr term_text_sdl2(const TERM_LEN c, const TERM_LEN r, const int n, const TERM_COLOR attr, const char* euc_arg) {
    // 壁の内部コード。lib/pref/font-sdl.prf で指定されている。
    // この値は EUC-JP と干渉しない。
    constexpr char CH_WALL = 0x7F;

    const auto& win = wins[current_term_id()];

    // 入力文字列内の CH_WALL を '#' に置換し、そのインデックスを記録していく。
    // このインデックスたちを壁描画位置として使う。
    // CH_WALL を含む入力ではバイト数と文字幅が等しいと仮定している。
    // (EUC-JP の3バイト文字や半角カナを含まないということ)

    std::string euc(euc_arg, n);
    std::vector<int> offs_wall;
    for (const auto i : IRANGE(n)) {
        if (euc[i] == CH_WALL) {
            offs_wall.emplace_back(i);
            euc[i] = '#';
        }
    }

    const auto utf8 = euc_to_utf8(euc);

    const Color fg(
        angband_color_table[attr][1],
        angband_color_table[attr][2],
        angband_color_table[attr][3],
        0xFF);
    const Color bg(0, 0, 0, 0xFF);

    // 最初に draw_blanks() を行わないと画面にゴミが残ることがある。
    // draw_text() の描画範囲はテキスト内の文字によって変動するため。
    // (等幅フォントであっても高さは文字ごとに異なる)
    // XXX: バイト数と文字幅が一致すると仮定しているが、これは厳密には正しくない
    win.term_fill_rect(c, r, n, 1, Color(0, 0, 0, 0xFF));

    // 先にテキストを描画
    win.term_draw_text(c, r, utf8, fg, bg);

    // 後から壁を描画
    for (const auto off : offs_wall)
        win.term_draw_wall(c + off, r, fg);

    return 0;
}

std::vector<GameWindowDesc> get_window_descs() {
    return {
        GameWindowDesc().title("Hengband").x(0).y(0).w(1350).h(756).font_pt(18),
        GameWindowDesc().title("Term-1").x(0).y(780).w(1350).h(252).font_pt(18),
        GameWindowDesc().title("Term-2").x(1360).y(0).w(549).h(252).font_pt(18),
        GameWindowDesc().title("Term-3").x(1360).y(260).w(549).h(483).font_pt(18),
        GameWindowDesc().title("Term-4").x(1360).y(780).w(549).h(273).font_pt(18),
        GameWindowDesc().title("Term-5").visible(false),
        GameWindowDesc().title("Term-6").visible(false),
        GameWindowDesc().title("Term-7").visible(false),
    };
}

} // anonymous namespace

void init_sdl2(int /*argc*/, char** /*argv*/) {
    // ファイルローカルで確保したリソースの解放については考えない。
    // どうせプログラム終了時に解放されるので。

    sys = new System;

    const auto win_descs = get_window_descs();

    for (const auto i : IRANGE(TERM_COUNT)) {
        auto win = win_descs[i].build(i == 0);
        const auto [ncol, nrow] = win.term_size();
        wins.emplace_back(std::move(win));

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

    // これを行わないとクラッシュする
    // Term の初期化はドライバ側の責任らしい
    term_activate(angband_term[0]);
}
