#include "database_algorithm.hpp"
#include <unistd.h>

const char* opt = "a:n:v:t:l:r:gbmch";

enum class operation
{
    gd,
    bs,
    ms,
    gc
};

int main(int argc, char* argv[])
{
    /* std::int32_t base_addr{ 0 }; */
    /* std::int32_t target_addr{ 0 }; */
    /* std::int32_t block_num{ 0 }; */
    /* std::int32_t search_value{ 0 }; */
    /* std::int32_t addr1{ 0 }; */
    /* std::int32_t addr2{ 0 }; */

    /* operation op; */
    /* int oc; */
    /* while((oc = ::getopt(argc, argv, opt)) != -1) { */
    /*     switch(oc) { */
    /*         case 'a': */
    /*             base_addr = std::atoi(optarg); */
    /*             break; */
    /*         case 'n': */
    /*             block_num = std::atoi(optarg); */
    /*             break; */
    /*         case 'v': */
    /*             search_value = std::atoi(optarg); */
    /*             break; */
    /*         case 't': */
    /*             target_addr = std::atoi(optarg); */
    /*             break; */
    /*         case 'l': */
    /*             addr1 = std::atoi(optarg); */
    /*             break; */
    /*         case 'r': */
    /*             addr2 = std::atoi(optarg); */
    /*             break; */
    /*         case 'g': */
    /*             op = operation::gd; */
    /*             break; */
    /*         case 'b': */
    /*             op = operation::bs; */
    /*             break; */
    /*         case 'm': */
    /*             op = operation::ms; */
    /*             break; */
    /*         case 'c': */
    /*             op = operation::gc; */
    /*             break; */
    /*         case 'h': */
    /*             std::printf("g: data_generator(t, n)\nb: binary_search(a, n, v, t)\nm: n_merge_sort(a, n)\nc: get_convergence(l, r)\n"); */
    /*             std::printf("a: base_addr\nn: block_num\nv: search_value\nt: target_addr\nl: addr1\nr: addr2\n"); */
    /*             return 1; */
    /*     } */
    /* } */

    algorithm::gen_block_data(10000, 16, 64, 1, 40, 1, 10);
    algorithm::gen_block_data(20000, 32, 64, 20, 61, 1, 10);
    algorithm::sorting::n_merge_sort(10000, 16);
    algorithm::sorting::n_merge_sort(20000, 32);
    /* algorithm::collection::get_convergence(10000, 20000); */
    /* algorithm::collection::get_intersection(10000, 20000); */
    /* algorithm::collection::get_complement(10000, 20000); */
    algorithm::connection::nest_loop_join(10000, 20000);
    algorithm::connection::sort_merge_join(10000, 20000);

    /* switch(op) { */
    /*     case operation::gd: */
    /*         algorithm::gen_block_data(target_addr, block_num, 64, 1, 41, 1, 10001); */
    /*         break; */
    /*     case operation::bs: */
    /*         algorithm::selection::binary_search(base_addr, block_num, search_value, target_addr); */
    /*         break; */
    /*     case operation::ms: */
    /*         algorithm::sorting::n_merge_sort(base_addr, block_num); */
    /*         break; */
    /*     case operation::gc: */
    /*         algorithm::collection::get_convergence(addr1, addr2); */
    /*         break; */
    /*     default: */
    /*         ; */
    /* } */
    /* selection::linear_search(10000, 20000); */
    /* sort_algorithm::n_merge_sort(10000, 16); */
    /* std::cout << "\n\n"; */
    /* sort_algorithm::n_merge_sort(20000, 32); */

    /* if(argc != 5) { */
    /*     std::printf("using default configure.............\n"); */
    /*     std::printf("binary search relation R with target is 40...............\n"); */
    /*     algorithm::selection::binary_search(10000, 16, 40, 50000); */
    /*     std::printf("....................................\n"); */
    /*     std::printf("binary search relation S with target is 60...............\n"); */
    /*     algorithm::selection::binary_search(20000, 32, 60, 60000); */
    /*     return 0; */
    /* } */
    /* std::int32_t base_addr = std::atoi(argv[1]); */
    /* std::int32_t block_num = std::atoi(argv[2]); */
    /* std::int32_t target = std::atoi(argv[3]); */
    /* std::int32_t write_addr = std::atoi(argv[4]); */
    /* std::printf("binary search block [%d, %d] with target is 60...............\n", base_addr, base_addr + block_num - 1); */
    /* algorithm::selection::binary_search(base_addr, block_num, target, write_addr); */
    return 0;
}
