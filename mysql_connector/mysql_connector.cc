#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_error.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/connection.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset_metadata.h>

#include <iostream>
#include <string>
#include <memory>

int main()
{
    try {
        sql::Driver* driver = ::get_driver_instance();
        std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "3764819"));
        conn->setSchema("test");

        {
            std::unique_ptr<sql::Statement> stmt(conn->createStatement());
            stmt->execute("DROP TABLE IF EXISTS test");
            stmt->execute("CREATE TABLE test(id VARCHAR(100), time DATETIME)");
        }

        {
            std::cout << "begin insert" << std::endl;
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("INSERT INTO test(id, time) VALUES(?, ?)"));
            for(int i = 0; i != 10; ++i) {
                pstmt->setString(1, std::to_string(i));
                pstmt->setString(2, "2018-5-20 23:31");
                pstmt->executeUpdate();
            }

            std::cout << "done" << std::endl;
        }

        {
            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("SELECT * FROM test"));
            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            auto rsmd = res->getMetaData();
            res->afterLast();
            while(res->previous()) {
                if(rsmd->getColumnType(1) == sql::DataType::VARCHAR) {
                    std::cout << "is string" << std::endl;
                }
                std::cout << "\t... MySQL counts: " << res->getString("id") << " " << res->getString(2) << std::endl;
            }
        }
    }
    catch(sql::SQLException& e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __func__ << ") on line " << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
    }
    return 0;
    /* try { */
    /*     sql::Driver *driver = get_driver_instance(); */
    /*     std::unique_ptr<sql::Connection> conn(driver->connect("tcp://127.0.0.1:3306", "root", "3764819")); */
    /*     conn->setSchema("test"); */
    /*     std::unique_ptr<sql::Statement> stmt(conn->createStatement()); */

    /*     std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 'HELLO WORLD!' AS message")); */
    /*     while(res->next()) { */
    /*         std::cout << "\t... MySQL replies: "; */
    /*         std::cout << res->getString("message") << std::endl; */
    /*         std::cout << "\t... MySQL says it again: "; */
    /*         std::cout << res->getString(1) << std::endl; */
    /*     } */
    /* } */
    /* catch(sql::SQLException& e) { */
    /*     std::cout << "# ERR: SQLException in " << __FILE__; */
    /*     std::cout << "(" << __func__ << ") on line " << __LINE__ << std::endl; */
    /*     std::cout << "# ERR: " << e.what(); */
    /*     std::cout << " (MySQL error code: " << e.getErrorCode(); */
    /*     std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl; */
    /* } */

    return 0;
}

