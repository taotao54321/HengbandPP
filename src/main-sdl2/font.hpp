#pragma once

#include <optional>
#include <string>
#include <utility>

#include <boost/core/noncopyable.hpp>

#include <SDL_ttf.h>

#include "main-sdl2/system.hpp"

// 等幅フォントを仮定。
// 事前に TTF_Init() を呼んでおくこと。
class Font : private boost::noncopyable {
private:
    TTF_Font* font_;
    int w_;
    int h_;

    void drop();

public:
    Font(const std::string& path, int pt);

    // movable
    Font(Font&& other) noexcept;
    Font& operator=(Font&& rhs) noexcept;

    ~Font();

    // 文字の幅を得る。
    [[nodiscard]] int w() const;

    // 文字の高さを得る。
    [[nodiscard]] int h() const;

    // ピクセル座標 (x,y) を文字座標 (c,r) に変換する。
    [[nodiscard]] std::pair<int, int> xy2cr(int x, int y) const;

    // 文字座標 (c,r) をピクセル座標 (x,y) に変換する。
    [[nodiscard]] std::pair<int, int> cr2xy(int c, int r) const;

    // UTF-8 文字列 text を色 (fg,bg) で描画する。
    [[nodiscard]] Surface render(const std::string& text, Color fg, Color bg) const;
};

// フォント名 name に対応するフォントファイルのフルパスを得る。
// 失敗したら std::nullopt を返す。
std::optional<std::string> get_font_path(const std::string& name);
