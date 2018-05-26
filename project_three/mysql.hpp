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
#include "sql.hpp"

namespace mysql
{
    class MySQL
    {
        public:
            MySQL(std::string_view ip, std::string_view user, std::string_view passwd, std::string_view dbname)
                : driver_(::get_driver_instance()),
                  conn_(driver_->connect(ip.data(), user.data(), passwd.data()))
            {
                {
                    std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                    stmt->execute(make_create_db_sql(dbname));
                }
                conn_->setSchema(dbname.data());
            }

            template <typename T, typename... Args>
            void bind_attribute(Args... args) {
                static_assert(reflection::is_reflection<T>::value, "table need to be reflected type");
                auto table_name = reflection::reflection_t<T>::name();
                utils::for_each(std::make_tuple(std::forward<Args>(args)...), [&, this](auto& t, std::size_t) {
                    for(auto name : t.fields) {
                        if(t.attr == attribute::key) {
                            this->primary_keys_[table_name].emplace(std::move(name));
                        }
                        else if(t.attr == attribute::not_null) {
                            this->not_nulls_[table_name].emplace(std::move(name));
                        }
                        else {
                            this->auto_increments_[table_name].emplace(std::move(name));
                        }
                    }
                }, std::make_index_sequence<sizeof...(Args)>{});
            }

            template <typename T, typename... Args>
            bool create_table(Args&&... args) {
                delete_table<T>();
                constexpr auto table_name = reflection::reflection_t<T>::name();
                utils::for_each(std::make_tuple(std::forward<Args>(args)...), [&, this](auto& t, std::size_t) {
                    for(auto&& name : t.fields) {
                        if(t.attr == attribute::key) {
                            this->primary_keys_[table_name].emplace(std::move(name));
                        }
                        else if(t.attr == attribute::not_null) {
                            this->not_nulls_[table_name].emplace(std::move(name));
                        }
                        else {
                            this->auto_increments_[table_name].emplace(std::move(name));
                        }
                    }
                }, std::make_index_sequence<sizeof...(Args)>{});
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                return stmt->execute(make_create_tb_sql<T>(primary_keys_, not_nulls_, auto_increments_));
            }
            template <typename T>
            bool delete_table() {
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                return stmt->execute(make_delete_tb_sql<T>());
            }

            template <typename Tuple>
            std::vector<Tuple> query_execute(std::string&& sql) {
                static_assert(utils::is_tuple_v<Tuple>, "template argument need a std::tuple type");
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(std::move(sql)));

                std::vector<Tuple> query_results;
                while(res->next()) {
                    Tuple t;
                    std::size_t base_idx { 0 };
                    utils::for_each(t, [&](auto& field, std::size_t idx) {
                        idx += base_idx;
                        using field_type = std::remove_reference_t<decltype(field)>;
                        if constexpr (reflection::is_reflection<field_type>::value) {
                            reflection::for_each(field, [&](auto& item, std::size_t i) {
                                query_impl(item, res, idx + i);
                            });
                            base_idx += reflection::reflection_t<field_type>::count() - 1;
                        }
                        else {
                            query_impl(field, res, idx);
                        }
                    }, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
                    query_results.emplace_back(std::move(t));
                }
                return query_results;
            }

            template <typename... Ts, typename... Args>
            std::enable_if_t<(!utils::is_tuple_v<std::tuple_element_t<0, std::tuple<Ts...>>>),
            std::conditional_t<(sizeof...(Ts)>1), std::vector<std::tuple<Ts...>>, std::vector<Ts...>>>
            query(Args&&... args) {
                rename_map renames{};
                query_field_set query_field{};
                utils::for_each(std::make_tuple(args...), [&](auto item, std::size_t) {
                    using item_type = std::remove_reference_t<decltype(item)>;
                    if constexpr (std::is_same_v<item_type, rename_map>) {
                        renames = std::move(item);
                    }
                    else if constexpr (std::is_same_v<item_type, query_field_set>) {
                        query_field = std::move(item);
                    }
                }, std::make_index_sequence<sizeof...(Args)>{});

                auto filter = black_magic::type_filter<rename_map, query_field_set>::filter(std::forward<Args>(args)...);
                std::string sql(make_query_sql<Ts...>(std::move(renames), std::move(query_field), std::move(filter)));
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(std::move(sql)));

                if constexpr (sizeof...(Ts) == 1) {
                    using T = std::tuple_element_t<0, std::tuple<Ts...>>;
                    std::vector<T> query_results;
                    while(res->next()) {
                        T t{};
                        reflection::for_each(t, [&](auto& field, std::size_t idx) {
                            query_impl(field, res, idx);
                        });
                        query_results.emplace_back(std::move(t));
                    }
                    return query_results;
                }
                else {
                    std::vector<std::tuple<Ts...>> query_results;
                    while(res->next()) {
                        std::tuple<Ts...> t;
                        std::size_t base_idx{ 0 };
                        utils::for_each(t, [&](auto& tb, std::size_t) {
                            reflection::for_each(tb, [&](auto& field, std::size_t idx) {
                                query_impl(field, res, base_idx + idx);
                            });
                            base_idx += reflection::reflection_t<std::remove_reference_t<decltype(tb)>>::count();
                        }, std::make_index_sequence<sizeof...(Ts)>{});
                        query_results.emplace_back(std::move(t));
                    }
                    return query_results;
                }
            }

            template <typename Tuple, typename... Ts, typename... Args>
            std::enable_if_t<utils::is_tuple_v<Tuple>, std::vector<Tuple>>
            query(Args&&... args) {
                rename_map renames{};
                query_field_set query_field{};
                utils::for_each(std::make_tuple(args...), [&](auto&& item, std::size_t) {
                    using item_type = std::remove_reference_t<decltype(item)>;
                    if constexpr (std::is_same_v<item_type, rename_map>) {
                        renames = std::move(item);
                    }
                    else if constexpr (std::is_same_v<item_type, query_field_set>) {
                        query_field = std::move(item);
                    }
                }, std::make_index_sequence<sizeof...(Args)>{});

                auto filter = black_magic::type_filter<rename_map, query_field_set>::filter(std::forward<Args>(args)...);
                std::string sql(make_query_sql<Ts...>(std::move(renames), std::move(query_field), std::move(filter)));
                return query_execute<Tuple>(std::move(sql));
            }

            template <typename T>
            std::enable_if_t<reflection::is_reflection<T>::value, bool>
            insert(T&& t, std::unordered_set<std::string_view>&& null_field_set = std::unordered_set<std::string_view>{}) {
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                stmt->executeUpdate(make_insert_sql(std::forward<T>(t), std::move(null_field_set)));
            }

            template <typename T>
            bool update(T&& t, update_field_set&& update_field = update_field_set{}) {
                std::string sql = make_update_sql(std::forward<T>(t), primary_keys_, std::forward<update_field_set>(update_field));
                std::unique_ptr<sql::Statement> stmt(conn_->createStatement());
                return stmt->executeUpdate(sql);

            }

            template <typename T, typename... Args>
            bool remove(Args&&... args) {
                std::string sql = make_remove_sql<T>(std::forward<Args>(args)..., primary_keys_);
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
        private:
            sql::Driver* driver_;
            std::unique_ptr<sql::Connection> conn_;

            primary_key_map primary_keys_;
            not_null_map not_nulls_;
            auto_increment_map auto_increments_;
    };
}
