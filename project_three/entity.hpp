#pragma once

#include "std.hpp"
#include "reflection.hpp"

namespace mysql
{
    struct user
    {
        std::array<char, 50> user_id{ "" };  // PRIMARY KEY         INDEX
        std::array<char, 50> password{ "" };
        std::array<char, 50> nickname{ "" };
        std::array<char, 50> fullname{ "" };
        std::array<char, 50> sex{ "male" };
        std::array<char, 50> email{ "" };
        std::array<char, 50> address{ "" };
    };
    REFLECTION(user, user_id, password, nickname, fullname, sex, email);

    struct edu_experience
    {
        std::array<char, 50> user_id{ "" };
        std::array<char, 50> level{ "" };
        std::array<char, 50> school{ "" };
        std::array<char, 50> degree{ "" };
        std::chrono::system_clock::time_point start_date;
        std::chrono::system_clock::time_point end_date;
    };
    REFLECTION(edu_experience, user_id, level, school, degree, start_date, end_date)

    struct work_experience
    {
        std::array<char, 50> user_id;
        std::array<char, 50> position;
        std::array<char, 50> company;
        std::chrono::system_clock::time_point start_date;
        std::chrono::system_clock::time_point end_date;
    };
    REFLECTION(work_experience, user_id, position, company, start_date, end_date)

    struct user_group
    {
                                    // PRIMARY KEY(group_id, friend_id)
        std::int32_t group_id;      // INDEX
        std::array<char, 50> friend_id{ "" };
    };
    REFLECTION(user_group, group_id, friend_id)

    struct groups
    {
        std::int32_t group_id;                  // PRIMARY KEY(group_id, user_id)
        std::array<char, 50> user_id{ "" };     // INDEX
        std::array<char, 50> group_name{ "" };
    };
    REFLECTION(groups, group_id, user_id, group_name)

    struct log
    {
        std::int32_t log_id;
        std::array<char, 50> user_id;
        std::array<char, 50> title;
        std::array<char, 1000> context;
        std::chrono::system_clock::time_point publish_date;
        std::chrono::system_clock::time_point modify_date;
    };
    REFLECTION(log, log_id, user_id, title, context, publish_date, modify_date)

    struct log_message
    {
        std::array<char, 50> sender_id;
        std::int32_t log_id;
        std::array<char, 1000> context;
        std::chrono::system_clock::time_point publish_date;
    };
    REFLECTION(log_message, sender_id, log_id, context, publish_date)

    struct chat_message
    {
        std::int32_t message_id;
        std::array<char, 50> sender_id;
        std::array<char, 50> recver_id;
        std::array<char, 1000> context;
        std::int32_t pending;
    };
    REFLECTION(chat_message, message_id, sender_id, recver_id, context, pending)

    struct share_log
    {
        std::array<char, 50> sharer_id;
        std::int32_t log_id;
        std::array<char, 1000> context;
        std::chrono::system_clock::time_point share_date;
    };
    REFLECTION(share_log, sharer_id, log_id, context, share_date)

    struct friend_request
    {
        std::int32_t request_id;            // PRIMARY KEY
        std::array<char, 50> sender_id;     // INDEX
        std::array<char, 50> recver_id;     // INDEX
        std::int32_t pending;               // INDEX
        std::array<char, 50> sender_group;
        std::array<char, 50> recver_group;
    };
    REFLECTION(friend_request, request_id, sender_id, recver_id, pending, sender_group, recver_group)


// VIEW

    struct user_friends
    {
        std::array<char, 50> user{ "" };
        std::array<char, 50> group_name{ "" };
        std::array<char, 50> user_id{ "" };
        std::array<char, 50> password{ "" };
        std::array<char, 50> nickname{ "" };
        std::array<char, 50> fullname{ "" };
        std::array<char, 50> sex{ "male" };
        std::array<char, 50> email{ "" };
    };
    REFLECTION(user_friends, user, group_name, user_id, password, nickname, fullname, sex, email);

    struct friends_in_every_group
    {
        std::array<char, 50> user_id{ "" };
        std::array<char, 50> password{ "" };
        std::array<char, 50> nickname{ "" };
        std::array<char, 50> fullname{ "" };
        std::array<char, 50> sex{ "male" };
        std::array<char, 50> email{ "" };
        std::int32_t group_id;
    };
    REFLECTION(friends_in_every_group, user_id, password, nickname, fullname, sex, email, group_id);
}
