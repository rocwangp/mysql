#pragma once

#include "std.hpp"

namespace mysql
{
    template <typename >
    struct identity {};

    static constexpr inline auto type_to_name(identity<std::int8_t>) { return "TINYINT"sv; }
    static constexpr inline auto type_to_name(identity<std::int16_t>) { return "SMALLINT"sv; }
    static constexpr inline auto type_to_name(identity<std::int32_t>) { return "INTEGER"sv; }
    static constexpr inline auto type_to_name(identity<float>) { return "FLOAT"sv; }
    static constexpr inline auto type_to_name(identity<double>) { return "DOUBLE"sv; }
    static constexpr inline auto type_to_name(identity<std::int64_t>) { return "BIGINT"sv; }
    static constexpr inline auto type_to_name(identity<std::uint32_t>) { return "INT UNSIGNED"sv; }
    static constexpr inline auto type_to_name(identity<std::string>) { return "TEXT"sv; }
    static constexpr inline auto type_to_name(identity<std::chrono::system_clock::time_point>) { return "DATETIME"sv; }
    template <std::size_t N>
    static constexpr inline auto type_to_name(identity<std::array<char, N>>) {
        return std::string("VARCHAR(" + std::to_string(N) + ")");
    }

}
