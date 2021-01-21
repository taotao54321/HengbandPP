#include <string>
#include <utility>

#include <SDL.h>
#include <SDL_ttf.h>

#include "main-sdl2/font.hpp"
#include "main-sdl2/prelude.hpp"
#include "main-sdl2/system.hpp"

namespace {

} // anonymous namespace

Font::Font(const std::string& path, const int pt)
    : font_(TTF_OpenFont(path.c_str(), pt)) {
    constexpr char CHARS_ASCII[] = R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

    if (!font_) PANIC("TTF_OpenFont() failed");

    // 半角文字の幅を得る (等幅フォントを仮定)
    if (TTF_SizeUTF8(font_, "X", &w_, nullptr) != 0)
        PANIC("TTF_SizeUTF8() failed");

    // 文字の高さの最大値を得る。
    // TTF_FontHeight() は最大値より小さい値を返すことがあるため、
    // TTF_SizeUTF8() に全ての ASCII 文字からなる文字列を渡す方法をとる。
    // 非 ASCII 文字については全て調べるのが非現実的なので妥協する。
    if (TTF_SizeUTF8(font_, CHARS_ASCII, nullptr, &h_) != 0)
        PANIC("TTF_SizeUTF8() failed");
}

Font::~Font() {
    TTF_CloseFont(font_);
}

int Font::w() const { return w_; }
int Font::h() const { return h_; }

std::pair<int, int> Font::xy2cr(const int x, const int y) const {
    return { x / w_, y / h_ };
}

std::pair<int, int> Font::cr2xy(const int c, const int r) const {
    return { w_ * c, h_ * r };
}

Surface Font::render(const std::string& text, Color fg, Color bg) const {
    auto* surf = TTF_RenderUTF8_Shaded(font_, text.c_str(), fg.to_sdl_color(), bg.to_sdl_color());
    if (!surf) PANIC("TTF_RenderUTF8_Shaded() failed");
    return Surface(surf);
}
