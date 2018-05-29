#pragma once

#include <cortono/cortono.hpp>
#include <inja.hpp>
#include <nlohmann/json.hpp>

#include "mysql.hpp"
#include "entity.hpp"

using query_kv_map = std::unordered_map<std::string, std::string>;

template <std::size_t N>
void string_to_array(std::string& str, std::array<char, N>& arr) {
    std::memcpy(&arr[0], &str[0], str.length());
}
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

// 视图查询（嵌套查询）
std::string handle_query_all_friends(mysql::MySQL& mysql, std::unordered_map<std::string, std::string> query_kv_pairs) {
    auto query_results = mysql.query<mysql::user_friends>("user = \"" + query_kv_pairs["user_id"] + "\"");
    constexpr auto field_names = reflection::reflection_t<mysql::user_friends>::arr();
    inja::json friend_list;
    for(auto& result : query_results) {
        inja::json data;
        reflection::for_each(result, [&](auto& field, std::size_t idx) {
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
    std::string s = friend_list.dump();
    log_info(s);
    return s;
}

// 视图查询（嵌套查询）
std::string handle_query_friends_in_group(mysql::MySQL& mysql, std::unordered_map<std::string, std::string> query_kv_pairs) {
    auto query_results = mysql.query<mysql::friends_in_every_group>( "group_id = \"" + query_kv_pairs["group_id"] + "\"");
    constexpr auto field_names = reflection::reflection_t<mysql::friends_in_every_group>::arr();
    inja::json friend_list;
    for(auto& result : query_results) {
        inja::json data;
        reflection::for_each(result, [&](auto& field, std::size_t idx) {
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

// 单表查询
std::string handle_query_all_users(mysql::MySQL& mysql, std::string user_id) {

    inja::json user_list;

    std::unordered_set<std::string> hash;
    if(!user_id.empty()) {
        mysql::query_field_set query_field{ "user_id" };
        auto query_results = mysql.query<std::tuple<decltype(mysql::user::user_id)>, mysql::user_friends>(
            "user = \"" + user_id + "\"",
            query_field
        );
        for(auto& result : query_results) {
            log_info(std::get<0>(result).data());
            hash.insert(std::get<0>(result).data());
        }
    }
    {
        constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
        auto query_results = mysql.query<mysql::user>();

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
                if(field_name == "user_id") {
                    if(hash.count(field.data())) {
                        data["is_friend"] = 1;
                    }
                    else {
                        data["is_friend"] = 0;
                    }
                }
            });
            user_list.push_back(data);
        }
    }
    std::string s = user_list.dump();
    log_info(s);
    return s;
}

// 连接查询
std::string handle_query_all_groups(mysql::MySQL& mysql,
                                                 std::unordered_map<std::string, std::string> query_kv_pairs) {

    mysql::query_field_set query_field{ "groups.*", "COUNT(*)" };
    auto query_results = mysql.query_execute<std::tuple<mysql::groups, int>> (
    "\
        SELECT groups.*, COUNT(*) AS friend_count FROM groups, user_group WHERE \
        groups.group_id = user_group.group_id AND \
        groups.user_id = \"" + query_kv_pairs["user_id"] + "\" \
        GROUP BY (groups.group_id) \
    ");

    constexpr auto field_names = reflection::reflection_t<mysql::groups>::arr();
    inja::json group_list;
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
        data["friend_count"] = std::get<1>(result);
        group_list.push_back(data);
    }
    std::string s = group_list.dump();
    log_info(s);
    return s;
}

// 多表连接查询
std::string handle_query_common_friend(mysql::MySQL& mysql,
                                        std::unordered_map<std::string, std::string> query_kv_pairs) {
    std::string user_id1 = query_kv_pairs["user_id"];
    std::string user_id2 = query_kv_pairs["friend_id"];
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
    std::string s= user_list.dump();
    log_info(s);
    return s;
}

// 单表查询
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

// 单表更新
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

// TODO
std::string handle_remove_user_with_user_id(mysql::MySQL& mysql, std::string params) {
    auto m = parse_query_params(std::move(params));
    auto user_id = m["user_id"];
    mysql.begin();
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
    mysql.commit();
    inja::json data;
    return data.dump();
}

// 插入
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

    try {
        mysql.insert(std::move(user), std::move(null_field));
    }
    catch(sql::SQLException& e) {
        data["reason"] = e.what();
    }
    std::string s = data.dump();
    log_info(s);
    return s;
}

// 单表查询（借助INDEX(recver_id, pending)）
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


// 单表插入
std::string handle_send_friend_request(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    query_kv_pairs["sender_id"] = query_kv_pairs["user_id"];
    query_kv_pairs["recver_id"] = query_kv_pairs["friend_id"];
    query_kv_pairs["sender_group"] = query_kv_pairs["group_name"];
    auto [t, null_field] = make_table_object<mysql::friend_request>(query_kv_pairs);
    mysql.insert(std::move(t), std::move(null_field));
    inja::json result;
    result["result"] = "send done";
    return result.dump();
}

// 事务，单表插入，单表删除
std::string handle_agree_friend_request(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql.begin();
    try {
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
    }
    catch(sql::SQLException& e) {
        log_error(e.what());
        mysql.rollback();
    }

    mysql.commit();

    inja::json result;
    result["result"] = "send done";
    return result.dump();
}

// 单表插入（主键约束，空值约束）
std::string handle_add_group(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    inja::json result;
    try {
        auto [t, null_field] = make_table_object<mysql::groups>(std::move(query_kv_pairs));
        mysql.insert(std::move(t), std::move(null_field));
        result["result"] = "done";
    }
    catch(sql::SQLException& e) {
        result["reason"] = e.what();
    }
    return result.dump();
}


// 单表插入
std::string handle_send_message(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    query_kv_pairs["sender_id"] = query_kv_pairs["user_id"];
    query_kv_pairs["recver_id"] = query_kv_pairs["friend_id"];
    auto [t, null_field] = make_table_object<mysql::chat_message>(std::move(query_kv_pairs));
    mysql.insert(std::move(t), std::move(null_field));
    inja::json result;
    result["result"] = "send done";
    return result.dump();
}

// 单表查询（INDEX(recver_id, pending)）
std::string handle_query_chat_message(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    auto query_result = mysql.query<mysql::chat_message>(
        "recver_id = \"" + query_kv_pairs["user_id"] + "\"",
        "pending = 1"
    );

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

// 单表删除
std::string handle_reject_friend_request(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql.remove<mysql::friend_request>("request_id=\"" + query_kv_pairs["request_id"] + "\"");
    inja::json result;
    result["result"] = "ok";
    return result.dump();
}

// 事务，两个单表删除
std::string handle_delete_friend(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql.begin();
    try{
        mysql.execute(
        "\
            DELETE FROM user_group WHERE \
            friend_id = \"" + query_kv_pairs["friend_id"] + "\" AND \
            group_id in ( \
                SELECT group_id FROM groups WHERE user_id = \"" + query_kv_pairs["user_id"] + "\" \
            )\
        ");

        mysql.execute(
        "\
            DELETE FROM user_group WHERE \
            friend_id = \"" + query_kv_pairs["user_id"] + "\" AND \
            group_id in ( \
                SELECT group_id FROM groups WHERE user_id = \"" + query_kv_pairs["friend_id"] + "\" \
            )\
        ");
    }
    catch(sql::SQLException& e) {
        log_error(e.what());
        mysql.rollback();
    }

    mysql.commit();

    inja::json result;
    result["result"] = "ok";
    return result.dump();
}

// 事务，单表更新，单表插入
std::string handle_reply_message(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql::update_field_set update_field{ { "pending" } };
    auto [t, _] = make_table_object<mysql::chat_message>(query_kv_pairs);
    auto [tt, null_field] = make_table_object<mysql::chat_message>(query_kv_pairs);
    null_field.emplace("message_id");

    mysql.begin();
    try {
        mysql.update(std::move(t), std::move(update_field));
        mysql.insert(std::move(tt), std::move(null_field));
    }
    catch(sql::SQLException& e) {
        log_error(e.what());
        mysql.rollback();
    }
    mysql.commit();

    inja::json result;
    result["result"] = "ok";
    return result.dump();
}

// 单表更新
std::string handle_read_done_message(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql::update_field_set update_field{ { "pending" } };
    auto [t, _] = make_table_object<mysql::chat_message>(query_kv_pairs);
    mysql.update(std::move(t), std::move(update_field));

    inja::json result;
    result["result"] = "ok";
    return result.dump();
}

// 单表更新
std::string handle_modify_user(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    auto [t, _] = make_table_object<mysql::user>(query_kv_pairs);
    mysql.update(std::move(t));

    inja::json result;
    result["result"] = "ok";
    return result.dump();
}

// 事务，删除，插入，连接查询
std::string handle_modify_friend_group(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql.begin();
    try {
        mysql.execute(
        "\
            DELETE FROM user_group WHERE \
            friend_id = \"" + query_kv_pairs["friend_id"] + "\" AND \
            group_id in ( \
                SELECT group_id FROM groups \
                WHERE user_id = \"" + query_kv_pairs["user_id"] + "\" \
            )\
        ");
        mysql.execute(
        "\
            INSERT INTO user_group (group_id, friend_id) VALUES ( \
                (\
                    SELECT group_id FROM groups WHERE \
                    user_id = \"" + query_kv_pairs["user_id"] + "\" AND \
                    group_name = \"" + query_kv_pairs["group_name"] + "\" \
                ), \
                \"" + query_kv_pairs["friend_id"] + "\" \
            )\
        ");

    }
    catch(sql::SQLException& e) {
        log_info(e.what());
        mysql.rollback();
    }
    try {
        mysql::query_field_set query_field { "user.*", "groups.group_name" };
        auto query_results = mysql.query<
            std::tuple<mysql::user, decltype(mysql::groups::group_name)>,
            mysql::user, mysql::groups, mysql::user_group>(
            "groups.user_id = \"" + query_kv_pairs["user_id"] + "\"",
            "groups.group_id = user_group.group_id",
            "user_group.friend_id = \"" + query_kv_pairs["friend_id"] + "\"",
            "user.user_id = user_group.friend_id",
            query_field
        );
        mysql.commit();

        constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
        inja::json data;
        if(query_results.empty()) {
            return data.dump();
        }
        reflection::for_each(std::get<0>(query_results.front()), [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (utils::is_cpp_array_v<field_type>) {
                data[field_names[idx].data()] = field.data();
            }
            else {
                data[field_names[idx].data()] = field;
            }
        });
        data["group_name"] = std::get<1>(query_results.front()).data();
        std::string s = data.dump();
        log_info(s);
        return s;
    }
    catch(sql::SQLException& e) {
        log_error(e.what());
        mysql.rollback();
    }
    return "";
}

// 单表查询
std::string handle_query_user_with_user_id(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    auto query_results = mysql.query<mysql::user>("user_id = \"" + query_kv_pairs["user_id"] + "\"");

    constexpr auto field_names = reflection::reflection_t<mysql::user>::arr();
    inja::json data;
    if(query_results.empty()) {
        return data.dump();
    }
    reflection::for_each((query_results.front()), [&](auto& field, std::size_t idx) {
        using field_type = std::remove_reference_t<decltype(field)>;
        if constexpr (utils::is_cpp_array_v<field_type>) {
            data[field_names[idx].data()] = field.data();
        }
        else {
            data[field_names[idx].data()] = field;
        }
    });
    std::string s = data.dump();
    log_info(s);
    return s;
}

// 单表更新
std::string handle_modify_group_name(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    auto [t, null_field] = make_table_object<mysql::groups>(query_kv_pairs);

    mysql.update(std::move(t));
    inja::json data;
    return data.dump();
}

// 事务，删除
std::string handle_delete_group(mysql::MySQL& mysql, query_kv_map query_kv_pairs) {
    mysql.begin();
    mysql.execute(
    "\
        DELETE FROM user_group WHERE group_id IN ( \
            SELECT a.group_id FROM ( \
                SELECT group_id FROM user_group WHERE \
                friend_id = ( \
                    SELECT user_id FROM groups WHERE group_id = ? \
                ) \
                AND \
                group_id IN ( \
                    SELECT group_id FROM groups WHERE user_id IN ( \
                        SELECT friend_id FROM user_group WHERE group_id = ? \
                    ) \
                ) \
            ) \
        AS a)\
    ", query_kv_pairs["group_id"], query_kv_pairs["group_id"]);

    mysql.execute( "DELETE FROM user_group WHERE group_id = ? ", query_kv_pairs["group_id"]);
    mysql.execute(" DELETE FROM groups WHERE group_id = ?", query_kv_pairs["group_id"]);
    mysql.commit();

    inja::json data;
    data["result"] = "ok";
    return data.dump();
}

