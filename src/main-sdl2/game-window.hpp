#pragma once

#include <string>
#include <utility>

#include <boost/core/noncopyable.hpp>

#include "main-sdl2/font.hpp"
#include "main-sdl2/prelude.hpp"
#include "main-sdl2/system.hpp"

class GameWindow;

class GameWindowDesc {
private:
    std::string title_;
    int x_;
    int y_;
    int w_;
    int h_;
    std::string font_path_;
    int font_pt_;
    bool visible_;

public:
    GameWindowDesc();

    [[nodiscard]] const std::string& title() const;
    GameWindowDesc& title(std::string title);

    [[nodiscard]] int x() const;
    GameWindowDesc& x(int x);

    [[nodiscard]] int y() const;
    GameWindowDesc& y(int y);

    [[nodiscard]] int w() const;
    GameWindowDesc& w(int w);

    [[nodiscard]] int h() const;
    GameWindowDesc& h(int h);

    [[nodiscard]] const std::string& font_path() const;
    GameWindowDesc& font_path(std::string path);

    [[nodiscard]] int font_pt() const;
    GameWindowDesc& font_pt(int pt);

    [[nodiscard]] bool visible() const;
    GameWindowDesc& visible(bool visible);

    [[nodiscard]] GameWindow build(bool is_main) const;
};

class GameWindow {
private:
    Font font_;
    Window win_;
    Renderer ren_;
    Texture tex_wall_;

    [[nodiscard]] Texture init_tex_wall() const;

    GameWindow(Font font, Window win);

    // ウィンドウ位置を得る。
    [[nodiscard]] std::pair<int, int> pos() const;

    // クライアント領域のサイズを得る。
    [[nodiscard]] std::pair<int, int> client_area_size() const;

    friend class GameWindowDesc;

public:
    // noncopyable
    GameWindow(GameWindow&) = delete;
    GameWindow& operator=(GameWindow&) = delete;

    // movable
    GameWindow(GameWindow&& other) noexcept = default;
    GameWindow& operator=(GameWindow&& rhs) noexcept = default;

    // FIXME: リサイズ処理に必要なので生やしたが、リファクタの余地ありそう
    [[nodiscard]] const Font& font() const;

    [[nodiscard]] u32 id() const;

    [[nodiscard]] bool is_visible() const;
    void set_visible(bool visible);

    // 端末画面の (ncol,nrow) を得る。
    [[nodiscard]] std::pair<int, int> term_size() const;

    // 端末画面全体をクリアする。
    void term_clear() const;

    // 端末画面の矩形範囲 (c,r,ncol,nrow) を色 color で塗りつぶす。
    void term_fill_rect(int c, int r, int ncol, int nrow, Color color) const;

    // 端末画面の文字座標 (c,r) に UTF-8 文字列 text を色 (fg,bg) で描画する。
    //
    // XXX:
    //   事前に term_fill_rect() で対象範囲をクリアしておかないと画面にゴミが残
    //   ることがある。等幅フォントであっても文字ごとに高さが異なるのが原因。
    //   文字幅計算の都合上、このクリア処理は呼び出し側に任せている。
    //   (とはいえ EUC-JP でもバイト数と文字幅は厳密には一致しないが)
    void term_draw_text(int c, int r, const std::string& text, Color fg, Color bg) const;

    // 端末画面の文字座標 (c,r) に壁画像を色 color で描画する。
    void term_draw_wall(int c, int r, Color color) const;

    // バッファに描画した内容を画面に反映する。
    void present() const;

    [[nodiscard]] GameWindowDesc desc() const;
};
