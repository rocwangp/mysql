#pragma once
#include "std.hpp"
#include "utility.hpp"
namespace reflection
{

#define MAKE_STRING_VIEW(element) std::string_view(#element, sizeof(#element) - 1)

#define SEPERATOR ,
#define CON_STR_1(element, ...) MAKE_STRING_VIEW(element)
#define CON_STR_2(element, ...) MAKE_STRING_VIEW(element) SEPERATOR CON_STR_1(__VA_ARGS__)
#define CON_STR_3(element, ...) MAKE_STRING_VIEW(element) SEPERATOR CON_STR_2(__VA_ARGS__)
#define CON_STR_4(element, ...) MAKE_STRING_VIEW(element) SEPERATOR CON_STR_3(__VA_ARGS__)
#define CON_STR_5(element, ...) MAKE_STRING_VIEW(element) SEPERATOR CON_STR_4(__VA_ARGS__)
#define CON_STR_6(element, ...) MAKE_STRING_VIEW(element) SEPERATOR CON_STR_5(__VA_ARGS__)
#define CON_STR_7(element, ...) MAKE_STRING_VIEW(element) SEPERATOR CON_STR_6(__VA_ARGS__)

#define MAKE_ARG_LIST_1(op, arg, ...) op(arg)
#define MAKE_ARG_LIST_2(op, arg, ...) op(arg) SEPERATOR MAKE_ARG_LIST_1(op, __VA_ARGS__)
#define MAKE_ARG_LIST_3(op, arg, ...) op(arg) SEPERATOR MAKE_ARG_LIST_2(op, __VA_ARGS__)
#define MAKE_ARG_LIST_4(op, arg, ...) op(arg) SEPERATOR MAKE_ARG_LIST_3(op, __VA_ARGS__)
#define MAKE_ARG_LIST_5(op, arg, ...) op(arg) SEPERATOR MAKE_ARG_LIST_4(op, __VA_ARGS__)
#define MAKE_ARG_LIST_6(op, arg, ...) op(arg) SEPERATOR MAKE_ARG_LIST_5(op, __VA_ARGS__)
#define MAKE_ARG_LIST_7(op, arg, ...) op(arg) SEPERATOR MAKE_ARG_LIST_6(op, __VA_ARGS__)

#define MARCO_CONCAT(A, B) A##_##B

#define ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define RSEQ_N() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define GET_ARG_COUNT_INNER(...) ARG_N(__VA_ARGS__)
#define GET_ARG_COUNT(...) GET_ARG_COUNT_INNER(__VA_ARGS__, RSEQ_N())

// __VA_ARGS__是 STRUCT_NAME::*类型
#define MAKE_META_DATA_IMPL(STRUCT_NAME, ...) \
    static auto members_reflection(const STRUCT_NAME&) \
    {\
        struct members_reflection_helper\
        {\
            static constexpr decltype(auto) apply_impl() {\
                return std::make_tuple(__VA_ARGS__);\
            }\
            using type = void;\
            static constexpr std::string_view name() { return std::string_view(#STRUCT_NAME, sizeof(#STRUCT_NAME) - 1); }\
            static constexpr std::size_t count() { return GET_ARG_COUNT(__VA_ARGS__); }\
            static constexpr std::array<std::string_view, GET_ARG_COUNT(__VA_ARGS__)> arr() { return arr_##STRUCT_NAME; } \
        };\
        return members_reflection_helper{};\
    }

#define FIELD(t) t

#define MAKE_META_DATA(STRUCT_NAME, N, ...) \
    static constexpr inline std::array<std::string_view, N> arr_##STRUCT_NAME = { MARCO_CONCAT(CON_STR, N)(__VA_ARGS__) }; \
    MAKE_META_DATA_IMPL(STRUCT_NAME, MARCO_CONCAT(MAKE_ARG_LIST, N)(&STRUCT_NAME::FIELD, __VA_ARGS__))

#define REFLECTION(STRUCT_NAME, ...) \
    MAKE_META_DATA(STRUCT_NAME, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

    template <typename T>
    using reflection_t = decltype(members_reflection(std::declval<std::remove_reference_t<T>>()));

    template <typename T, typename = void>
    struct is_reflection : std::false_type {};

    template <typename T>
    struct is_reflection<T, std::void_t<typename reflection_t<T>::type>> : std::true_type {};

    namespace detail
    {
        template <typename T, typename Func, std::size_t... Idx>
        constexpr void for_each(T&& t, Func&& f, std::index_sequence<Idx...>) {
            if constexpr (is_reflection<T>::value) {
                constexpr auto tp = reflection_t<T>::apply_impl();
                (std::forward<Func>(f)(t.*std::get<Idx>(tp), Idx), ...);
            }
            else {
                (std::forward<Func>(f)(std::get<Idx>(t), Idx), ...);
            }
        }

    }

    template <typename T, typename Func>
    constexpr std::enable_if<is_reflection<T>::value> for_each(T&& t, Func&& f) {
        detail::for_each(std::forward<T>(t), std::forward<Func>(f), std::make_index_sequence<reflection_t<T>::count()>{});
    }

    template <typename... Args, typename Func>
    constexpr void for_each(std::tuple<Args...>&& t, Func&& f) {
        detail::for_each(std::forward<std::tuple<Args...>>(t), std::forward<Func>(f), std::make_index_sequence<sizeof...(Args)>{});
    }
}
