#include "../utility.hpp"
#include "../mysql.hpp"
#include "entity.hpp"

template <typename... Args>
void filter(Args&&... args) {
    auto t = black_magic::type_filter<mysql::rename_map>::filter(std::forward<Args>(args)...);
    utils::for_each(t, [](auto& item, std::size_t) {
        std::cout << item << std::endl;
    }, std::make_index_sequence<std::tuple_size_v<decltype(t)>>{});
}
int main()
{
    mysql::rename_map renames { { "user", { "u1", "u2" } } };
    mysql::query_field_set query_field { "u2.*" };


    std::string user_id = "user1";
    std::string group_name = "user1_group1";
    filter(
        "u1.user_id = \"" + user_id + "\"",
        "u1.user_id = groups.user_id",
        "groups.group_id = user_group.group_id",
        "groups.group_name = \"" + group_name + "\"",
        "u2.user_id = user_group.friend_id",
        renames
    );

    return 0;
}
