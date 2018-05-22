#include <iostream>
#include <vector>
#include <string>
#include <tuple>

#include "../reflection.hpp"


struct user
{
    int id;
    std::string name;
};

REFLECTION(user, id, name);


template <typename Tuple, typename Func, std::size_t... Idx>
void for_each(Tuple&& t, Func&& f, std::index_sequence<Idx...>) {
    (std::forward<Func>(f)(std::get<Idx>(t)), ...);
}
int main()
{
    using reflection_type = decltype(members_reflection(std::declval<user>()));
    std::cout << reflection_type::name() << std::endl;

    for(auto& member_name : reflection_type::arr()) {
        std::cout << member_name << " ";
    }
    std::cout << "\n";

    user u{ 2, "hello world" };
    for_each(reflection_type::apply_impl(), [&u](auto& item) {
        std::cout << u.*item << " ";
    }, std::make_index_sequence<reflection_type::count()>{});
    std::cout << std::endl;

    return 0;
}
