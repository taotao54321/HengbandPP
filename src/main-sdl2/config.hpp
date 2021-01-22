#pragma once

#include <array>
#include <optional>

#include "main-sdl2/game-window.hpp"

struct Config {
    std::array<GameWindowDesc, 8> win_descs;

    // デフォルト設定を得る。
    Config();

    // 規定の方法で設定をロードする。失敗すると std::nullopt を返す。
    static std::optional<Config> load();

    // 規定の方法で設定を保存する。失敗すると false を返す。
    [[nodiscard]] bool save() const;
};
