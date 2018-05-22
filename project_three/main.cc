#include <cortono/cortono.hpp>
#include "mysql.hpp"
#include "entity.hpp"

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
T make_table_object(const std::unordered_map<std::string, std::string>& query_kv_pairs) {
    T t;
    std::unordered_set<std::string_view> null_field_set;
    using table_type = reflection::reflection_t<T>;
    constexpr auto field_names = table_type::arr();
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
    mysql.insert(make_table_object<mysql::groups>(query_kv_pairs));
}
void handle_query_all_friends(mysql::MySQL& mysql, std::string params) {
    auto query_kv_pairs = parse_query_params(std::move(params));

}
int main()
{
    using namespace cortono;
    /* cortono::util::logger::close_logger(); */

    mysql::MySQL mysql("tcp://127.0.0.1", "root", "3764819", "social");

    http::SimpleApp app;
    app.register_rule("/www/<path>")([](const http::Request&, http::Response& res, std::string filename) {
        if(filename == "query_all_friends") {
            res = http::Response(handle_query_all_friends(mysql, req.body));
        }
        else if(filename == "query_user_with_user_id") {
            res = http::Response(handle_query_user_with_user_id(mysql, req.body));
        }
        else if(filename == "query_friends_with_group_name") {
            res = http::Response(handle_query_friend_with_group_name(mysql, req.body));
        }
        else {
            res = http::Response(200);
            res.send_file("www/" + filename);
        }
    });

    app.register_rule("/www/<string>").methods(http::HttpMethod::POST)([&](const http::Request& req, std::string action) {
        if(action == "register_user") {
            handle_register_user(mysql, req.body);
        }
        else if(action == "create_group") {
            handle_create_group(mysql, req.body);
        }
        else if(action == "add_friend") {
            handle_add_friend(mysql, req.body);
        }
        return "ok";
    });


    app.multithread().port(10000).run();
    return 0;
}
