#include "../mysql.hpp"
#include "entity.hpp"
#include <tuple>

template <typename T>
void single_query(mysql::MySQL& mysql) {
    auto query_results = mysql.query<T>();
    for(auto& tb : query_results) {
        reflection::for_each(tb, [](auto& field, std::size_t) {
            std::cout << field << " ";
        });
        std::cout << "\n";
    }
}

template <typename T, typename... Args>
void single_query_with_condition(mysql::MySQL& mysql, Args&&... args) {
    auto query_results = mysql.query<T>(std::forward<Args>(args)...);
    for(auto& tb : query_results) {
        reflection::for_each(tb, [](auto& field, std::size_t) {
            std::cout << field << " ";
        });
        std::cout << "\n";
    }
}

template <typename... Ts>
void multi_query(mysql::MySQL& mysql) {
    auto query_results_tp = mysql.query<Ts...>();
    for(auto& result_tp : query_results_tp) {
        utils::for_each(result_tp, [](auto& tb, std::size_t ) {
            reflection::for_each(tb, [](auto& item, std::size_t ) {
                std::cout << item << " ";
            });
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(result_tp)>>>{});
        std::cout << "\n";
    }
}


template <typename... Ts, typename... Args>
void multi_query_with_condition(mysql::MySQL& mysql, Args&&... args) {
    auto query_results_tp = mysql.query<Ts...>(std::forward<Args>(args)...);
    for(auto& result_tp : query_results_tp) {
        utils::for_each(result_tp, [](auto& tb, std::size_t ) {
            reflection::for_each(tb, [](auto& item, std::size_t ) {
                std::cout << item << " ";
            });
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(result_tp)>>>{});
        std::cout << "\n";
    }
}

void multi_query_with_field(mysql::MySQL& mysql, std::string sql) {
    /* std::string sql("SELECT ENAME FROM EMPLOYEE, WORKS_ON, PROJECT WHERE \ */
    /*                 EMPLOYEE.ESSN = WORKS_ON.ESSN  AND \ */
    /*                 WORKS_ON.PNO = PROJECT.PNO AND \ */
    /*                 PROJECT.PNAME = \"SQL Project\""); */
    auto query_results = mysql.query<std::tuple<std::string>>(sql);
    for(auto& result_tp : query_results) {
        utils::for_each(result_tp, [](auto& field, std::size_t) {
            std::cout << field << " ";
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(result_tp)>>>{});
        std::cout << "\n";
    }
}

int main()
{
    mysql::MySQL mysql("tcp://127.0.0.1", "root", "3764819", "social");

    mysql::mysql_key keys1{ { "user_id" } };
    mysql::mysql_not_null not_nulls1{ { "user_id", "password", "nickname" } };
    mysql.create_table<mysql::user>(keys1, not_nulls1);

    mysql::mysql_not_null not_nulls2{ { "user_id" } };
    mysql.create_table<mysql::edu_experience>(not_nulls2);

    mysql::mysql_not_null not_nulls3{ { "user_id" } };
    mysql.create_table<mysql::work_experience>(not_nulls3);

    mysql::mysql_key keys4{ { "group_id", "friend_id" } };
    mysql.create_table<mysql::user_group>(keys4);

    mysql::mysql_key keys5{ { "group_id", "user_id" } };
    mysql::mysql_not_null not_nulls5{ { "user_id", "group_name" } };
    mysql::mysql_auto_increment auto_incs5{ { "group_id" } };
    mysql.create_table<mysql::groups>(keys5, not_nulls5, auto_incs5);

    mysql::mysql_key keys6{ { "log_id" } };
    mysql::mysql_not_null not_nulls6{ { "user_id", "title", "publish_date", "modify_date" } };
    mysql.create_table<mysql::log>(keys6, not_nulls6);

    mysql::mysql_not_null not_nulls7{ { "sender_id", "recver_id", "publish_date" } };
    mysql.create_table<mysql::chat_message>(not_nulls7);

    mysql::mysql_not_null not_nulls8{ { "sender_id", "log_id", "publish_date" } };
    mysql.create_table<mysql::chat_message>(not_nulls8);

    mysql::mysql_not_null not_nulls9{ { "sharer_id", "log_id", "publish_date" } };
    mysql.create_table<mysql::log_message>(not_nulls9);

    mysql::mysql_not_null not_nulls10{ { "sharer_id", "log_id", "share_date" } };
    mysql.create_table<mysql::share_log>(not_nulls10);

    // insert user
    mysql::user user1{ "user1", "3764819", "roc", "wangpeng", "male", "wangpeng29029@163.com", "HIT_A02_6103" };
    mysql.insert(user1);

    mysql::user user2{ "user2", "1234567", "u2" };
    mysql.insert(user2, { "fullname", "sex", "email", "address" });

    mysql::user user3{ "user3", "23423", "u3" };
    mysql.insert(user3, { "fullname", "sex", "email", "address" });

    mysql::user user4{ "user4", "23423", "u4" };
    mysql.insert(user4, { "fullname", "sex", "email", "address" });

    mysql::user user5{ "user5", "23423", "u5" };
    mysql.insert(user5, { "fullname", "sex", "email", "address" });

    mysql::user user6{ "user6", "23423", "u6" };
    mysql.insert(user6, { "fullname", "sex", "email", "address" });

    try {
        mysql::user user7{ "user7" };
        mysql.insert(user7, { "password", "nickname", "fullname", "sex", "email", "address" });
    }
    catch(sql::SQLException& e) {
        std::cout << "insert error: " << e.what() << std::endl;
    }

    try {
        mysql::user user7{ "user2", "balabala", "u2" };
        mysql.insert(user7, { "fullname", "sex", "email", "address" });
    }
    catch(sql::SQLException& e) {
        std::cout << "insert error: " << e.what() << std::endl;
    }

    // insert education experience
    mysql::edu_experience exp1{ "user1", "high", "HIT", "bachelor" };
    mysql.insert(exp1, {  "start_date", "end_date" });

    mysql::edu_experience exp2{ "user2" };
    mysql.insert(exp2, { "level", "school", "degree", "start_date", "end_date" });

    try {
        mysql::edu_experience exp3;
        mysql.insert(exp3, { "user_id", "level", "school", "degree", "start_date", "end_date" });
    }
    catch(sql::SQLException& e) {
        std::cout << "insert education experience error: " << e.what() << std::endl;
    }

    // create group
    // 1
    mysql::groups group1{ 0, "user1", "user1_group1" };
    mysql.insert(group1, { "group_id" });

    // 2
    mysql::groups group2{ 0, "user2", "user2_group2" };
    mysql.insert(group2);

    // modify group name
    mysql::groups group21{ 2, "user2", "user2_group1" };
    mysql.update(group21);

    // 3
    mysql::groups group3{ 0, "user3", "user3_group1" };
    mysql.insert(group3);

    // 4
    mysql::groups group4{ 0, "user4", "user4_group1" };
    mysql.insert(group4);

    // 5
    mysql::groups group5{ 0, "user5", "user5_group1" };
    mysql.insert(group5);

    // 6
    mysql::groups group6{ 0, "user6", "user6_group1" };
    mysql.insert(group6);

    try {
        mysql::groups group10;
        mysql.insert(group10, { "group_name" });
    }
    catch(sql::SQLException& e) {
        std::cout << "insert group error: " << e.what() << std::endl;
    }

    // insert friends
    // user1 and user2
    mysql::user_group user_group1{ 1, "user2" };
    mysql.insert(user_group1);
    mysql::user_group user_group2{ 2, "user1" };
    mysql.insert(user_group2);

    // user1 and user3
    mysql::user_group user_group3{ 1, "user3" };
    mysql.insert(user_group3);
    mysql::user_group user_group4{ 3, "user1" };
    mysql.insert(user_group4);

    // user1 and user4
    mysql::user_group user_group5{ 1, "user4" };
    mysql.insert(user_group5);
    mysql::user_group user_group6{ 4, "user1" };
    mysql.insert(user_group6);

    // user2 and user3
    mysql::user_group user_group7{ 2, "user3" };
    mysql.insert(user_group7);
    mysql::user_group user_group8{ 3, "user2" };
    mysql.insert(user_group8);

    try {
        mysql::user_group user_group3{ 2 };
        mysql.insert(user_group3, { "friend_id" });
    }
    catch(sql::SQLException& e) {
        std::cout << "insert user_group error: " << e.what() << std::endl;
    }

    try {
        mysql::user_group user_group3;
        mysql.insert(user_group3, { "group_id", "friend_id" });
    }
    catch(sql::SQLException& e) {
        std::cout << "insert user_group error: " << e.what() << std::endl;
    }

    // query friends
    std::cout << "user1's friends" << std::endl;
    auto user1_friends = mysql.query<mysql::user_group, mysql::groups>(
        "user_group.group_id = groups.group_id",
        "groups.user_id = \"user1\""
    );
    for(auto&& user1_friend : user1_friends) {
        utils::for_each(user1_friend, [](auto& tb, std::size_t) {
            using table_type = std::remove_reference_t<decltype(tb)>;
            if constexpr (std::is_same_v<table_type, mysql::user_group>) {
                std::cout << std::string(tb.friend_id.begin(), tb.friend_id.end()) << " ";
            }
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(user1_friend)>>>{});
        std::cout << std::endl;
    }

    std::cout << "commom friends of user1 and user2" << std::endl;
    auto common_friends_in_user12 = mysql.query<std::tuple<std::array<char, 50>>>(
        "select ug1.friend_id from user_group as ug1, groups as gs1 where \
         ug1.group_id=gs1.group_id and gs1.user_id=\"user1\" and ug1.friend_id in ( \
         select ug2.friend_id from user_group as ug2, groups as gs2 where \
         ug2.group_id=gs2.group_id and gs2.user_id=\"user2\")"
    );
    for(auto&& common_friend : common_friends_in_user12) {
        utils::for_each(common_friend, [](auto& field, std::size_t) {
            using field_type = std::remove_reference_t<decltype(field)>;
            if constexpr (std::is_same_v<field_type, std::array<char, 50>>) {
                std::cout << std::string(field.data()) << " ";
            }
            else {
                std::cout << field << " ";
            }
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(common_friend)>>>{});
        std::cout << std::endl;
    }



    /* single_query(mysql); */
    /* std::cout << "\n\n"; */
    /* single_query_with_condition(mysql); */
    /* std::cout << "\n\n"; */
    /* multi_query(mysql); */
    /* std::cout << "\n\n"; */
    /* multi_query_with_condition(mysql); */
    /* std::cout << "\n\n"; */

    /* multi_query_with_field(mysql); */
    /* std::cout << "\n\n"; */
    return 0;
}
