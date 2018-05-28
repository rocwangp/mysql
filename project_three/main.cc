#include "handler.hpp"

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
int main()
{
    using namespace cortono;
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
    mysql::not_null not_nulls11{ { "request_id", "sender_id", "recver_id", "pending" } };
    mysql.bind_attribute<mysql::friend_request>(key11, not_nulls11);

    http::SimpleApp app;
    app.register_rule("/www/<path>")([&](const http::Request& req, http::Response& res, std::string filename) {
        log_info(filename);
        if(is_file_path(filename)) {
            res = http::Response(200);
            res.send_file("www/" + filename);
            return;
        }
        else if(filename == "query_all_users") {
            res = http::Response(handle_query_all_users(mysql));
        }
        else if(filename == "query_all_friends") {
            auto query_pairs = req.query_kv_pairs;
            auto ws = req.get_session(query_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_pairs["user_id"]) {
                res = http::Response("no login");
            }
            else {
                res = http::Response(handle_query_all_friends(mysql, req.query_kv_pairs));
            }
        }
        else if(filename == "add_friend") {
            auto query_pairs = req.query_kv_pairs;
            auto ws = req.get_session(query_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_pairs["user_id"]) {
                res = http::Response("no login");
            }
            else {
                res = http::Response(handle_request_friend(mysql, req.query_kv_pairs));
            }
        }
        else if(filename == "query_friend_request") {
            auto query_pairs = req.query_kv_pairs;
            auto ws = req.get_session(query_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_pairs["user_id"]) {
                if(session == nullptr) {
                    log_info("session is nullptr");
                }
                log_info("error");
                res = http::Response("no login");
            }
            else {
                res = http::Response(handle_query_friend_request(mysql, query_pairs));
            }
        }
        else if(filename == "query_chat_message") {
            auto query_pairs = req.query_kv_pairs;
            auto ws = req.get_session(query_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_pairs["user_id"]) {
                if(session == nullptr) {
                    log_info("session is nullptr");
                }
                log_info("error");
                res = http::Response("no login");
            }
            else {
                res = http::Response(handle_query_chat_message(mysql, query_pairs));
            }
        }
        else {
            log_error("no handler");
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
        }
        else if(action == "send_friend_request") {
            auto query_kv_pairs = parse_query_params(req.body);
            auto ws = req.get_session(query_kv_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_kv_pairs["user_id"]) {
                if(session == nullptr) {
                    log_info("not login");
                }
                res = http::Response("not login");
            }
            else {
                res = http::Response(handle_send_friend_request(mysql, query_kv_pairs));
            }
        }
        else if(action == "agree_friend_request") {
            auto query_kv_pairs = parse_query_params(req.body);
            auto ws = req.get_session(query_kv_pairs["recver_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_kv_pairs["recver_id"]) {
                res = http::Response("not login");
            }
            else {
                res = http::Response(handle_agree_friend_request(mysql, query_kv_pairs));
            }
        }
        else if(action == "add_group") {
            auto query_kv_pairs = parse_query_params(req.body);
            auto ws = req.get_session(query_kv_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_kv_pairs["user_id"]) {
                res = http::Response("not login");
            }
            else {
                res = http::Response(handle_add_group(mysql, query_kv_pairs));
            }
        }
        else if(action == "modify_user") {
            auto query_kv_pairs = parse_query_params(req.body);
            auto ws = req.get_session(query_kv_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_kv_pairs["user_id"]) {
                res = http::Response("not login");
            }
            else {
                res = http::Response(handle_modify_user(mysql, req.body));
            }
        }
        else if(action == "send_message") {
            auto query_kv_pairs = parse_query_params(req.body);
            auto ws = req.get_session(query_kv_pairs["user_id"]);
            auto session = ws.lock();
            if(session == nullptr || session->get_data<std::string>("user_id") != query_kv_pairs["user_id"]) {
                if(session == nullptr) {
                    log_info("session is nullptr");
                }
                log_info("error");
                res = http::Response("not login");
            }
            else {
                res = http::Response(handle_send_message(mysql, query_kv_pairs));
            }
        }
        else if(action == "remove_user") {
            res = http::Response(handle_remove_user_with_user_id(mysql, req.body));
        }
        else if(action == "register_user") {
            res = http::Response(handle_register_user(mysql, req.body));
        }
    });


    app.multithread().port(9999).run();
    return 0;
}
