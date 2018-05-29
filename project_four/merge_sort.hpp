#pragma once
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include "extmem-c/extmem.c"


namespace sort_algorithm
{

    void sort_block(unsigned char* blk) {
        std::int64_t* p = (std::int64_t*)(blk);
        std::sort(p, (std::int64_t*)((unsigned char*)(p) + 8 * 7), [](std::int64_t n1, std::int64_t n2) {
            std::int32_t n11 = *(std::int32_t*)((char*)(n1));
            std::int32_t n21 = *(std::int32_t*)((char*)(n2));
            return n11 < n21;
        });
    }

    void n_merge_sort(std::size_t n, std::size_t base_addr1, std::size_t base_addr2) {
        static constexpr std::size_t buffer_size = 520;
        static constexpr std::size_t block_size = 64;

        ::Buffer buffer;
        ::initBuffer(2 * block_size, block_size, &buffer);
        std::size_t addr = base_addr1;
        for(std::size_t i = 0; i != 16; ++i) {
            unsigned char* blk = ::readBlockFromDisk(addr + i, &buffer, "./block/");
            sort_block(blk);
            ::writeBlockToDisk(blk, addr + i, &buffer, "./block/");
        }

        ::freeBuffer(&buffer);

        ::initBuffer(buffer_size, block_size, &buffer);

        struct block
        {
            std::list<unsigned char*> ptrs;
            std::size_t read_bytes{ 0 };
        };

        struct block_compare
        {
            bool operator()(block blk1, block blk2) {
                return *(std::int32_t*)(blk1.ptrs.front() + blk1.read_bytes) < *(std::int32_t*)(blk2.ptrs.front() + blk2.read_bytes);
            }
        };

        auto handler = [&](std::size_t step) {
            for(std::size_t i = 0; i != 16; i += 2) {
                std::priority_queue<block, std::vector<block>, block_compare> pq;
                unsigned char* blk = ::getNewBlockInBuffer(&buffer);
                std::size_t write_bytes = 0;
                std::size_t write_blocks = 0;
                for(std::size_t j = 0; j != 2; ++j) {
                    block b;
                    for(std::size_t k = 0; k != step; ++k) {
                        b.ptrs.emplace_back(::readBlockFromDisk(addr + i + j + k, &buffer, "./block/"));
                    }
                    pq.push(b);
                }
                while(!pq.empty()) {
                    block b = pq.top();
                    pq.pop();
                    *(std::int64_t*)(blk + write_bytes) = *(std::int64_t*)(b.ptrs.front());
                    write_bytes += 8;
                    if(write_bytes == 56) {
                        ::writeBlockToDisk(blk, addr + i + write_blocks++, &buffer, "./block/");
                        write_bytes = 0;
                        blk = ::getNewBlockInBuffer(&buffer);
                    }
                    if(b.read_bytes != 56) {
                        b.ptrs.front() += 8;
                        pq.push(b);
                    }
                    else {
                        b.ptrs.pop_front();
                        if(!b.ptrs.empty()) {
                            pq.push(b);
                        }
                    }
                }
            }
        };
        handler(1);
        handler(2);
        handler(4);
        handler(8);
    }
}
