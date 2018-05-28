#pragma once

#include <cortono/cortono.hpp>
#include <inja.hpp>
#include <nlohmann/json.hpp>

#include "mysql.hpp"
#include "entity.hpp"

using query_kv_map = std::unordered_map<std::string, std::string>;

std::string render_view(const std::string& filepath, std::unordered_map<std::string, inja::json> m) {
    inja::Environment env = inja::Environment("./www/");
    env.set_element_notation(inja::ElementNotation::Dot);
    inja::Template tmpl = env.parse_template(filepath);
    inja::json tmpl_json_data;
    for(auto& [key, value] : m) {
        tmpl_json_data[key] = value;
    }
    for(auto& [key, data] : m) {
        if(data.is_object()) {
            for(auto it = data.begin(); it != data.end(); ++it) {
                tmpl_json_data[it.key()] = it.value();
            }
        }
    }
    return env.render_template(tmpl, tmpl_json_data);
}

std::unordered_map<std::string, std::string> parse_query_params(const std::string& params) {
    std::unordered_map<std::string, std::string> query_kv_pairs;
    std::size_t front{ 0 };
    std::size_t back{ 0 };
    std::string key;
    std::string value;
    while(back <= params.length()) {
        if(back == params.length() || params[back] == '&') {
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
auto make_table_object(std::unordered_map<std::string, std::string> query_kv_pairs) {
    T t;
    mysql::null_field_set null_field;
    using table_type = reflection::reflection_t<T>;
    auto field_names = table_type::arr();
    reflection::for_each(t, [&](auto& field, std::size_t idx) {
        if(query_kv_pairs.count(std::string(field_names[idx].data(), field_names[idx].length()))) {
            std::string value = query_kv_pairs[field_names[idx].data()];
            if(value.empty()) {
                null_field.emplace(field_names[idx]);
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
                std::memset(&field[0], 0, field.size());
                std::memcpy(&field[0], &value[0], value.length());
            }
            else {

            }
        }
        else {
            null_field.emplace(field_names[idx]);
        }
    });
    return std::make_pair(t, null_field);
}
void handle_create_group(mysql::MySQL& mysql, std::string params) {
    try {
        auto [t, null_field] = make_table_object<mysql::groups>(parse_query_params(std::move(params)));
        mysql.insert(std::move(t), std::move(null_field));
    }
    catch(sql::SQLException& e) {
        log_info(e.what());
    }
}
void handle_add_friend(mysql::MySQL& mysql, std::unordered_map<std::string, std::string> query_kv_pairs) {
    try {
        auto [t, null_field] = make_table_object<mysql::groups>(query_kv_pairs);
        mysql.insert(std::move(t), std::move(null_field));
    }
    catch(sql::SQLException& e) {
        log_info(e.what());
    }
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
    return friend_list.dump();
}

std::string handle_query_user_with_user_id(mysql::MySQL& mysql, std::unordered_map<std::string, std::string> query_kv_pairs) {
/* std::string handle_query_user_with_user_id(mysql::MySQL& mysql, std::string params) { */
    /* auto query_kv_pairs = parse_query_params(std::move(params)); */
    std::string user_id = query_kv_pairs["user_id"];
    std::string condition = "user_id=\"" + user_id + "\"";
    auto query_results = mysql.query<mysql::user>(condition);

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();

    inja::json data;
    if(query_results.empty()) {
        return data.dump();
    }
    reflection::for_each(query_results.front(), [&](auto& field, std::size_t idx) {
        using field_type = std::remove_reference_t<decltype(field)>;
        if constexpr(utils::is_cpp_array_v<field_type>) {
            data[field_names[idx].data()] = field.data();
        }
        else {
            data[field_names[idx].data()] = field;
        }
    });
    return data.dump();
    /* std::unordered_map<std::string, inja::json> m{ { "list", user_list } }; */
    /* return render_view("./user_list.html", user_list); */
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
    std::unordered_map<std::string, inja::json> m{ { "list", friend_list } };
    return render_view("./friend_list.html", m);
}

std::string handle_query_all_users(mysql::MySQL& mysql) {

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
    auto query_results = mysql.query<mysql::user>();

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


    return user_list.dump();
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
    std::unordered_map<std::string, inja::json> m{ { "list", group_list } };
    return render_view("./group_list.html", m);
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

    /* mysql.query_execute<mysql::user>( */
    /*     "SELECT u3.* \ */
    /*      FROM user as u3 \ */
    /*      WHERE u3.user_id in \ */
    /*      (\ */
    /*         SELECT u1.user_id \ */
    /*         FROM \ */
    /*             user as u1, groups as g1, user_group as ug1, \ */
    /*             user as u2, groups as g2, user_group as ug2 \ */
    /*         WHERE \ */
    /*             u1.user_id = ? \ */
    /*             u1.user_id = g1.user_id AND \ */
    /*             g1.group_id = ug1.group_id AND \ */
    /*             u2.user_id = ? \ */
    /*             u2.user_id = g2.user_id AND \ */
    /*             g2.group_id = ug2.group_di AND \ */
    /*             ug1.friend_id = ug2.friend_id \ */
    /*      )" */
    /* ); */

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
    std::unordered_map<std::string, inja::json> m{ { "list", user_list } };
    return render_view("./user_list.html", m);
}

std::pair<bool, std::string> handle_user_login(mysql::MySQL& mysql, std::unordered_map<std::string, std::string>& query_kv_pairs) {
    auto query_results = mysql.query<mysql::user>(
        "user_id = \"" + query_kv_pairs["user_id"] + "\"",
        "password = \"" + query_kv_pairs["password"] + "\""
    );
    inja::json user;
    if(query_results.empty()) {
        log_info("login error");
        return { false, user.dump() };
    }
    else {
        constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
        reflection::for_each(query_results.front(), [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            log_info(field_names[idx].data());
            if constexpr (utils::is_cpp_array_v<field_type>) {
                user[field_names[idx].data()] = field.data();
            }
            else {
                user[field_names[idx].data()] = field;
            }
        });
        std::string s = user.dump();
        log_info(s);
        return { true, s };
    }
}
std::pair<bool, std::string> handle_user_login(mysql::MySQL& mysql, std::string params) {
    log_info(params);
    auto query_kv_pairs = parse_query_params(std::move(params));
    return handle_user_login(mysql, query_kv_pairs);
}

template <std::size_t N>
void string_to_array(std::string& str, std::array<char, N>& arr) {
    std::memcpy(&arr[0], &str[0], str.length());
}
std::string handle_modify_user(mysql::MySQL& mysql, std::string params) {
    auto m = parse_query_params(std::move(params));
    mysql::user user;
    inja::json data;

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
    reflection::for_each(user, [&](auto& field, std::size_t idx) {
        string_to_array(m[field_names[idx].data()], field);
        data[field_names[idx].data()] = m[m[field_names[idx].data()]];
    });

    mysql.update(std::move(user));
    return data.dump();
}

std::string handle_remove_user_with_user_id(mysql::MySQL& mysql, std::string params) {
    auto m = parse_query_params(std::move(params));
    auto user_id = m["user_id"];
    mysql.remove<mysql::user>( "user_id=\"" + m["user_id"] + "\"");
    mysql.remove<mysql::edu_experience>( "user_id=\"" + m["user_id"] + "\"" );
    mysql.remove<mysql::work_experience>( "user_id=\"" + m["user_id"] + "\"" );
    mysql.execute(
    "\
        DELETE FROM user_group \
        WHERE group_id in ( \
            SELECT group_id FROM groups WHERE user_id=\"" + user_id + "\" \
        )\
    ");
    mysql.remove<mysql::groups>( "user_id=\"" + m["user_id"] + "\"" );
    inja::json data;
    return data.dump();
}

std::string handle_register_user(mysql::MySQL& mysql, std::string params) {
    auto m = parse_query_params(std::move(params));
    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
    mysql::user user;
    mysql::null_field_set null_field;
    inja::json data;
    reflection::for_each(user, [&](auto& field, std::size_t idx) {
        if(m[field_names[idx].data()].empty()) {
            log_info(idx);
            null_field.emplace(field_names[idx]);
        }
        string_to_array(m[field_names[idx].data()], field);
        data[field_names[idx].data()] = m[field_names[idx].data()];
    });
    mysql.insert(std::move(user), std::move(null_field));

    std::string s = data.dump();
    log_info(s);
    return s;
}

std::string handle_query_friend_request(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    log_info("query_friend_request");
    auto query_results = mysql.query<mysql::friend_request>(
        "recver_id = \"" + query_kv_pairs["user_id"] + "\"",
        "pending = 1"
    );
    constexpr auto field_names = reflection::reflection_t<mysql::friend_request>::arr();
    inja::json request_list;
    for(auto& result : query_results) {
        inja::json data;
        reflection::for_each(result, [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            log_info(field_names[idx].data());
            if constexpr (utils::is_cpp_array_v<field_type>) {
                data[field_names[idx].data()] = field.data();
            }
            else {
                data[field_names[idx].data()] = field;
            }
        });
        request_list.push_back(data);
    }
    std::string s = request_list.dump();
    log_info(s);
    return s;
}


std::string handle_request_friend(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    inja::json result;
    result["result"] = "ok";
    return result.dump();
}

std::string handle_send_friend_request(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    query_kv_pairs["sender_id"] = query_kv_pairs["user_id"];
    query_kv_pairs["recver_id"] = query_kv_pairs["friend_id"];
    query_kv_pairs["sender_group"] = query_kv_pairs["group_name"];
    query_kv_pairs["pending"] = "1";
    auto [t, null_field] = make_table_object<mysql::friend_request>(query_kv_pairs);
    mysql.insert(std::move(t), std::move(null_field));
    inja::json result;
    result["result"] = "send done";
    return result.dump();
}

std::string handle_agree_friend_request(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql.begin();
    mysql.execute(
    "\
    INSERT INTO user_group VALUES (\
        (\
            SELECT DISTINCT group_id FROM user, groups WHERE \
            user.user_id = groups.user_id AND \
            user.user_id = \"" + query_kv_pairs["sender_id"] + "\" AND \
            group_name = \"" + query_kv_pairs["sender_group"] + "\" \
        ),\
        \"" + query_kv_pairs["recver_id"] + "\" \
    )\
    ");

    mysql.execute(
    "\
    INSERT INTO user_group VALUES (\
        (\
            SELECT DISTINCT group_id FROM user, groups WHERE \
            user.user_id = groups.user_id AND \
            user.user_id = \"" + query_kv_pairs["recver_id"] + "\" AND \
            group_name = \"" + query_kv_pairs["recver_group"] + "\" \
        ),\
        \"" + query_kv_pairs["sender_id"] + "\" \
    )\
    ");

    mysql.remove<mysql::friend_request>( "request_id = \"" + query_kv_pairs["request_id"] + "\"");

    mysql.commit();

    inja::json result;
    result["result"] = "send done";
    return result.dump();
}

std::string handle_add_group(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    auto [t, null_field] = make_table_object<mysql::groups>(std::move(query_kv_pairs));
    mysql.insert(std::move(t), std::move(null_field));
    inja::json result;
    result["result"] = "done";
    return result.dump();
}


std::string handle_send_message(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    query_kv_pairs["sender_id"] = query_kv_pairs["user_id"];
    query_kv_pairs["recver_id"] = query_kv_pairs["friend_id"];
    auto [t, null_field] = make_table_object<mysql::chat_message>(std::move(query_kv_pairs));
    mysql.insert(std::move(t), std::move(null_field));
    inja::json result;
    result["result"] = "send done";
    return result.dump();
}

std::string handle_query_chat_message(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    auto query_result = mysql.query<mysql::chat_message>( "recver_id = \"" + query_kv_pairs["user_id"] + "\"");

    constexpr auto field_names = reflection::reflection_t<mysql::chat_message>::arr();
    inja::json message;
    for(auto& result : query_result) {
        inja::json data;
        reflection::for_each(result, [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            log_info(field_names[idx].data());
            if constexpr (utils::is_cpp_array_v<field_type>) {
                data[field_names[idx].data()] = field.data();
            }
            else {
                data[field_names[idx].data()] = field;
            }
        });
        message.push_back(data);
    }
    return message.dump();
}
