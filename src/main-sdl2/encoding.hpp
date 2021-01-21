#pragma once

#include <cassert>
#include <string>

#include "main-sdl2/prelude.hpp"

std::string euc_to_utf8(const std::string& euc);

#if 0
// EUC-JP を1文字読み進め、その文字を返す。
// 2~3 バイト文字についてはリトルエンディアンで返す。
//
// 呼び出し後、it は次の文字を指す。
// it == last で呼び出してはならない。
template <class InputIt>
u32 euc_next(InputIt& it, InputIt last) {
    assert(it != last);

    const u8 b1 = *it++;

    if (b1 <= 0x7F) return b1;
}
#endif
