#pragma once

#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/connection.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/datatype.h>
#include <jdbc/cppconn/parameter_metadata.h>

#include "reflection.hpp"

namespace mysql
{

    enum class mysql_attr
    {
        key,
        not_null,
        auto_increment
    };

    struct mysql_key
    {
        mysql_key(std::vector<std::string_view>&& f) : fields(std::move(f) ){ }

        static constexpr mysql_attr attr = mysql_attr::key;
        std::vector<std::string_view> fields;
    };

    struct mysql_not_null
    {
        mysql_not_null(std::vector<std::string_view>&& f) : fields(std::move(f)) { }

        static constexpr mysql_attr attr = mysql_attr::not_null;
        std::vector<std::string_view> fields;
    };

    struct mysql_auto_increment
    {
        mysql_auto_increment(std::vector<std::string_view>&& f) : fields(std::move(f)) {}

        static constexpr mysql_attr attr = mysql_attr::auto_increment;
        std::vector<std::string_view> fields;
    };

    template <typename >
    struct identity {};

    static constexpr inline auto type_to_name(identity<std::int8_t>) { return "TINYINT"sv; }
    static constexpr inline auto type_to_name(identity<std::int16_t>) { return "SMALLINT"sv; }
    static constexpr inline auto type_to_name(identity<std::int32_t>) { return "INTEGER"sv; }
    static constexpr inline auto type_to_name(identity<float>) { return "FLOAT"sv; }
    static constexpr inline auto type_to_name(identity<double>) { return "DOUBLE"sv; }
    static constexpr inline auto type_to_name(identity<std::int64_t>) { return "BIGINT"sv; }
    static constexpr inline auto type_to_name(identity<std::uint32_t>) { return "INT UNSIGNED"sv; }
    static constexpr inline auto type_to_name(identity<std::string>) { return "TEXT"sv; }
    static constexpr inline auto type_to_name(identity<std::chrono::system_clock::time_point>) { return "DATETIME"sv; }
    template <std::size_t N>
    static constexpr inline auto type_to_name(identity<std::array<char, N>>) {
        return std::string("VARCHAR(" + std::to_string(N) + ")");
    }


    class MySQL
    {
        public:
            MySQL(std::string_view ip, std::string_view user, std::string_view passwd, std::string_view dbname)
                : driver_(::get_driver_instance()),
                  conn_(driver_->connect(ip.data(), user.data(), passwd.data()))
            {
                {
                    std::string sql = make_create_db_sql(dbname);
                    std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                    stmt->execute(sql);
                }
                conn_->setSchema(dbname.data());
            }

            template <typename T, typename... Args>
            void bind_primary_key_name(Args... args) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                using table_type = reflection::reflection_t<T>;
                constexpr auto table_name = table_type::name();

                utils::for_each(std::make_tuple(args...), [&table_name, this](auto&& field_name, std::size_t) {
                    primary_keys_[table_name].emplace(field_name);
                }, std::make_index_sequence<sizeof...(Args)>{});
            }

            template <typename T, typename... Args>
            bool create_table(Args&&... args) {
                delete_table<T>();
                std::string sql = make_create_tb_sql<T>(std::forward<Args>(args)...);
                std::cout << sql << std::endl;
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                return stmt->execute(sql);
            }
            template <typename T>
            bool delete_table() {
                std::string sql = make_delete_tb_sql<T>();
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                return stmt->execute(sql);
            }

            template <typename T>
            std::string make_delete_tb_sql() {
                std::string sql("DROP TABLE IF EXISTS ");
                utils::string_append(sql, reflection::reflection_t<T>::name().data());
                return sql;
            }

            template <typename Tuple>
            std::vector<Tuple> query(const std::string& sql) {
                std::vector<Tuple> query_results;
                {
                    Tuple t{};
                    std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql));
                    while(res->next()) {
                        utils::for_each(t, [&res,this](auto& field, std::size_t idx) {
                            query_impl(field, res, idx);
                        }, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
                        query_results.emplace_back(std::move(t));
                    }
                }
                return query_results;
            }

            // mysql.query<user, groups>("user.user_id=groups.user_id", "...", ...);
            template <typename... Ts, typename... Args>
            std::enable_if_t<(sizeof...(Ts)>1), std::vector<std::tuple<Ts...>>> query(Args&&... args) {
                std::string sql = make_query_sql<Ts...>(std::forward<Args>(args)...);
                /* std::vector<std::tuple<Ts...>> query_results; */
                std::vector<std::tuple<Ts...>> query_results;
                {
                    std::tuple<Ts...> t{};
                    std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql));
                    while(res->next()) {
                        std::size_t base_idx{ 0 };
                        utils::for_each(t, [&res, &base_idx, this](auto& tb, std::size_t) {
                            reflection::for_each(tb, [&res, &base_idx, this](auto& field, std::size_t idx) {
                                query_impl(field, res, base_idx + idx);
                            });
                            base_idx += reflection::reflection_t<std::remove_reference_t<decltype(tb)>>::count();
                        }, std::make_index_sequence<sizeof...(Ts)>{});
                        query_results.emplace_back(std::move(t));
                    }
                }
                return query_results;
            }

            // mysql.query<user>("user_id=2");
            // mysql.query<user>();
            template <typename T, typename... Args>
            std::enable_if_t<reflection::is_reflection<T>::value, std::vector<T>> query(Args&&... args)  {
                std::string sql = make_query_sql<T>(std::forward<Args>(args)...);
                /* std::vector<T> query_results; */
                return query_execute<T>(std::move(sql));
            }

            template <typename T>
            std::vector<T> query_execute(std::string&& sql) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                std::vector<T> query_results;
                {
                    T tb{};
                    std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(sql));
                    while(res->next()) {
                        reflection::for_each(tb, [&res, this](auto& field, std::size_t idx) {
                            query_impl(field, res, idx);
                        });
                        query_results.emplace_back(std::move(tb));
                    }
                }
                return query_results;
            }

            template <typename T>
            std::enable_if_t<reflection::is_reflection<T>::value, bool>
            insert(T&& t, std::unordered_set<std::string_view>&& null_field_set = std::unordered_set<std::string_view>{}) {
                std::string sql = make_insert_sql(std::forward<T>(t), null_field_set);
                std::cout << sql << std::endl;
                std::unique_ptr<sql::PreparedStatement> pstmt(conn_->prepareStatement(sql));
                std::size_t null_count = 0;
                constexpr auto field_names = reflection::reflection_t<T>::arr();
                reflection::for_each(t, [&, rsmd = pstmt->getParameterMetaData()](auto& field, std::size_t idx) {
                    if(null_field_set.count(field_names[idx])) {
                        ++null_count;
                        return;
                    }
                    using field_type = std::remove_reference_t<decltype(field)>;
                    idx -= null_count;
                    if constexpr (std::is_same_v<field_type, std::string>) {
                        pstmt->setString(idx + 1, field);
                    }
                    else if constexpr (utils::is_cpp_array_v<field_type>) {
                        pstmt->setString(idx + 1, std::string(field.data()));
                    }
                    else if constexpr (std::is_same_v<field_type, std::int32_t>) {
                        pstmt->setInt(idx + 1, field);
                    }
                    else if constexpr (std::is_same_v<field_type, std::uint32_t>) {
                        pstmt->setUInt(idx + 1, field);
                    }
                    else if constexpr (std::is_same_v<field_type, std::chrono::system_clock::time_point>) {
                        pstmt->setDateTime(idx + 1 ,utils::time_point_to_string(field));
                    }
                    else {
                        throw std::runtime_error(std::string("error field type ") + typeid(field_type).name());
                    }
                });
                return pstmt->executeUpdate();
            }

/*             template <typename... Ts> */
/*             std::enable_if_t<(sizeof...(Ts)>1), bool> insert(Ts&&... t) { */
/*                 bool done{ true }; */
/*                 utils::for_each(std::make_tuple(t...), [&done, this](auto&& tb, std::size_t) { */
/*                     using table_type = std::remove_reference_t<decltype(tb)>; */
/*                     static_assert(reflection::is_reflection<table_type>::value, "table need to be reflected type"); */
/*                     if(!this->insert(std::move(tb))) { */
/*                         done = false; */
/*                     } */
/*                 }, std::make_index_sequence<sizeof...(Ts)>{}); */
/*                 return done; */
/*             } */

            template <typename T>
            bool update(T&& t) {
                std::string sql = make_update_sql(t);
                std::cout << sql << std::endl;

                using table_type = reflection::reflection_t<T>;
                std::unique_ptr<sql::PreparedStatement> pstmt(conn_->prepareStatement(sql));
                reflection::for_each(t, [&pstmt](auto& field, std::size_t idx) {
                    using field_type = std::remove_reference_t<decltype(field)>;
                    if constexpr (std::is_same_v<field_type, std::string>) {
                        pstmt->setString(idx + 1, field);
                    }
                    else if constexpr (utils::is_cpp_array_v<field_type>) {
                        pstmt->setString(idx + 1, std::string(field.data()));
                    }
                    else if constexpr (std::is_same_v<field_type, std::int32_t>) {
                        pstmt->setInt(idx + 1, field);
                    }
                    else if constexpr (std::is_same_v<field_type, std::uint32_t>) {
                        pstmt->setUInt(idx + 1, field);
                    }
                    else if constexpr (std::is_same_v<field_type, std::chrono::system_clock::time_point>) {
                        pstmt->setDateTime(idx + 1, utils::time_point_to_string(field));
                    }
                    else {
                        throw std::runtime_error("error field type");
                    }
                });

                auto& primary_key = primary_keys_[table_type::name()];
                constexpr auto field_count = table_type::count();
                constexpr auto field_names = table_type::arr();
                reflection::for_each(t, [this, field_count, &field_names, &primary_key, &pstmt](auto& field, std::size_t idx) {
                    if(!primary_key.count(field_names[idx])) {
                        return;
                    }
                    using field_type = std::remove_reference_t<decltype(field)>;
                    idx += field_count;
                    if constexpr (std::is_same_v<field_type, std::string>) {
                        pstmt->setString(idx + 1, field);
                    }
                    else if constexpr (utils::is_cpp_array_v<field_type>) {
                        pstmt->setString(idx + 1, std::string(field.data()));
                    }
                    else if constexpr (std::is_same_v<field_type, std::int32_t>) {
                        pstmt->setInt(idx + 1, field);
                    }
                    else if constexpr (std::is_same_v<field_type, std::uint32_t>) {
                        pstmt->setUInt(idx + 1, field);
                    }
                    else if constexpr (std::is_same_v<field_type, std::chrono::system_clock::time_point>) {
                        pstmt->setDateTime(idx + 1, utils::time_point_to_string(field));
                    }
                    else {
                        throw std::runtime_error("error field type");
                    }
                });

                return pstmt->executeUpdate();
            }

            template <typename T>
            std::string make_update_sql(T&& t) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                using table_type = reflection::reflection_t<T>;
                constexpr auto field_names = table_type::arr();

                std::string sql("UPDATE");
                utils::string_append(sql, " ", table_type::name().data(), " SET");
                reflection::for_each(t, [&sql, &field_names](auto&, std::size_t idx) {
                    if(idx != 0) {
                        utils::string_append(sql, " ,");
                    }
                    utils::string_append(sql, " ", field_names[idx].data(), "=?");
                });

                std::unordered_set<std::string_view>& primary_key = primary_keys_[table_type::name()];
                bool first = true;
                reflection::for_each(t, [&first, &sql, &field_names, &primary_key](auto&, std::size_t idx) {
                    if(primary_key.count(field_names[idx])) {
                        if(!first) {
                            utils::string_append(sql, " AND");
                        }
                        else {
                            utils::string_append(sql, " WHERE");
                            first = false;
                        }
                        utils::string_append(sql, " ", field_names[idx].data(), "=?");
                    }
                });

                return sql;
            }

            template <typename T>
            bool insert(std::vector<T>&& t) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                for(auto&& tb : t) {
                    if(!this->insert(std::move(tb))) {
                        return false;
                    }
                }
                return true;
            }


            template <typename T, typename... Args>
            bool delete_records(Args&&... args) {
                std::string sql = make_delete_sql<T>(std::forward<Args>(args)...);
                std::cout << sql << std::endl;
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                return stmt->execute(sql);
            }

            int execute(const std::string& sql) {
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                return stmt->execute(sql);
            }

            void begin() {
                conn_->setAutoCommit(false);
            }
            void commit() {
                conn_->commit();
            }
            void rollback() {
                conn_->rollback();
            }
        private:
            template <typename T>
                void query_impl(T& field, std::unique_ptr<sql::ResultSet>& res, std::size_t idx) {
                    using field_type = std::remove_reference_t<decltype(field)>;
                    if constexpr (std::is_same_v<field_type, std::int32_t>) {
                        field = res->getInt(idx + 1);
                    }
                    else if constexpr (std::is_same_v<field_type, std::uint32_t>) {
                        field = res->getUInt(idx + 1);
                    }
                    else if constexpr (utils::is_cpp_array_v<field_type>) {
                        std::string str = res->getString(idx + 1);
                        std::memcpy(&field[0], &str[0], str.length());
                    }
                    else if constexpr (std::is_same_v<field_type, std::string>) {
                        field = res->getString(idx + 1);
                    }
                    else if constexpr (std::is_same_v<field_type, std::chrono::system_clock::time_point>) {
                        field = utils::string_to_time_point(res->getString(idx + 1));
                    }
                    else {
                        throw std::runtime_error(std::string("error field type: ") + typeid(field_type).name());
                    }

                }

            template <typename... Ts, typename... Args>
            std::string make_query_sql(Args&&... args) {
                std::string sql("SELECT * FROM");
                utils::for_each(std::tuple<Ts...>{}, [&sql](auto&& table, std::size_t idx) {
                    using table_type = std::remove_reference_t<decltype(table)>;
                    if(idx != 0) {
                        utils::string_append(sql, " ,");
                    }
                    static_assert(reflection::is_reflection<table_type>::value, "table need to be a reflected type");
                    utils::string_append(sql, " ", reflection::reflection_t<table_type>::name().data());
                }, std::make_index_sequence<sizeof...(Ts)>{});

                utils::for_each(std::make_tuple(args...), [&sql](std::string&& condition, std::size_t idx) {
                    if(idx != 0) {
                        utils::string_append(sql, " AND");
                    }
                    else {
                        utils::string_append(sql, " WHERE");
                    }
                    utils::string_append(sql, " ", condition);
                }, std::make_index_sequence<sizeof...(Args)>{});

                return sql;
            }


            template <typename T>
            std::string make_insert_sql(T&& t, const std::unordered_set<std::string_view>& null_field_set) {
                static_assert(reflection::is_reflection<T>::value, "table needs to be reflected type");
                using table_type = reflection::reflection_t<std::remove_reference_t<decltype(t)>>;
                std::string sql("INSERT INTO");
                utils::string_append(sql, " ", table_type::name().data(), "(");

                constexpr auto fields = table_type::arr();
                bool first = true;
                for(std::size_t i = 0; i != fields.size(); ++i) {
                    if(null_field_set.count(fields[i])) {
                        continue;
                    }
                    if(!first) {
                        utils::string_append(sql, " ,");
                    }
                    else {
                        first = false;
                    }
                    utils::string_append(sql, fields[i].data());
                }
                utils::string_append(sql, ") VALUES(");

                first = true;
                constexpr auto field_count = table_type::count();
                for(std::size_t i = 0; i != field_count; ++i) {
                    if(null_field_set.count(fields[i])) {
                        continue;
                    }
                    if(!first) {
                        utils::string_append(sql, " ,");
                    }
                    else {
                        first = false;
                    }
                    utils::string_append(sql, "?");
                }

                /* reflection::for_each(t, [&sql](auto& field, std::size_t idx) { */
                /*     if(idx != 0) { */
                /*         utils::string_append(sql, " ,"); */
                /*     } */
                /*     utils::string_append(sql, field); */
                /* }); */
                utils::string_append(sql, ")");

                return sql;
            }

            template <typename T, typename... Args>
            std::string make_delete_sql(Args&&... args) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                using table_type = reflection::reflection_t<T>;
                std::string sql("DELETE FROM ");
                utils::string_append(sql, table_type::name().data());

                utils::for_each(std::make_tuple(args...), [&sql](auto&& condition, std::size_t idx) {
                    if(idx != 0) {
                        utils::string_append(sql, " AND");
                    }
                    else {
                        utils::string_append(sql, " WHERE");
                    }
                    utils::string_append(sql, " ", condition);
                }, std::make_index_sequence<sizeof...(Args)>{});

                return sql;
            }
            template <typename T>
            std::string make_delete_sql(T&& t) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                using table_type = reflection::reflection_t<T>;
                constexpr auto table_name = table_type::name();
                constexpr auto field_names = table_type::arr();

                std::string sql("DELETE FROM ");
                utils::string_append(sql, table_name.data());

                bool first = true;
                reflection::for_each(t, [&first, &sql, &field_names, &table_name, this](auto& field, std::size_t idx) {
                    if(primary_keys_[table_name].count(field_names[idx])) {
                        if(!first) {
                            utils::string_append(sql, " AND");
                        }
                        else {
                            utils::string_append(sql, " WHERE");
                            first = false;
                        }
                        utils::string_append(sql, " ", field_names[idx].data(), "=\"", field = "\"");
                    }
                });
                return sql;
            }

            template <typename T, typename... Args>
            std::string make_create_tb_sql(Args&&... args) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                using table_type = reflection::reflection_t<T>;

                std::string sql("CREATE TABLE IF NOT EXISTS ");
                utils::string_append(sql, table_type::name().data(), " (");

                std::string keys;
                std::unordered_set<std::string_view> not_nulls;
                std::unordered_set<std::string_view> auto_increments;
                utils::for_each(std::make_tuple(std::forward<Args>(args)...), [&](auto& item, std::size_t) {
                    using attr_type = std::remove_reference_t<decltype(item)>;
                    if constexpr (attr_type::attr == mysql_attr::key) {
                        for(auto&& field : item.fields) {
                            if(!keys.empty()) {
                                keys.append(1, ',');
                            }
                            utils::string_append(keys, field.data());
                            primary_keys_[table_type::name()].emplace(field);
                        }
                    }
                    else if constexpr (attr_type::attr == mysql_attr::not_null) {
                        for(auto&& field : item.fields) {
                            not_nulls.emplace(std::move(field));
                        }
                    }
                    else if constexpr (attr_type::attr == mysql_attr::auto_increment) {
                        for(auto&& field : item.fields) {
                            auto_increments.emplace(std::move(field));
                        }
                    }
                }, std::make_index_sequence<sizeof...(Args)>{});

                constexpr auto field_names = table_type::arr();
                reflection::for_each(T{}, [&](auto& field, std::size_t idx) {
                    using field_type = std::remove_reference_t<decltype(field)>;
                    if(idx != 0) {
                        utils::string_append(sql, " ,");
                    }
                    utils::string_append(sql, field_names[idx].data(), " ", type_to_name(identity<field_type>{}).data());
                    if(auto_increments.count(field_names[idx])) {
                        utils::string_append(sql, " AUTO_INCREMENT");
                    }
                    if(not_nulls.count(field_names[idx])) {
                        utils::string_append(sql, " NOT NULL");
                    }
                });

                if(!keys.empty()) {
                    utils::string_append(sql, " ,", "PRIMARY KEY(", keys, ")");
                }
                utils::string_append(sql, ")ENGINE=InnoDB DEFAULT CHARSET=utf8");
                return sql;
            }
            std::string make_create_db_sql(std::string_view dbname) {
                std::string sql("CREATE DATABASE IF NOT EXISTS ");
                utils::string_append(sql, dbname.data());
                return sql;
            }
        private:
            sql::Driver* driver_;
            std::unique_ptr<sql::Connection> conn_;

            std::unordered_map<std::string_view, std::unordered_set<std::string_view>> primary_keys_;
    };
}
