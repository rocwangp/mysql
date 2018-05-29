#pragma once

#include <iostream>
#include <functional>
#include <cstring>

#include "extmem-c/extmem.h"

namespace selection
{
    void linear_search(std::size_t base_addr1, std::size_t base_addr2) {
        static constexpr std::size_t buffer_size = 520;
        static constexpr std::size_t block_size = 64;

        ::Buffer buffer;
        ::initBuffer(buffer_size, block_size, &buffer);
        unsigned char* out_blk = ::getNewBlockInBuffer(&buffer);

        std::size_t out_addr = 30000;
        std::size_t write_bytes = 0;
        auto handler = [&](std::int32_t addr, std::size_t block_count, std::function<bool(int, int)>&& f) {
            std::size_t read_blocks = 0;
            while(read_blocks != block_count) {
                unsigned char* in_blk = ::readBlockFromDisk(addr, &buffer, "./block/");
                for(std::size_t i = 0; i != 7; ++i) {
                    std::int32_t n1 = *(std::int32_t*)(in_blk + 8 * i);
                    std::int32_t n2 = *(std::int32_t*)(in_blk + 8 * i + 4);
                    if(f(n1, n2)) {
                        *(std::int32_t*)(out_blk + write_bytes) = n1;
                        *(std::int32_t*)(out_blk + write_bytes + 4) = n2;
                        std::cout << n1 << " " << n2 << std::endl;
                        write_bytes += 8;
                        if(write_bytes == 56) {
                            *(std::int32_t*)(out_blk + write_bytes) = out_addr + 1;
                            ::writeBlockToDisk(out_blk, out_addr, &buffer, "./block/");
                            out_blk = ::getNewBlockInBuffer(&buffer);
                            write_bytes = 0;
                            ++out_addr;
                        }
                    }
                }
                addr = *(std::int32_t*)(in_blk + 8 * 7);
                ++read_blocks;
                ::freeBlockInBuffer(in_blk, &buffer);
            }
        };

        auto f1 = [](int a, int) { return a == 40; };
        auto f2 = [](int c, int) { return c == 60; };
        handler(base_addr1, 16, std::move(f1));
        handler(base_addr2, 32, std::move(f2));
        if(write_bytes != 0) {
            std::cout << out_addr << std::endl;
            ::writeBlockToDisk(out_blk, out_addr, &buffer, "./block/");
        }
    }
}
