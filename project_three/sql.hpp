#pragma once

#include "std.hpp"
#include "reflection.hpp"
#include "utility.hpp"
#include "type_mapping.hpp"

#include <cortono/util/util.hpp>

namespace mysql
{
    enum class attribute
    {
        key,
        not_null,
        auto_increment
    };

    struct primary_key
    {
        primary_key(std::vector<std::string_view>&& f) : fields(std::move(f)) {}

        std::vector<std::string_view> fields;
        static constexpr attribute attr = attribute::key;
    };

    struct not_null
    {
        not_null(std::vector<std::string_view>&& f) : fields(std::move(f)) {}

        std::vector<std::string_view> fields;
        static constexpr attribute attr = attribute::not_null;
    };

    struct auto_increment
    {
        auto_increment(std::vector<std::string_view>&& f) : fields(std::move(f)) {}

        std::vector<std::string_view> fields;
        static constexpr attribute attr = attribute::auto_increment;
    };

    using rename_map = std::unordered_map<std::string_view, std::list<std::string_view>>;
    using null_field_set = std::unordered_set<std::string_view>;
    using primary_key_map = std::unordered_map<std::string_view, std::unordered_set<std::string_view>>;
    using not_null_map = std::unordered_map<std::string_view, std::unordered_set<std::string_view>>;
    using auto_increment_map = std::unordered_map<std::string_view, std::unordered_set<std::string_view>>;
    using query_field_set = std::vector<std::string_view>;
    using update_field_set = std::unordered_set<std::string_view>;

    template <typename... Ts, typename Tuple>
    std::string make_query_sql(rename_map&& renames, query_field_set&& query_field, Tuple&& conditions) {
        static_assert(utils::is_tuple_v<Tuple>, "not a tuple");
        std::string sql;
        if(query_field.empty()) {
            sql = "SELECT * FROM ";
        }
        else {
            sql = "SELECT ";
            for(auto it = query_field.begin(); it != query_field.end(); ++it) {
                if(it != query_field.begin()) {
                    utils::string_append(sql, ", ");
                }
                utils::string_append(sql, *it);
            }
            utils::string_append(sql, " FROM ");
        }
        bool first{ true };
        utils::for_each(std::tuple<Ts...>{}, [&](auto&& tb, std::size_t) {
            using table_type = std::remove_reference_t<decltype(tb)>;
            static_assert(reflection::is_reflection<table_type>::value, "the table needs to be a reflected type");

            if(!first) {
                utils::string_append(sql, ", ");
            }
            else {
                first = false;
            }
            constexpr auto table_name = reflection::reflection_t<table_type>::name();
            if(auto it = renames.find(table_name); it != renames.end()) {
                if(it->second.empty()) {
                    throw std::runtime_error("error query");
                }
                utils::string_append(sql, table_name, " as ", it->second.front());
                it->second.pop_front();
            }
            else {
                utils::string_append(sql, table_name);
            }
        }, std::make_index_sequence<sizeof...(Ts)>{});

        utils::for_each(conditions, [&](auto& condition, std::size_t idx) {
            if(idx == 0) {
                utils::string_append(sql, " WHERE ");
            }
            else {
                utils::string_append(sql, " AND ");
            }
            utils::string_append(sql, condition);
        }, std::make_index_sequence<std::tuple_size_v<Tuple>>{});

        log_info(sql);
        return sql;
    }

    template <typename T>
    std::string make_insert_sql(T&& t, null_field_set&& null_fields = null_field_set{}) {
        static_assert(reflection::is_reflection<T>::value, "the table needs to be a reflected type");
        constexpr auto table_name = reflection::reflection_t<T>::name();
        constexpr auto field_names = reflection::reflection_t<T>::arr();

        std::string sql;
        utils::string_append(sql, "INSERT INTO ", table_name, " ");
        bool first{ true };
        for(std::size_t i = 0; i != field_names.size(); ++i) {
            if(null_fields.count(field_names[i])) {
                continue;
            }
            if(first) {
                utils::string_append(sql, "(");
                first = false;
            }
            else {
                utils::string_append(sql, ", ");
            }
            utils::string_append(sql, field_names[i]);
        }
        utils::string_append(sql, ")");

        first = true;
        utils::string_append(sql, " VALUES (");
        reflection::for_each(t, [&](auto&& field, std::size_t idx) {
            if(null_fields.count(field_names[idx])) {
                return;
            }
            if(!first) {
                utils::string_append(sql, ", ");
            }
            else {
                first = false;
            }
            using field_type = std::remove_reference_t<decltype(field)>;
            utils::string_append(sql, "\"");
            if constexpr (utils::is_cpp_array_v<field_type>) {
                log_info(field.data());
                utils::string_append(sql, field.data());
            }
            else if constexpr (utils::is_system_time_point_v<field_type>) {
                utils::string_append(sql, utils::time_point_to_string(field));
            }
            else {
                utils::string_append(sql, field);
            }
            utils::string_append(sql, "\"");
        });
        utils::string_append(sql, ")");
        log_info(sql);
        return sql;
    }

    template <typename T>
    std::string make_update_sql(T&& t, primary_key_map& primary_key, update_field_set&& update_field = update_field_set{}) {
        static_assert(reflection::is_reflection<T>::value, "the table needs to be a reflected type");
        constexpr auto field_names = reflection::reflection_t<T>::arr();
        constexpr auto table_name = reflection::reflection_t<T>::name();
        auto& primary_key_set = primary_key[table_name];

        std::string sql("UPDATE ");
        utils::string_append(sql, table_name);
        bool first{ true };
        reflection::for_each(t, [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            auto field_name = field_names[idx];
            if(primary_key_set.count(field_name)) {
                return;
            }
            if(!first) {
                utils::string_append(sql, ", ");
            }
            else {
                utils::string_append(sql, " SET ");
                first = false;
            }
            utils::string_append(sql, field_name, "=\"");
            if constexpr (utils::is_cpp_array_v<field_type>) {
                utils::string_append(sql, field.data(), "\"");
            }
            else {
                utils::string_append(sql, field, "\"");
            }
        });
        first = true;
        reflection::for_each(t, [&](auto& field, std::size_t idx) {
            using field_type = std::remove_reference_t<decltype(field)>;
            auto field_name = field_names[idx];
            if(!primary_key_set.count(field_name)) {
                return;
            }
            if(!first) {
                utils::string_append(sql, " AND ");
            }
            else {
                utils::string_append(sql, " WHERE ");
                first = false;
            }
            utils::string_append(sql, field_name, "=\"");
            if constexpr (utils::is_cpp_array_v<field_type>) {
                utils::string_append(sql, field.data(), "\"");
            }
            else {
                utils::string_append(sql, field, "\"");
            }
        });
        log_info(sql);
        return sql;
    }

    template <typename T, typename... Args>
    std::string make_remove_sql(Args&&... args) {
        static_assert(reflection::is_reflection<T>::value, "the table needs to be a reflected type");
        constexpr auto field_names = reflection::reflection_t<T>::arr();
        constexpr auto table_name = reflection::reflection_t<T>::name();
        std::string sql("DELETE FROM ");
        utils::string_append(sql, table_name);
        utils::for_each(std::make_tuple(args...), [&](auto& field, std::size_t idx) {
            if(idx == 0) {
                utils::string_append(sql, " WHERE ");
            }
            else {
                utils::string_append(sql, " AND ");
            }
            utils::string_append(sql, field);
        }, std::make_index_sequence<sizeof...(Args)>{});
        log_info(sql);
        return sql;
    }

    template <typename T>
    std::string make_create_tb_sql(primary_key_map& pk_map, not_null_map& nn_map, auto_increment_map& ai_map) {
        static_assert(reflection::is_reflection<T>::value, "the table needs to be a reflected type");
        constexpr auto table_name = reflection::reflection_t<T>::name();
        constexpr auto field_names = reflection::reflection_t<T>::arr();

        auto& primary_key_set = pk_map[table_name];
        auto& not_null_set = nn_map[table_name];
        auto& auto_increment_set = ai_map[table_name];

        std::string sql("CREATE TABLE IF NOT EXISTS " + table_name + " (");
        reflection::for_each(T{}, [&](auto&& field, std::size_t idx) {
            if(idx != 0) {
                utils::string_append(sql, ", ");
            }
            using field_type = std::remove_reference_t<decltype(field)>;
            constexpr auto field_name = field_names[idx];
            utils::string_append(sql, field_name, " ", type_to_name(identity<field_type>{}));

            if(not_null_set.count(field_name)) {
                utils::string_append(sql, " ", "NOT NULL");
            }
            if(auto_increment_set.count(field_name)) {
                utils::string_append(sql, " ", "AUTO_INCREMENT");
            }
        });

        for(auto it = primary_key_set.begin(); it != primary_key_set.end(); ++it) {
            if(it == primary_key_set.begin()) {
                utils::string_append(sql, ", PRIMARY KEY(");
            }
            else {
                utils::string_append(sql, ", ");
            }
            utils::string_append(sql, *it);
        }

        utils::string_append(sql, ")");

        log_info(sql);
        return sql;
    }

    template <typename T>
    std::string make_delete_tb_sql() {
        static_assert(reflection::is_reflection<T>::value, "the table needs to be a reflected type");
        std::string sql("DROP TABLE IF EXISTS " + reflection::reflection_t<T>::name());
        return sql;
    }

    static inline std::string make_create_db_sql(std::string_view name) {
        std::string sql("CREATE DATABASE IF NOT EXISTS ");
        utils::string_append(sql, name);
        log_info(sql);
        return sql;
    }
}
