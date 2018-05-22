#include "../mysql.hpp"
#include "entity.hpp"



int main()
{
    mysql::MySQL mysql("tcp://127.0.0.1", "root", "3764819", "test");

    mysql.insert(u);
    return 0;
}
