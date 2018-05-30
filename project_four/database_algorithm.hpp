#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <random>
#include <cstring>

#include "extmem-c/extmem.c"

namespace algorithm
{

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
            for(std::size_t j = 0; j != 7; ++j) {
                *(std::int32_t*)(blk + 8 * j) = (rd() % (right_bound1 - left_bound1)) + left_bound1;
                *(std::int32_t*)(blk + 8 * j + 4) = (rd() % (right_bound2 - left_bound2)) + left_bound2;
                /* std::cout << *(std::int32_t*)(blk + 8 * j) << " " << *(std::int32_t*)(blk + 8 * j + 4) << std::endl; */
            }
            *(std::size_t*)(blk + 8 * 7) = base_addr + i + 1;
            ::writeBlockToDisk(blk, base_addr + i, &buffer, "./block/");
        }
        ::freeBuffer(&buffer);
    }

    namespace sorting
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
            ::freeBuffer(&buffer);
        }
    }

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

        namespace detail
        {

            // [start, end)
            std::pair<std::int32_t, std::int32_t> search_block_left_bound(std::size_t start, std::size_t end, std::int32_t target) {
                std::size_t base_addr = start;
                static constexpr std::size_t block_size = 64;
                ::Buffer buffer;
                ::initBuffer(520, block_size, &buffer);

                unsigned char* blk{ nullptr };
                --end;
                // [start, end]
                while(start != end) {
                    std::size_t middle = (start + end) / 2;
                    blk = ::readBlockFromDisk(middle, &buffer, "./block/");
                    std::int32_t value = *(std::int32_t*)(blk);
                    ::freeBlockInBuffer(blk, &buffer);
                    if(value < target) {
                        start = middle + 1;
                    }
                    else {
                        end = middle;
                    }
                }
                std::printf("in left bound\nstart: %d, end: %d\n", (int)start, (int)end);

                blk = ::readBlockFromDisk(start, &buffer, "./block/");
                std::int32_t value = *(std::int32_t*)(blk);
                ::freeBlockInBuffer(blk, &buffer);

                // compare current value and expand range

                // on the most left position already, return
                if(value == target && start == base_addr) {
                    ::freeBuffer(&buffer);
                    return { start, 0 };
                }
                // can't find the target
                if(value > target && start == base_addr) {
                    ::freeBuffer(&buffer);
                    return  { -1, -1 };
                }
                // scan the previous block to expand range
                std::size_t block_idx = start;
                if(value >= target) {
                    --block_idx;
                }
                blk = ::readBlockFromDisk(block_idx, &buffer, "./block/");
                for(std::size_t i = 0; i != 7; ++i) {
                    std::int32_t n = *(std::int32_t*)(blk + i * 8);
                    if(n == target) {
                        ::freeBlockInBuffer(blk, &buffer);
                        ::freeBuffer(&buffer);
                        return { block_idx, i * 8 };
                    }
                }
                ::freeBlockInBuffer(blk, &buffer);
                ::freeBuffer(&buffer);
                if(value == target) {
                    return { start, 0 };
                }
                // value > target or value < target
                else {
                    return { -1, -1 };
                }
            }

            std::pair<std::int32_t, std::int32_t> search_block_right_bound(std::size_t start, std::size_t end, std::int32_t target) {
                std::size_t base_addr = start;
                static constexpr std::size_t block_size = 64;
                ::Buffer buffer;
                ::initBuffer(520, block_size, &buffer);

                unsigned char* blk{ nullptr };
                --end;
                while(start != end) {
                    std::size_t middle = (start + end + 1) / 2;
                    blk = ::readBlockFromDisk(middle, &buffer, "./block/");
                    std::int32_t value = *(std::int32_t*)(blk);
                    ::freeBlockInBuffer(blk, &buffer);
                    if(value > target) {
                        end = middle - 1;
                    }
                    else {
                        start = middle;
                    }
                }
                std::printf("in right bound\nstart: %d, end: %d\n", (int)start, (int)end);

                blk = ::readBlockFromDisk(start, &buffer, "./block/");
                std::int32_t value = *(std::int32_t*)(blk);

                // can't find the target
                if(value > target && start == base_addr) {
                    ::freeBuffer(&buffer);
                    return { -1, -1 };
                }
                // scan the previous or current block to expand range
                std::size_t block_idx = start;
                if(value > target) {
                    // hoping the previous has one value equal to target
                    --block_idx;
                }
                blk = ::readBlockFromDisk(block_idx, &buffer, "./block/");
                for(std::int32_t i = 6; i >= 0; --i) {
                    std::int32_t n = *(std::int32_t*)(blk + i * 8);
                    if(n == target) {
                        ::freeBlockInBuffer(blk, &buffer);
                        ::freeBuffer(&buffer);
                        return { block_idx, i * 8 };
                    }
                }
                ::freeBlockInBuffer(blk, &buffer);
                ::freeBuffer(&buffer);
                return  { -1, -1 };
            }

        }
        void binary_search(std::size_t base_addr, std::size_t block_num, std::int32_t target, std::size_t write_addr) {
            static constexpr std::size_t block_size = 64;

            // step1: sort data block using n_merge_sort
            sorting::n_merge_sort(base_addr, block_num);

            // step2: find data range equal to target using binary search
            auto [left_idx, left_offset] = detail::search_block_left_bound(base_addr, base_addr + block_num, target);
            auto [right_idx, right_offset] = detail::search_block_right_bound(base_addr, base_addr + block_num, target);
            std::printf("bound [%d, %d], [%d, %d]\n", left_idx, left_offset, right_idx, right_offset);
            //        target doesn't exist in all block
            if(left_idx == -1 || right_idx == -1) {
                std::cout << "can't find target" << std::endl;
                return;
            }
            std::printf("block [%d, %d] is valid range\n", left_idx, right_idx);
            std::printf("first block offset: %d\nlast block offset: %d\n", left_offset, right_offset);

            // step3: init buffer storing block
            ::Buffer buffer;
            ::initBuffer(520, block_size, &buffer);

            std::size_t write_bytes = 0;
            std::size_t write_cnt = 0;
            unsigned char* write_blk{ nullptr };

            // copy [start, end) data in read_blk to write_blk
            auto copy_write_block = [&](unsigned char* read_blk, std::size_t start, std::size_t end) {
                std::cout << start << " " << end << std::endl;
                for(std::size_t bytes = start; bytes != end; bytes += 8) {
                    if(write_blk == nullptr) {
                        write_blk = ::getNewBlockInBuffer(&buffer);
                        std::memset(write_blk, 0, 60);
                    }
                    std::cout << write_bytes << "\n";
                    *(std::int64_t*)(write_blk + write_bytes) = *(std::int64_t*)(read_blk + bytes);
                    write_bytes += 8;
                    if(write_bytes == 56) {
                        ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
                        write_blk = nullptr;
                        write_bytes = 0;
                    }
                }
            };

            // step4: copy the data and write to new block
            unsigned char* read_blk = ::readBlockFromDisk(left_idx, &buffer, "./block/");

            // step4.1: if all valid data in one block, only copy and write one time
            if(left_idx == right_idx) {
                copy_write_block(read_blk, left_offset, right_offset + 8);
                ::freeBlockInBuffer(read_blk, &buffer);
            }
            // step4.2: otherwise, handle
            //          the most left block one time,
            //          the middle blocks one time
            //          the most right block one time
            else {
                copy_write_block(read_blk, left_offset, 56);
                ::freeBlockInBuffer(read_blk, &buffer);

                for(std::int32_t i = left_idx + 1; i < right_idx; ++i) {
                    read_blk = ::readBlockFromDisk(i, &buffer, "./block/");
                    copy_write_block(read_blk, 0, 56);
                    ::freeBlockInBuffer(read_blk, &buffer);
                }

                read_blk = ::readBlockFromDisk(right_idx, &buffer, "./block/");
                copy_write_block(read_blk, 0, right_offset + 8);
                ::freeBlockInBuffer(read_blk, &buffer);
            }

            // step5: write the extra data to disk
            if(write_blk != nullptr) {
                ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
            }

            // step6: free buffer
            ::freeBuffer(&buffer);
        }
    }
}
