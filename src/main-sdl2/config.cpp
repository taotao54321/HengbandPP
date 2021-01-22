// FIXME:
//   現状 Linux しか考慮していない。
//   設定ファイルのパスは ~/.angband/Hengband/sdl2.json に決め打ちしている。
//
// TODO: もっと楽なシリアライズ機構を導入すべきかも

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iterator>
#include <optional>
#include <string>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "main-sdl2/picojson.h"

#include "main-sdl2/config.hpp"
#include "main-sdl2/prelude.hpp"

namespace {

std::optional<std::string> get_env_var(const std::string& name) {
    const auto* value = std::getenv(name.c_str());
    if (!value) return std::nullopt;
    return std::string(value);
}

bool is_directory(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return false;

    return bool(st.st_mode & S_IFDIR);
}

bool ensure_directory(const std::string& dir) {
    if (is_directory(dir)) return true;

    return mkdir(dir.c_str(), 0755) == 0;
}

std::optional<std::string> get_config_directory() {
    const auto home = get_env_var("HOME");
    if (!home) return std::nullopt;

    return FORMAT("{}/.angband/Hengband", *home);
}

std::optional<std::string> get_config_path() {
    const auto dir = get_config_directory();
    if (!dir) return std::nullopt;

    return FORMAT("{}/sdl2.json", *dir);
}

std::optional<std::string> get_ensured_config_path() {
    const auto dir = get_config_directory();
    if (!dir) return std::nullopt;
    if (!ensure_directory(*dir)) return std::nullopt;

    return FORMAT("{}/sdl2.json", *dir);
}

template <class T>
std::optional<std::reference_wrapper<const T>> json_get(const picojson::value& v) {
    if (!v.is<T>()) return std::nullopt;
    return v.get<T>();
}

#if 0
// 今のところ未使用
std::optional<int> json_get_int(const picojson::value& v) {
    const auto real = json_get<double>(v);
    if (!real) return std::nullopt;
    return int(*real);
}
#endif

template <class T>
std::optional<std::reference_wrapper<const T>> json_object_get(const picojson::object& o, const std::string& key) {
    using std::end;
    const auto it = o.find(key);
    return it == end(o) ? std::nullopt : json_get<T>(it->second);
}

std::optional<int> json_object_get_int(const picojson::object& o, const std::string& key) {
    const auto real = json_object_get<double>(o, key);
    if (!real) return std::nullopt;
    return int(*real);
}

picojson::value json_value_int(const int x) {
    return picojson::value(double(x));
}

} // anonymous namespace

Config::Config()
    : win_descs() { }

std::optional<Config> Config::load() {
    using std::size;

    const auto path = get_config_path();
    if (!path) return std::nullopt;

    std::ifstream in(*path, std::ios::in);
    if (!in) return std::nullopt;

    picojson::value json_v;
    in >> json_v;
    if (!picojson::get_last_error().empty()) return std::nullopt;
    in.close();

    const auto json = json_get<picojson::object>(json_v);
    if (!json) return std::nullopt;

    // 以下、存在しない値や変な値は単に無視する
    Config config;

    if (const auto windows = json_object_get<picojson::array>(*json, "windows")) {
        for (const int i : IRANGE(std::min(size(windows->get()), size(config.win_descs)))) {
            const auto window = json_get<picojson::object>(windows->get()[i]);
            if (!window) continue;

            auto& desc = config.win_descs[i];

            if (const auto title = json_object_get<std::string>(*window, "title"))
                desc.title(*title);
            if (const auto x = json_object_get_int(*window, "x"))
                desc.x(*x);
            if (const auto y = json_object_get_int(*window, "y"))
                desc.y(*y);
            if (const auto w = json_object_get_int(*window, "w"))
                desc.w(*w);
            if (const auto h = json_object_get_int(*window, "h"))
                desc.h(*h);
            if (const auto font_path = json_object_get<std::string>(*window, "font_path"))
                desc.font_path(*font_path);
            if (const auto font_pt = json_object_get_int(*window, "font_pt"))
                desc.font_pt(*font_pt);
            if (const auto visible = json_object_get<bool>(*window, "visible"))
                desc.visible(*visible);
        }
    }

    return config;
}

bool Config::save() const {
    picojson::object json;

    picojson::array windows;
    for (const auto& desc : win_descs) {
        picojson::object window;
        window.emplace("title", desc.title());
        window.emplace("x", json_value_int(desc.x()));
        window.emplace("y", json_value_int(desc.y()));
        window.emplace("w", json_value_int(desc.w()));
        window.emplace("h", json_value_int(desc.h()));
        window.emplace("font_path", desc.font_path());
        window.emplace("font_pt", json_value_int(desc.font_pt()));
        window.emplace("visible", desc.visible());

        windows.emplace_back(picojson::value(window));
    }
    json.emplace("windows", windows);

    const auto path = get_ensured_config_path();
    if (!path) return false;

    std::ofstream out(*path, std::ios::out);
    if (!out) return false;

    picojson::value(json).serialize(std::ostream_iterator<char>(out), /* prettify= */ true);

    return true;
}
