#pragma once

#include "std.hpp"

namespace black_magic
{
    template <typename T>
    struct function_traits;

    template <typename ClassType, typename R, typename... Args>
    struct function_traits<R(ClassType::*)(Args...)>
    {
        using return_type = R;
        static constexpr std::size_t param_count = sizeof...(Args);
        static constexpr std::size_t traits_number = 0;

        template <std::size_t N>
        using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template <typename ClassType, typename R, typename... Args>
    struct function_traits<R(ClassType::*)(Args...) const>
    {
        using return_type = R;
        static constexpr std::size_t param_count = sizeof...(Args);
        static constexpr std::size_t traits_number = 1;

        template <std::size_t N>
        using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template <typename R, typename... Args>
    struct function_traits<R(Args...)>
    {
        using return_type = R;
        static constexpr std::size_t param_count = sizeof...(Args);
        static constexpr std::size_t traits_number = 2;

        template <std::size_t N>
        using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
    };

    template <typename R, typename... Args>
    struct function_traits<R(Args...) const>
    {
        using return_type = R;
        static constexpr std::size_t param_count = sizeof...(Args);
        static constexpr std::size_t traits_number = 3;

        template <std::size_t N>
        using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
    };


    template <typename T>
    struct function_traits : public function_traits<decltype(&T::operator())>
    {
    };


    template <typename T, typename... Args>
    struct contains : public std::false_type {};

    template <typename T, typename U, typename... Args>
    struct contains<T, U, Args...> : public std::conditional_t<std::is_same_v<T, U>, std::true_type, contains<T, Args...>> {};

    template <typename... FilterArgs>
    struct type_filter
    {
        static constexpr auto filter() {
            return std::tuple{};
        }

        template <typename T, typename... Args>
        static constexpr auto filter(T&& t, Args&&... args) {
            if constexpr (contains<std::remove_reference_t<T>, FilterArgs...>::value) {
                return filter(std::forward<Args>(args)...);
            }
            else {
                return std::tuple_cat(std::make_tuple(std::forward<T>(t)), filter(std::forward<Args>(args)...));
            }
        }
    };
}

namespace utils
{
    template <typename... Args>
    void string_append(std::string& str, Args... args) {
        if constexpr (sizeof...(Args) == 0) {
            return;
        }
        std::tuple<Args...> t = std::make_tuple(args...);
        for_each(t, [&str](auto& item, std::size_t) {
            using item_type = std::remove_reference_t<decltype(item)>;
            if constexpr (std::is_same_v<item_type, std::string>) {
                str.append(item);
            }
            else if constexpr (std::is_same_v<item_type, std::string_view>) {
                str.append(std::string{ item.data(), item.length() });
            }
            else if constexpr (std::is_pointer_v<item_type>) {
                str.append(item);
            }
            else {
                str.append(std::to_string(item));
            }
        }, std::make_index_sequence<sizeof...(Args)>{});
    }

    template <typename Tuple, typename Func, std::size_t... Idx>
    void for_each(Tuple&& t, Func&& f, std::index_sequence<Idx...>) {
        (std::forward<Func>(f)(std::get<Idx>(t), Idx), ...);
    }

    static inline auto string_to_time_point(const std::string& time_str) {
        std::istringstream ss(time_str);
        std::tm tm_time;
        ss >> std::get_time(&tm_time, "%Y-%m-%d %H:%M:%S");
        return std::chrono::system_clock::from_time_t(std::mktime(&tm_time));
    }

    static inline auto time_point_to_string(const std::chrono::system_clock::time_point& tp) {
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::ostringstream ss;
        ss << std::put_time(std::localtime(&tt), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    template <typename T>
    struct is_cpp_array : std::false_type {};

    template <typename T, std::size_t N>
    struct is_cpp_array<std::array<T, N>> : std::true_type {};

    template <typename T>
    struct is_tuple : std::false_type {};

    template <typename... Args>
    struct is_tuple<std::tuple<Args...>> : std::true_type {};

    template <typename T>
    static constexpr decltype(std::true_type::value) is_cpp_array_v = is_cpp_array<T>::value;

    template <typename T>
    static constexpr decltype(std::true_type::value) is_tuple_v = is_tuple<T>::value;

    template <typename T>
    static constexpr decltype(std::true_type::value) is_system_time_point_v = std::is_same_v<T, std::chrono::system_clock::time_point>;
}
