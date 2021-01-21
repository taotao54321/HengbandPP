#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/preprocessor/cat.hpp>
#include <boost/range/irange.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

// boost::irange() が first > last の場合に壊れる問題を回避するラッパー
template <class Integer>
inline auto IRANGE(const Integer first, const Integer last) {
    return boost::integer_range<Integer>(std::min(first, last), last);
}

// IRANGE(0, last) と等価
template <class Integer>
inline auto IRANGE(const Integer last) {
    return IRANGE(static_cast<Integer>(0), last);
}

// ループカウンタなしで n 回ループ
#define LOOP(n) for ([[maybe_unused]] std::add_const_t<std::remove_cv_t<std::remove_reference_t<decltype(n)>>> BOOST_PP_CAT(macro_loop_counter, __COUNTER__) : IRANGE(n))

// f(begin(c), end(c), ...) と等価
#define ALL(f, c, ...) (([&](decltype((c)) cccc) { using std::begin, std::end; return (f)(begin(cccc), end(cccc), ##__VA_ARGS__); })(c))

// choose minimum
// x < xmin ならば xmin = x と更新する
// 更新があったかどうかを返す
template <class T, class Comp = std::less<>>
inline bool chmin(T& xmin, const T& x, Comp comp = {}) {
    if (comp(x, xmin)) {
        xmin = x;
        return true;
    }
    return false;
}

// choose maximum
// x > xmax ならば xmax = x と更新する
// 更新があったかどうかを返す
template <class T, class Comp = std::less<>>
inline bool chmax(T& xmax, const T& x, Comp comp = {}) {
    if (comp(xmax, x)) {
        xmax = x;
        return true;
    }
    return false;
}

namespace detail {
template <class S, class... Args>
inline std::string FORMAT_IMPL(const S& format_str, Args&&... args) {
    return fmt::format(format_str, std::forward<Args>(args)...);
}

template <class S, class... Args>
inline void WRITE_IMPL(std::ostream& out, const S& format_str, Args&&... args) {
    fmt::print(out, format_str, std::forward<Args>(args)...);
}

template <class S, class... Args>
inline void WRITE_IMPL(std::FILE* const out, const S& format_str, Args&&... args) {
    fmt::print(out, format_str, std::forward<Args>(args)...);
}

template <class S, class... Args>
inline void WRITELN_IMPL(std::ostream& out, const S& format_str, Args&&... args) {
    WRITE_IMPL(out, format_str, std::forward<Args>(args)...);
    fmt::print(out, FMT_STRING("\n"));
}

template <class S, class... Args>
inline void WRITELN_IMPL(std::FILE* const out, const S& format_str, Args&&... args) {
    WRITE_IMPL(out, format_str, std::forward<Args>(args)...);
    fmt::print(out, FMT_STRING("\n"));
}

template <class S, class... Args>
inline void PRINT_IMPL(const S& format_str, Args&&... args) {
    WRITE_IMPL(std::cout, format_str, std::forward<Args>(args)...);
}

template <class S, class... Args>
inline void PRINTLN_IMPL(const S& format_str, Args&&... args) {
    WRITELN_IMPL(std::cout, format_str, std::forward<Args>(args)...);
}

template <class S, class... Args>
inline void EPRINT_IMPL(const S& format_str, Args&&... args) {
    WRITE_IMPL(std::cerr, format_str, std::forward<Args>(args)...);
}

template <class S, class... Args>
inline void EPRINTLN_IMPL(const S& format_str, Args&&... args) {
    WRITELN_IMPL(std::cerr, format_str, std::forward<Args>(args)...);
}
} // namespace detail

// Rust の同名マクロの模造品
#define FORMAT(s, ...) detail::FORMAT_IMPL(FMT_STRING(s), ##__VA_ARGS__)
#define WRITE(out, s, ...) detail::WRITE_IMPL((out), FMT_STRING(s), ##__VA_ARGS__)
#define WRITELN(out, s, ...) detail::WRITELN_IMPL((out), FMT_STRING(s), ##__VA_ARGS__)
#define PRINT(s, ...) detail::PRINT_IMPL(FMT_STRING(s), ##__VA_ARGS__)
#define PRINTLN(s, ...) detail::PRINTLN_IMPL(FMT_STRING(s), ##__VA_ARGS__)
#define EPRINT(s, ...) detail::EPRINT_IMPL(FMT_STRING(s), ##__VA_ARGS__)
#define EPRINTLN(s, ...) detail::EPRINTLN_IMPL(FMT_STRING(s), ##__VA_ARGS__)

namespace detail {
template <class S, class... Args>
[[noreturn]] inline void PANIC_IMPL(const std::string_view file, const int line, const S& format_str, Args&&... args) {
    throw std::logic_error(FORMAT("[{}:{}]: PANIC: {}", file, line, FORMAT_IMPL(format_str, std::forward<Args>(args)...)));
}
} // namespace detail

// Rust の panic! 模造品
// 例外を用いて実装しているので、C と C++ が混在する環境では C 側に例外が漏れないよう注意すること
#define PANIC(s, ...) detail::PANIC_IMPL(__FILE__, __LINE__, FMT_STRING(s), ##__VA_ARGS__)

namespace detail {
template <class T>
inline void DBG_IMPL(const std::string_view file, const int line, const std::string_view expr, T&& value) {
    EPRINTLN("[{}:{}]: {} = {}\n", file, line, expr, std::forward<T>(value));
}

template <class... Ts>
inline void DBG_IMPL(const std::string_view file, const int line, const std::string_view expr, Ts&&... values) {
    EPRINTLN("[{}:{}]: ({}) = {}\n", file, line, expr, std::forward_as_tuple(values...));
}
} // namespace detail

// Rust の dbg! 模造品
#define DBG(...) detail::DBG_IMPL(__FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)
