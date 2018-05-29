#include "handler.hpp"

using namespace cortono;
bool is_file_path(const std::string& filename) {
    if(filename.size() > 5 && filename.substr(filename.size() - 5) == ".html") {
        return true;
    }
    else if(filename.size() > 4 && filename.substr(filename.size() - 4) == ".jsp") {
        return true;
    }
    else {
        return false;
    }
}

bool is_login(const http::Request& req) {
    std::unordered_map<std::string, std::string> query_kv_pairs;
    if(req.method == http::HttpMethod::POST) {
        query_kv_pairs = parse_query_params(req.body);
    }
    else {
        query_kv_pairs = req.query_kv_pairs;
    }
    auto ws = req.get_session(query_kv_pairs["user_id"]);
    auto session = ws.lock();
    if(session == nullptr || session->get_data<std::string>("user_id") != query_kv_pairs["user_id"]) {
        if(session == nullptr) {
            log_info("session is nullptr");
        }
        log_info("no login");
        return false;
    }
    else {
        return true;
    }
}
int main()
{
    /* cortono::util::logger::close_logger(); */

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

    mysql::primary_key keys7{ { "message_id" } };
    mysql::not_null not_nulls7{ { "message_id", "sender_id", "recver_id"  } };
    mysql.bind_attribute<mysql::chat_message>(keys7, not_nulls7);

    mysql::not_null not_nulls9{ { "sender_id", "log_id", "publish_date" } };
    mysql.bind_attribute<mysql::log_message>(not_nulls9);

    mysql::not_null not_nulls10{ { "sharer_id", "log_id", "share_date" } };
    mysql.bind_attribute<mysql::share_log>(not_nulls10);

    mysql::primary_key key11{ { "request_id" } };
    mysql::not_null not_nulls11{ { "request_id", "sender_id", "recver_id"  } };
    mysql.bind_attribute<mysql::friend_request>(key11, not_nulls11);

    http::SimpleApp app;
    app.register_rule("/www/<path>")([&](const http::Request& req, http::Response& res, std::string filename) {
        log_info(filename);
        if(is_file_path(filename)) {
            res = http::Response(200);
            res.send_file("www/" + filename);
            return;
        }
        if(filename == "query_all_users") {
            if(is_login(req)) {
                auto query_pairs = req.query_kv_pairs;
                res = http::Response(handle_query_all_users(mysql, query_pairs["user_id"]));
            }
            else {
                res = http::Response(handle_query_all_users(mysql, ""));
            }
            return;
        }
        if(!is_login(req)) {
            res = http::Response("no login");
            return;
        }
        else if(filename == "query_all_friends") {
            res = http::Response(handle_query_all_friends(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_friend_request") {
            res = http::Response(handle_query_friend_request(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_chat_message") {
            res = http::Response(handle_query_chat_message(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_common_friend") {
            res = http::Response(handle_query_common_friend(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_user_information") {
            res = http::Response(handle_query_user_with_user_id(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_all_groups") {
            res = http::Response(handle_query_all_groups(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_friends_in_group") {
            res = http::Response(handle_query_friends_in_group(mysql, req.query_kv_pairs));
        }
        else {
            res = http::Response("no handler");
        }
    });

    app.register_rule("/www/<string>").methods(http::HttpMethod::POST)([&](const http::Request& req, http::Response& res, std::string action) {
        log_info(action);
        if(action == "user_login") {
            auto query_kv_pairs = parse_query_params(req.body);
            auto [result, content] = handle_user_login(mysql, query_kv_pairs);
            if(result == true) {
                res = http::Response(std::move(content));
                auto session = res.start_session(query_kv_pairs["user_id"]);
                session->set_data("user_id", query_kv_pairs["user_id"]);
            }
            else {
                res = http::Response(std::move(content));
            }
            return;
        }
        if(action == "register_user") {
            res = http::Response(handle_register_user(mysql, req.body));
            return;
        }
        if(!is_login(req)) {
            res = http::Response("no login");
            return;
        }
        else if(action == "send_friend_request") {
            res = http::Response(handle_send_friend_request(mysql, req.query_kv_pairs));
        }
        else if(action == "agree_friend_request") {
            res = http::Response(handle_agree_friend_request(mysql, req.query_kv_pairs));
        }
        else if(action == "reject_friend_request") {
            res = http::Response(handle_reject_friend_request(mysql, req.query_kv_pairs));
        }
        else if(action == "add_group") {
            res = http::Response(handle_add_group(mysql, req.query_kv_pairs));
        }
        else if(action == "modify_user") {
            res = http::Response(handle_modify_user(mysql, req.body));
        }
        else if(action == "send_message") {
            res = http::Response(handle_send_message(mysql, req.query_kv_pairs));
        }
        else if(action == "delete_friend") {
            res = http::Response(handle_delete_friend(mysql, req.query_kv_pairs));
        }
        else if(action == "reply_message") {
            res = http::Response(handle_reply_message(mysql, req.query_kv_pairs));
        }
        else if(action == "read_done_message") {
            res = http::Response(handle_read_done_message(mysql, req.query_kv_pairs));
        }
        else if(action == "modify_user") {
            res = http::Response(handle_modify_user(mysql, req.query_kv_pairs));
        }
        else if(action == "modify_friend_group") {
            res = http::Response(handle_modify_friend_group(mysql, req.query_kv_pairs));
        }
        else if(action == "modify_group_name") {
            res = http::Response(handle_modify_group_name(mysql, req.query_kv_pairs));
        }
        else if(action == "delete_group") {
            res = http::Response(handle_delete_group(mysql, req.query_kv_pairs));
        }
        else {
            res = http::Response("no handler");
        }
    });


    app.multithread().port(9999).run();
    return 0;
}
