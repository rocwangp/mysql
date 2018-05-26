#pragma once

#include <cortono/cortono.hpp>
#include <inja.hpp>
#include <nlohmann/json.hpp>

#include "mysql.hpp"
#include "entity.hpp"

std::string render_view(const std::string& filepath, inja::json& data) {
    inja::Environment env = inja::Environment("./www/");
    env.set_element_notation(inja::ElementNotation::Dot);
    inja::Template tmpl = env.parse_template(filepath);
    inja::json tmpl_json_data;
    tmpl_json_data["list"] = data;
    if(data.is_object()) {
        for(auto it = data.begin(); it != data.end(); ++it) {
            tmpl_json_data[it.key()] = it.value();
        }
    }
    return env.render_template(tmpl, tmpl_json_data);
}

std::unordered_map<std::string, std::string> parse_query_params(std::string&& params) {
    std::unordered_map<std::string, std::string> query_kv_pairs;
    std::size_t front{ 0 };
    std::size_t back{ 0 };
    std::string key;
    std::string value;
    while(back != params.length()) {
        if(params[back] == '&') {
            value = params.substr(front, back - front);
            query_kv_pairs.emplace(std::move(key), std::move(value));
            key.clear();
            value.clear();
            front = back + 1;
        }
        else if(key.empty() && params[back] == '=') {
            key = params.substr(front, back - front);
            front = back + 1;
        }
        ++back;
    }
    return query_kv_pairs;
}

template <typename T>
T make_table_object(std::unordered_map<std::string, std::string>&& query_kv_pairs) {
    T t;
    std::unordered_set<std::string_view> null_field_set;
    using table_type = reflection::reflection_t<T>;
    auto field_names = table_type::arr();
    reflection::for_each(t, [&](auto& field, std::size_t idx) {
        if(query_kv_pairs.count(std::string(field_names[idx].data(), field_names[idx].length()))) {
            std::string value = query_kv_pairs[field_names[idx].data()];
            if(value.empty()) {
                null_field_set.emplace(field_names[idx]);
                return;
            }
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (std::is_same_v<field_type, std::string>) {
                field = std::move(value);
            }
            else if constexpr (std::is_same_v<field_type, std::int32_t>) {
                field = std::strtol(value.data(), nullptr, 10);
            }
            else if constexpr (utils::is_cpp_array_v<field_type>) {
                std::memcpy(&field[0], &value[0], value.length());
            }
            else {

            }
        }
        else {
            std::cout << "field " << field_names[idx] << " not provied" << std::endl;
        }
    });
    return t;
}
void handle_register_user(mysql::MySQL& mysql, std::string params) {
    mysql.insert(make_table_object<mysql::user>(parse_query_params(std::move(params))));
}
void handle_create_group(mysql::MySQL& mysql, std::string params) {
    mysql.insert(make_table_object<mysql::user_group>(parse_query_params(std::move(params))));
}
void handle_add_friend(mysql::MySQL& mysql, std::string params) {
    auto query_kv_pairs = parse_query_params(std::move(params));
    mysql.insert(make_table_object<mysql::groups>(std::move(query_kv_pairs)));
}

std::string handle_query_all_friends(mysql::MySQL& mysql, std::unordered_map<std::string, std::string> query_kv_pairs) {
    mysql::rename_map renames{ { "user", { "u1", "u2" } } };
    mysql::query_field_set query_field { "u2.*" };
    std::string user_id = query_kv_pairs["user_id"];
    auto query_results = mysql.query<std::tuple<mysql::user>, mysql::user, mysql::user, mysql::groups, mysql::user_group>(
        "u1.user_id = \"" + user_id + "\"",
        "u1.user_id = groups.user_id",
        "groups.group_id = user_group.group_id",
        "user_group.friend_id = u2.user_id",
        renames,
        query_field
    );

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
    inja::json friend_list;
    for(auto& result : query_results) {
        inja::json data;
        reflection::for_each(std::get<0>(result), [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (utils::is_cpp_array_v<field_type>) {
                data[field_names[idx].data()] = field.data();
            }
            else {
                data[field_names[idx].data()] = field;
            }
        });
        friend_list.push_back(data);
    }

    return render_view("./friend_list.html", friend_list);
}

std::string handle_query_user_with_user_id(mysql::MySQL& mysql, std::unordered_map<std::string, std::string> query_kv_pairs) {
    std::string user_id = query_kv_pairs["user_id"];
    std::string condition = "user_id=\"" + user_id + "\"";
    auto query_results = mysql.query<mysql::user>(condition);

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();

    inja::json user_list;
    for(auto&& result : query_results) {
        inja::json data;
        reflection::for_each(result, [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr(utils::is_cpp_array_v<field_type>) {
                data[field_names[idx].data()] = field.data();
            }
            else {
                data[field_names[idx].data()] = field;
            }
        });
        user_list.push_back(data);
    }
    return render_view("./user_list.html", user_list);
}

std::string handle_query_friend_with_group_name(mysql::MySQL& mysql, std::unordered_map<std::string, std::string> query_kv_pairs) {
    mysql::rename_map renames { { "user", { "u1", "u2" } } };
    mysql::query_field_set query_field { "u2.*" };

    std::string user_id = query_kv_pairs["user_id"];
    std::string group_name = query_kv_pairs["group_name"];

    auto query_results = mysql.query<std::tuple<mysql::user>, mysql::user, mysql::user, mysql::groups, mysql::user_group>(
        "u1.user_id = \"" + user_id + "\"",
        "u1.user_id = groups.user_id",
        "groups.group_id = user_group.group_id",
        "groups.group_name = \"" + group_name + "\"",
        "u2.user_id = user_group.friend_id",
        renames,
        query_field
    );

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
    inja::json friend_list;
    for(auto& result : query_results) {
        inja::json data;
        reflection::for_each(std::get<0>(result), [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (utils::is_cpp_array_v<field_type>) {
                data[field_names[idx].data()] = field.data();
            }
            else {
                data[field_names[idx].data()] = field;
            }
        });
        friend_list.push_back(data);
    }
    return render_view("./friend_list.html", friend_list);
}

std::string handle_query_all_users(mysql::MySQL& mysql) {
    auto query_results = mysql.query<mysql::user>();

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();

    inja::json user_list;
    for(auto& result : query_results) {
        inja::json data;
        reflection::for_each(result, [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            std::string field_name(field_names[idx].data(), field_names[idx].length());
            if constexpr (utils::is_cpp_array_v<field_type>) {
                data[field_name] = std::string(field.data());
            }
            else {
                data[field_name] = field;
            }
        });
        user_list.push_back(data);
    }
    return render_view("./user_list.html", user_list);
}

std::string handle_query_all_groups_with_user_id(mysql::MySQL& mysql,
                                                 std::unordered_map<std::string, std::string> query_kv_pairs) {
    std::string user_id = query_kv_pairs["user_id"];
    mysql::query_field_set query_field{ "group_name" };
    auto query_results = mysql.query<std::tuple<decltype(mysql::groups::group_name)>, mysql::user, mysql::groups>(
        "user.user_id = \"" + user_id + "\"",
        "user.user_id = groups.user_id",
        query_field
    );

    inja::json group_list;
    for(auto& result : query_results) {
        using field_type = std::tuple_element_t<0, std::remove_reference_t<decltype(result)>>;
        if constexpr (utils::is_cpp_array_v<field_type>) {
            group_list.push_back(inja::json{ { "group_name", std::get<0>(result).data() } });
        }
        else {
            group_list.push_back(inja::json{ { "group_name", std::get<0>(result) } });
        }
    }
    return render_view("./group_list.html", group_list);
}

std::string handle_query_common_friends(mysql::MySQL& mysql,
                                        std::unordered_map<std::string, std::string> query_kv_pairs) {
    std::string user_id1 = query_kv_pairs["user_id1"];
    std::string user_id2 = query_kv_pairs["user_id2"];
    mysql::rename_map renames{
        { "user", { "u1", "u2", "u3" } },
        { "groups", { "g1", "g2" } },
        { "user_group", { "ug1", "ug2" } }
    };
    mysql::query_field_set query_field{ "u3.*" };
    auto query_results = mysql.query<
        std::tuple<mysql::user>,
        mysql::user, mysql::groups, mysql::user_group,
        mysql::user, mysql::groups, mysql::user_group,
        mysql::user>
    (
        "u1.user_id = \"" + user_id1 + "\"",
        "u1.user_id = g1.user_id",
        "g1.group_id = ug1.group_id",

        "u2.user_id = \"" + user_id2 + "\"",
        "u2.user_id = g2.user_id",
        "g2.group_id = ug2.group_id",

        "u3.user_id = ug1.friend_id",
        "u3.user_id = ug2.friend_id",
        renames,
        query_field
    );

    // std::string sql("
    // select u3.* from user as u3 where u3.user_id in (
    //     select u2.user_id from user as u2, groups as g2, user_group as ug2
    //     where u2.user_id = ? and u2.user_id = g2.user_id and g2.group_id = ug2.group_id
    //     and ug2.friend_id in (
    //         select u1.user_id from user as u1, groups as g1, user_group as ug1
    //         where u1.user_id = ? and u1.user_id = g1.user_id and g1.group_id = ug1.group_id
    //     )
    // )")

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
    inja::json user_list;
    for(auto& result : query_results) {
        inja::json data;
        reflection::for_each(std::get<0>(result), [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (utils::is_cpp_array_v<field_type>) {
                data[field_names[idx].data()] = field.data();
            }
            else {
                data[field_names[idx].data()] = field;
            }
        });
        user_list.push_back(data);
    }
    return render_view("./user_list.html", user_list);
}
