#include <iostream>
#include <random>
#include <algorithm>

#include "extmem-c/extmem.h"

void gen_block_data(std::size_t base_addr, std::size_t block_num, std::size_t block_size,
    std::size_t left_bound1, std::size_t right_bound1, std::size_t left_bound2, std::size_t right_bound2) {
    ::Buffer buffer;
    if(!::initBuffer((2 * block_num) * block_size, block_size, &buffer)) {
        std::cerr << "init buffer error";
        return;
    }
    std::random_device rd{};

    unsigned char* blk{ nullptr };
    for(std::size_t i = 0; i < block_num; ++i) {
        blk = ::getNewBlockInBuffer(&buffer);
        for(std::size_t j = 0; j < 7; ++j) {
            *(std::int32_t*)(blk + 8 * j) = (rd() % right_bound1) + left_bound1;
            *(std::int32_t*)(blk + 8 * j + 4) = (rd() % right_bound2) + left_bound2;
            std::cout << *(std::int32_t*)(blk + 8 * j) << " " << *(std::int32_t*)(blk + 8 * j + 4) << std::endl;
        }
        *(std::size_t*)(blk + 8 * 7) = base_addr + i + 1;
        std::cout << *(std::size_t*)(blk + 8 * 7) << std::endl;
        ::writeBlockToDisk(blk, base_addr + i, &buffer, "./block/");
    }

    ::freeBuffer(&buffer);
    /* ::initBuffer(20 * 64, 64, &buffer); */

    /* for(std::size_t i = 0; i < 16; ++i) { */
    /*     blk = ::readBlockFromDisk(base_addr + i, &buffer); */
    /*     for(std::size_t j = 0; j < 8; ++j) { */
    /*         std::cout << *(std::int32_t*)(blk + 4 * j) << " "; */
    /*     } */
    /*     std::cout << "\n"; */
    /* } */
    /* ::freeBuffer(&buffer); */
}

int main()
{
    ::srand((unsigned int)(::time(nullptr)));
    gen_block_data(10000, 16, 64, 1, 40, 1, 1000);
    gen_block_data(20000, 32, 64, 20, 60, 1, 1000);
    return 0;
}
