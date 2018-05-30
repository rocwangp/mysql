#include "database_algorithm.hpp"

int main(int argc, char* argv[])
{
    /* selection::linear_search(10000, 20000); */
    /* sort_algorithm::n_merge_sort(10000, 16); */
    /* std::cout << "\n\n"; */
    /* sort_algorithm::n_merge_sort(20000, 32); */

    if(argc != 5) {
        algorithm::selection::binary_search(10000, 16, 40, 50000);
        algorithm::selection::binary_search(20000, 32, 60, 60000);
    }
    std::int32_t base_addr = std::atoi(argv[1]);
    std::int32_t block_num = std::atoi(argv[2]);
    std::int32_t target = std::atoi(argv[3]);
    std::int32_t write_addr = std::atoi(argv[4]);
    std::cout << base_addr << " " << block_num << " " << target << " " << write_addr << "\n";
    /* selection::binary_search(10000, 16, 40); */
    algorithm::selection::binary_search(base_addr, block_num, target, write_addr);
    return 0;
}
