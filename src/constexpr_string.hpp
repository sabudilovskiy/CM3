#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

template <std::size_t Size = 1>
struct ConstexprString {
    static constexpr auto kSize = Size;
    std::array<char, Size> contents{};

    constexpr ConstexprString() noexcept = default;

    constexpr ConstexprString(const char (&str)[Size]) noexcept {
        std::copy_n(str, Size, begin(contents));
    }

    constexpr operator std::string_view() const { return AsStringView(); }
    constexpr const char& operator[](size_t index) const {
        return contents[index];
    }

    constexpr char& operator[](size_t index) { return contents[index]; }

    constexpr std::string_view AsStringView() const {
        return std::string_view{contents.data(), Size - 1};
    }

    std::string AsString() const { return std::string{AsStringView()}; }

    constexpr const char* data() const { return contents.begin(); }

    constexpr bool empty() const { return Size == 1; }

    constexpr auto size() const { return Size; }

    constexpr size_t FindFirstChar(char ch) const {
        return AsStringView().find_first_of(ch);
    }
    template <std::size_t begin, std::size_t N>
    constexpr static std::size_t real_n() {
        if constexpr (N == std::string_view ::npos) {
            static_assert(Size > begin);
            return Size - begin;
        } else {
            static_assert(N <= Size);
            return N;
        }
    }
    template <std::size_t begin, std::size_t N>
    constexpr auto substr_impl() const {
        static_assert(begin + N <= Size);
        ConstexprString<N> result;
        for (size_t i = begin; i < begin + N; i++) {
            result[i - begin] = contents[i];
        }
        return result;
    }
    template <std::size_t begin, std::size_t N = std::string_view::npos>
    constexpr auto substr() const {
        return substr_impl<begin, real_n<begin, N>()>();
    }

    constexpr auto c_str() const -> const char* { return contents.data(); }
    friend constexpr bool operator==(const ConstexprString& string,
                                     const char* rhs) {
        std::string_view right{rhs};
        return string.AsStringView() == right;
    }
    friend constexpr bool operator==(const char* lhs,
                                     const ConstexprString& string) {
        return string == lhs;
    }
};
