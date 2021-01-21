#pragma once

#include <string>

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

public:
    GameWindowDesc();

    GameWindowDesc& title(std::string title);
    GameWindowDesc& x(int x);
    GameWindowDesc& y(int y);
    GameWindowDesc& w(int w);
    GameWindowDesc& h(int h);
    GameWindowDesc& font_path(std::string path);
    GameWindowDesc& font_pt(int pt);

    [[nodiscard]] GameWindow build(bool is_main) const;
};

class GameWindow : private boost::noncopyable {
private:
    Font font_;
    Window win_;
    Renderer ren_;
    Texture tex_wall_;

    [[nodiscard]] Texture init_tex_wall() const;

    GameWindow(Font font, Window win);

    friend class GameWindowDesc;

public:
    [[nodiscard]] u32 id() const;

    [[nodiscard]] bool is_visible() const;
    void set_visible(bool visible);

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
};
