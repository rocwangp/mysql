#include "../mysql.hpp"
struct EMPLOYEE
{
    std::string ENAME;
    std::string ESSN;
    std::string ADDRESS;
    std::uint32_t SALARY;
    std::string SUPERSSN;
    std::string DNO;
};
REFLECTION(EMPLOYEE, ENAME, ESSN, ADDRESS, SALARY, SUPERSSN, DNO)

struct WORKS_ON
{
    std::string ESSN;
    std::string PNO;
    std::uint32_t HOURS;
};
REFLECTION(WORKS_ON, ESSN, PNO, HOURS)

struct PROJECT
{
    std::string PNAME;
    std::string PNO;
    std::string PLOCATION;
    std::string DNO;
};
REFLECTION(PROJECT, PNAME, PNO, PLOCATION, DNO)


template <typename T>
void single_query(mysql::MySQL& mysql) {
    auto query_results = mysql.query<T>();
    for(auto&& tb : query_results) {
        reflection::for_each(tb, [](auto& field, std::size_t) {
            std::cout << field << "\t";
        });
        std::cout << "\n";
    }
}

template <typename T, typename... Args>
void single_query_with_condition(mysql::MySQL& mysql, Args... args) {
    auto query_results = mysql.query<T>(args...);
    for(auto&& tb : query_results) {
        reflection::for_each(tb, [](auto& field, std::size_t) {
            std::cout << field << "\t";
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
void multi_query_with_condition(mysql::MySQL& mysql, Args... args) {
    auto query_results_tp = mysql.query<Ts...>(args...);
    /* auto query_results_tp = */
    /*     mysql.query<EMPLOYEE, WORKS_ON, PROJECT>("EMPLOYEE.ESSN=WORKS_ON.ESSN","WORKS_ON.PNO=PROJECT.PNO", "PROJECT.PNAME=\"SQL Project\""); */
    for(auto& result_tp : query_results_tp) {
        utils::for_each(result_tp, [](auto& tb, std::size_t ) {
            reflection::for_each(tb, [](auto& item, std::size_t ) {
                std::cout << item << " ";
            });
        }, std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<decltype(result_tp)>>>{});
        std::cout << "\n";
    }
}

void multi_query_with_field(mysql::MySQL& mysql) {
    std::string sql("SELECT ENAME FROM EMPLOYEE, WORKS_ON, PROJECT WHERE \
                    EMPLOYEE.ESSN = WORKS_ON.ESSN  AND \
                    WORKS_ON.PNO = PROJECT.PNO AND \
                    PROJECT.PNAME = \"SQL Project\"");
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

    mysql::MySQL mysql{"tcp://127.0.0.1", "root", "3764819", "test"};
    /* mysql.create_table<EMPLOYEE>(); */
    /* mysql.create_table<WORKS_ON>(); */
    /* mysql.create_table<PROJECT>(); */

    /* EMPLOYEE employee{ "1", "2", "3", 123456, "4", "5" }; */
    /* WORKS_ON works_on{ "2", "2", 10 }; */
    /* mysql.insert(employee, works_on); */

    mysql.delete_records<EMPLOYEE>("ESSN=\"2\"");
    single_query<EMPLOYEE>(mysql);

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

