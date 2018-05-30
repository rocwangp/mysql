#pragma once
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <cstring>

#include "extmem-c/extmem.h"


namespace sort_algorithm
{

    struct block
    {
        block() = default;

        block(std::size_t s, std::size_t c, std::size_t e)
            : start(s), cur(c), end(e)
        { }

        void read_from_disk(::Buffer* buffer) {
            /* std::cout << cur << "\n"; */
            blk = ::readBlockFromDisk(cur, buffer, "./block/");
        }
        bool update(::Buffer* buffer) {
            read_bytes += 8;
            if(read_bytes == 56) {
                ::freeBlockInBuffer(blk, buffer);
                if(cur != end) {
                    ++cur;
                    read_bytes = 0;
                    /* std::cout << "move " << cur - 1 << " to " << cur << "\n"; */
                    blk = ::readBlockFromDisk(cur, buffer, "./block/");
                    return true;
                }
                return false;
            }
            return true;
        }
        std::int64_t read_int64_t() {
            std::int64_t data = *(std::int64_t*)(blk + read_bytes);
            return data;
        }
        unsigned char* blk{ nullptr };
        std::size_t read_bytes{ 0 };
        std::size_t start;
        std::size_t cur;
        std::size_t end;
    };

    struct block_compare
    {
        bool operator()(const block& blk1, const block& blk2) {
            return *(std::int32_t*)(blk1.blk + blk1.read_bytes) > *(std::int32_t*)(blk2.blk + blk2.read_bytes);
        }
    };

    void n_merge_sort(std::size_t base_addr, std::size_t block_num) {
        static constexpr std::size_t buffer_size = 520;
        static constexpr std::size_t block_size = 64;
        static constexpr std::size_t readable_block = 8;

        // step0: init buffer
        ::Buffer buffer;
        ::initBuffer(buffer_size, block_size, &buffer);

        // step1: sort every block
        for(std::size_t i = 0; i != block_num; ++i) {
            unsigned char* blk = ::readBlockFromDisk(base_addr + i, &buffer, "./block/");
            std::int64_t* p = (std::int64_t*)(blk);
            std::sort(p, p + 8, [](std::int64_t n1, std::int64_t n2) {
                std::int32_t n11 = *(std::int32_t*)((char*)(&n1));
                std::int32_t n21 = *(std::int32_t*)((char*)(&n2));
                return n11 < n21;
            });
            /* std::cout << "\n"; */
            ::writeBlockToDisk(blk, base_addr + i, &buffer, "./block/");
        }

        // step2: merge every sorted block unit
        std::vector<block> blocks;
        blocks.reserve(block_num);
        // step2.0: init 16 sorted block unit, every unit has only one block
        for(std::size_t i = 0; i != block_num; ++i) {
            blocks.emplace_back(base_addr + i, base_addr + i, base_addr + i);
        }

        /* std::cout << "start sort all block" << std::endl; */
        while(blocks.size() != 1) {
            // step2.1: calculate block count could be read,
            //          max count is 7 because there is one block to store sorted data
            std::size_t read_block = std::min(readable_block - 1, blocks.size());
            /* std::cout << "blocks size: " << blocks.size() << " " << "read_block: " << read_block << std::endl; */

            // step2.2: calculate how many counts it cost to sort
            //          16 block unit need 3 counts: 7, 7, 2
            std::size_t read_cnt = blocks.size() / read_block;
            if(blocks.size() % read_block) {
                ++read_cnt;
            }
            /* std::cout << "will read " << read_cnt << " times\n"; */
            for(std::size_t cnt = 0; cnt != read_cnt; ++cnt) {
                // step2.3: init min_heap to sort block every time
                //          [0: 7), [7, 14), [14, 16) for above example
                std::priority_queue<block, std::vector<block>, block_compare> pq;
                for(std::size_t i = 0; i != read_block && cnt * read_block + i < blocks.size(); ++i) {
                    blocks[cnt * read_block + i].read_from_disk(&buffer);
                    pq.push(blocks[cnt * read_block + i]);
                }
                // step2.4: sort block unit in min_heap
                std::size_t write_addr = 40000;
                std::size_t write_cnt = 0;
                std::size_t write_bytes = 0;
                unsigned char* write_blk{ nullptr };
                /* std::cout << "heap size: " << pq.size() << std::endl; */
                while(!pq.empty()) {
                    if(write_blk == nullptr) {
                        write_blk = ::getNewBlockInBuffer(&buffer);
                    }
                    block b = pq.top();
                    pq.pop();
                    // read int64_t data from block
                    *(std::int64_t*)(write_blk + write_bytes) = b.read_int64_t();
                    write_bytes += 8;
                    if(write_bytes == 56) {
                        /* *(std::int32_t*)(write_blk + write_bytes) = blocks[cnt * read_block].start + write_cnt + 1; */
                        // write to a new position to store sorted data
                        /* std::cout << "write sorted block to " << write_addr + write_cnt << std::endl; */
                        ::writeBlockToDisk(write_blk, write_addr + write_cnt, &buffer, "./block/");
                        write_bytes = 0;
                        ++write_cnt;
                        write_blk = nullptr;
                    }
                    // if the block b is over, the update will return false
                    if(b.update(&buffer)) {
                        pq.emplace(std::move(b));
                    }
                }
                // rewrite the block
                std::size_t index = 0;
                for(std::size_t i = 0; i != read_block && cnt * read_block + i < blocks.size(); ++i) {
                    for(std::size_t j = blocks[cnt * read_block + i].start; j <= blocks[cnt * read_block + i].end; ++j) {
                        /* std::cout << "rewrite the block: " << write_addr + index << " to " << j << std::endl; */
                        unsigned char* sorted_blk = ::readBlockFromDisk(write_addr + index++, &buffer, "./block/");
                        ::writeBlockToDisk(sorted_blk, j, &buffer, "./block/");
                    }
                }
            }
            // step3: modify blocks, merge sorted blocks to one block unit
            //        [0: 7), [7, 14), [14, 16) for above example, now it has 3 block unit
            read_block = std::min(readable_block - 1, blocks.size());
            std::vector<block> new_blocks(read_cnt);
            for(std::size_t i = 0; i != read_cnt; ++i) {
                new_blocks[i].start = blocks[i * read_block].start;
                new_blocks[i].cur = new_blocks[i].start;
                new_blocks[i].end = blocks[std::min(blocks.size() - 1, i * read_block + read_block - 1)].end;
            }
            blocks.swap(new_blocks);

            /* std::cout << "loop once" << std::endl; */
        }
    }
}
