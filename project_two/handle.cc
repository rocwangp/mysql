#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_error.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
int main(int argc, char* argv[])
{
    const char* arg_opt = "q:p:";
    int id = 0;
    std::string params;

    int oc;
    while((oc = ::getopt(argc, argv, arg_opt)) != -1) {
        switch(oc) {
            case 'q':
                id = std::strtol(optarg, nullptr, 10);
                break;
            case 'p':
                params.assign(optarg, ::strlen(optarg));
                break;
            default:
                ;
        }
    }
    std::cout << id << " " << params << std::endl;
    try {
        sql::Driver* driver = get_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1", "root", "3764819"));
        conn->setSchema("COMPANY");

        if(id == 1) {
            std::stringstream oss;
            oss << "SELECT EMPLOYEE.ESSN FROM EMPLOYEE, WORKS_ON WHERE "
                << "EMPLOYEE.ESSN = WORKS_ON.ESSN AND WORKS_ON.PNO = ?";
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            pstmt->setString(1, params);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << std::endl;
            }
        }
        else if(id == 2) {
            std::stringstream oss;
            oss << "SELECT EMPLOYEE.ENAME "
                << "FROM EMPLOYEE, WORKS_ON, PROJECT "
                << "WHERE EMPLOYEE.ESSN = WORKS_ON.ESSN "
                << "AND WORKS_ON.PNO = PROJECT.PNO "
                << "AND PROJECT.PNAME = ?";
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            pstmt->setString(1, params);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << std::endl;
            }
        }
        else if(id == 3) {
            std::stringstream oss;
            oss << "SELECT EMPLOYEE.ENAME, EMPLOYEE.ADDRESS "
                << "FROM EMPLOYEE, DEPARTMENT "
                << "WHERE EMPLOYEE.DNO = DEPARTMENT.DNO AND DEPARTMENT.DNAME = ?";
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            pstmt->setString(1, params);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << " " << res->getString(2) << std::endl;
            }
        }
        else if(id == 4) {
            std::stringstream oss;
            oss << "SELECT EMPLOYEE.ENAME, EMPLOYEE.ADDRESS "
                << "FROM EMPLOYEE, DEPARTMENT "
                << "WHERE EMPLOYEE.DNO = DEPARTMENT.DNO "
                << "AND DEPARTMENT.DNAME = ? "
                << "AND EMPLOYEE.SALARY < ?";

            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            auto pos = params.find_last_of(' ');
            int salary = std::strtol(params.data() + pos + 1, nullptr, 10);
            auto dname = params.substr(0, pos);
            pstmt->setString(1, dname);
            pstmt->setInt(2, salary);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << " " << res->getString(2) << std::endl;
            }
        }
        else if(id == 5) {
            std::stringstream oss;
            oss << "SELECT E1.ENAME "
                << "FROM EMPLOYEE AS E1 "
                << "WHERE E1.ENAME NOT IN( "
                << "SELECT E2.ENAME "
                << "FROM EMPLOYEE AS E2, WORKS_ON "
                << "WHERE E2.ESSN = WORKS_ON.ESSN AND WORKS_ON.PNO = ?)";
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            pstmt->setString(1, params);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << std::endl;
            }
        }
        else if(id == 6) {

            std::stringstream oss;
            oss << "SELECT E1.ENAME, DEPARTMENT.DNAME "
                << "FROM EMPLOYEE AS E1, DEPARTMENT "
                << "WHERE E1.DNO = DEPARTMENT.DNO "
                << "AND E1.SUPERSSN = ("
                << "SELECT E2.SUPERSSN "
                << "FROM EMPLOYEE AS E2 "
                << "WHERE E2.ENAME = ?)";
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            pstmt->setString(1, params);
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << " " << res->getString(2) << std::endl;
            }
        }
        else if(id == 7) {
            std::string sql(
            "\
                SELECT \
                    DISTINCT EMPLOYEE.ESSN \
                FROM  \
                    EMPLOYEE, \
                    WORKS_ON, \
                    (SELECT EMPLOYEE.ESSN FROM EMPLOYEE, WORKS_ON WHERE \
                     EMPLOYEE.ESSN = WORKS_ON.ESSN AND \
                     WORKS_ON.PNO = ?) AS A \
                WHERE \
                    EMPLOYEE.ESSN = WORKS_ON.ESSN AND \
                    WORKS_ON.PNO = ? AND \
                    WORKS_ON.ESSN IN (A.ESSN) \
            ");
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(sql));
            auto pos = params.find_first_of(' ');
            pstmt->setString(1, params.substr(0, pos));
            pstmt->setString(2, params.substr(pos + 1));
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << std::endl;
            }
        }
        else if(id == 8) {
            std::stringstream oss;
            oss << "SELECT DNAME FROM DEPARTMENT "
                << "WHERE DEPARTMENT.DNO IN ("
                << "SELECT EMPLOYEE.DNO FROM EMPLOYEE "
                << "GROUP BY(DNO) "
                << "HAVING AVG(SALARY) < ?)";
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            pstmt->setInt(1, std::strtoll(params.data(), nullptr, 10));
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << std::endl;
            }
        }
        else if(id == 9) {
            std::stringstream oss;
            oss << "SELECT EMPLOYEE.ENAME FROM EMPLOYEE, WORKS_ON "
                << "WHERE EMPLOYEE.ESSN = WORKS_ON.ESSN "
                << "GROUP BY(EMPLOYEE.ESSN) "
                << "HAVING COUNT(PNO) >= ? AND SUM(HOURS) <= ?";

            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(oss.str()));
            auto pos = params.find_first_of(' ');
            pstmt->setInt(1, std::strtoll(params.substr(0, pos).data(), nullptr, 10));
            pstmt->setInt(2, std::strtoll(params.substr(pos + 1).data(), nullptr, 10));
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            while(res->next()) {
                std::cout << res->getString(1) << std::endl;
            }
        }
        else {
            std::cout << "error params" << std::endl;
        }
    }
    catch(...) {

    }
    return 0;
}
