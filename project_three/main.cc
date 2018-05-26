#include "handler.hpp"

int main()
{
    using namespace cortono;
    /* cortono::util::logger::close_logger(); */

    mysql::MySQL mysql("tcp://127.0.0.1", "root", "3764819", "social");

    http::SimpleApp app;
    app.register_rule("/www/<path>")([&](const http::Request& req, http::Response& res, std::string filename) {
        if(filename == "query_all_friends") {
            res = http::Response(handle_query_all_friends(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_user_with_user_id") {
            res = http::Response(handle_query_user_with_user_id(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_friends_with_group_name") {
            res = http::Response(handle_query_friend_with_group_name(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_all_users") {
            res = http::Response(handle_query_all_users(mysql));
        }
        else if(filename == "query_all_groups_with_user_id") {
            res = http::Response(handle_query_all_groups_with_user_id(mysql, req.query_kv_pairs));
        }
        else if(filename == "query_common_friends") {
            res = http::Response(handle_query_common_friends(mysql, req.query_kv_pairs));
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
