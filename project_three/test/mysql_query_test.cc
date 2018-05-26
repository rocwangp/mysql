#include "../mysql.hpp"
#include "entity.hpp"

void single_query_with_condition(mysql::MySQL& mysql) {
    auto query_results = mysql.query<mysql::user>( "user_id = \"user1\"");
    for(auto& result : query_results) {
        std::stringstream oss;
        reflection::for_each(result, [&](auto& field, std::size_t) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (utils::is_cpp_array_v<field_type>) {
                oss << (std::string(field.data()));
            }
            else {
                oss << field;
            }
            oss << "\t";
        });
        log_info(oss.str());
    }
}

void single_query_with_field_names(mysql::MySQL& mysql) {
    auto query_results = mysql.query<std::tuple<std::string>, mysql::user>(
        "user_id = \"user1\"",
        mysql::query_field_set{ "password" }
    );
    for(auto& results : query_results) {
        std::stringstream oss;
        utils::for_each(results, [&](auto& field, std::size_t) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (utils::is_cpp_array_v<field_type>) {
                oss << (std::string(field.data()));
            }
            else {
                oss << field;
            }
            oss << "\t";
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(results)>>>{});
        log_info(oss.str());
    }
}

void multi_query_with_condition(mysql::MySQL& mysql) {
    mysql::rename_map renames { { "user", { "u1", "u2" } } };
    mysql::query_field_set query_field { "u2.*" };
    auto query_results = mysql.query<std::tuple<mysql::user>, mysql::user, mysql::user, mysql::groups, mysql::user_group>(
        "u1.user_id = \"user1\"",
        "u1.user_id = groups.user_id",
        "groups.group_id = user_group.group_id",
        "user_group.friend_id = u2.user_id",
        renames,
        query_field
    );
    for(auto& result : query_results) {
        std::stringstream oss;
        utils::for_each(result, [&](auto& item, std::size_t) {
            using item_type = std::remove_reference_t<decltype(item)>;
            if constexpr (reflection::is_reflection<item_type>::value) {
                reflection::for_each(item, [&](auto& field, std::size_t) {
                    using field_type = std::remove_reference_t<decltype(field)>;
                    if constexpr (utils::is_cpp_array_v<field_type>) {
                        oss << (std::string(field.data()));
                    }
                    else {
                        oss << field;
                    }
                    oss << "\t";
                });
            }
            else {
                if constexpr (utils::is_cpp_array_v<item_type>) {
                    oss << (std::string(item.data()));
                }
                else {
                    oss << item;
                }
                oss << "\t";
            }
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(result)>>>{});
        log_info(oss.str());
    }
}
int main()
{
    mysql::MySQL mysql("tcp://127.0.0.1", "root", "3764819", "social");

    mysql::primary_key keys1{ { "user_id" } };
    mysql::not_null not_nulls1{ { "user_id", "password", "nickname" } };
    mysql.bind_attribute<mysql::user>(keys1, not_nulls1);

    mysql::not_null not_nulls2{ { "user_id" } };
    mysql.bind_attribute<mysql::edu_experience>( not_nulls2);

    mysql::not_null not_nulls3{ { "user_id" } };
    mysql.bind_attribute<mysql::work_experience>(not_nulls3);

    mysql::primary_key keys4{ { "group_id", "friend_id" } };
    mysql.bind_attribute<mysql::user_group>(keys4);

    mysql::primary_key keys5{ { "group_id", "user_id" } };
    mysql::not_null not_nulls5{ { "user_id", "group_name" } };
    mysql::auto_increment auto_incs5{ { "group_id" } };
    mysql.bind_attribute<mysql::groups>(keys5, not_nulls5, auto_incs5);

    mysql::primary_key keys6{ { "log_id" } };
    mysql::not_null not_nulls6{ { "user_id", "title", "publish_date", "modify_date" } };
    mysql.bind_attribute<mysql::log>(keys6, not_nulls6);

    mysql::not_null not_nulls7{ { "sender_id", "recver_id", "publish_date" } };
    mysql.bind_attribute<mysql::chat_message>(not_nulls7);

    mysql::not_null not_nulls9{ { "sender_id", "log_id", "publish_date" } };
    mysql.bind_attribute<mysql::log_message>(not_nulls9);

    mysql::not_null not_nulls10{ { "sharer_id", "log_id", "share_date" } };
    mysql.bind_attribute<mysql::share_log>(not_nulls10);

    single_query_with_condition(mysql);
    single_query_with_field_names(mysql);
    multi_query_with_condition(mysql);

    return 0;
}
