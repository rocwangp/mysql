#pragma once

#include "mysql.hpp"

class SocialNetwork
{
    public:
        SocialNetwork()
            : mysql_("tcp:/127.0.0.1", "root", "3764819", "social_network")
        { }

        bool register_user(const mysql::user& user) {
            return mysql_.insert(user);
        }
        template <typename T>
        bool update(const T& t) {
            return mysql_.update(t);
        }
        template <typename T>
        bool insert(const T& t) {
            return mysql_.insert(t);
        }
        template <typename T>
        std::enable_if_t<reflection::is_reflection<T>::value, bool> delete_records(const T& t) {
            return mysql_.delete_records<T>(t);
        }

        template <typename... Args>
        auto query_friend(Args... args) {
            std::string sql("SELECT u2.* FROM user AS u1, groups, user AS u2 WHERE u1.user_id=groups.user_id AND groups.friend_id=u2.user_id");
            utils::for_each(std::make_tuple(args...), [&sql](auto& field, std::size_t idx) {
                if(idx != 0) {
                    utils::string_append(sql, " AND");
                }
                utils::string_append(sql, " ", field);
            }, std::make_index_sequence<sizeof...(Args)>{});
            return mysql_.query_execute<mysql::user>(std::move(sql));
        }
    private:
        mysql::MySQL mysql_;
};
