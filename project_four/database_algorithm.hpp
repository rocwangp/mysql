#pragma once

#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <random>
#include <cstring>

#include "extmem-c/extmem.h"

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
                std::int32_t value11 = *(std::int32_t*)(blk1.blk + blk1.read_bytes);
                std::int32_t value12 = *(std::int32_t*)(blk1.blk + blk1.read_bytes + 4);
                std::int32_t value21 = *(std::int32_t*)(blk2.blk + blk2.read_bytes);
                std::int32_t value22 = *(std::int32_t*)(blk2.blk + blk2.read_bytes + 4);
                return value11 == value21 ? value12 > value22 : value11 > value21;
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
                std::sort(p, p + 7, [](std::int64_t n1, std::int64_t n2) {
                    std::int32_t n11 = *(std::int32_t*)((char*)(&n1));
                    std::int32_t n12 = *(std::int32_t*)((char*)(&n1) + 4);
                    std::int32_t n21 = *(std::int32_t*)((char*)(&n2));
                    std::int32_t n22 = *(std::int32_t*)((char*)(&n2) + 4);
                    return n11 == n21 ? n12 < n22 : n11 < n21;
                });
                ::writeBlockToDisk(blk, base_addr + i, &buffer, "./block/");
            }

            // step2: merge every sorted block unit
            std::vector<block> blocks;
            blocks.reserve(block_num);
            // step2.0: init 16 sorted block unit, every unit has only one block
            for(std::size_t i = 0; i != block_num; ++i) {
                blocks.emplace_back(base_addr + i, base_addr + i, base_addr + i);
            }

            std::cout << "start sort all block" << std::endl;
            while(blocks.size() != 1) {
                // step2.1: calculate block count could be read,
                //          max count is 7 because there is one block to store sorted data
                std::size_t read_block = std::min(readable_block - 1, blocks.size());
                std::cout << "blocks size: " << blocks.size() << " " << "read_block: " << read_block << std::endl;

                // step2.2: calculate how many counts it cost to sort
                //          16 block unit need 3 counts: 7, 7, 2
                std::size_t read_cnt = blocks.size() / read_block;
                if(blocks.size() % read_block) {
                    ++read_cnt;
                }
                std::cout << "will read " << read_cnt << " times\n";
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
                    std::cout << "heap size: " << pq.size() << std::endl;
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
                            // write to a new position to store sorted data
                            std::cout << "write sorted block to " << write_addr + write_cnt << "\n";
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
                            std::cout << "rewrite the block: " << write_addr + index << " to " << j << std::endl;
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

                std::cout << "loop once" << std::endl;
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

            ::freeBuffer(&buffer);
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

    namespace projection
    {
        void shadow() {


        }
    }

    namespace connection
    {
        void nest_loop_join(std::size_t addr1, std::size_t addr2) {
            static constexpr std::size_t buffer_size = 520;
            static constexpr std::size_t block_size = 64;
            static constexpr std::size_t block_num1 = 16;
            static constexpr std::size_t block_num2 = 32;
            static constexpr std::size_t write_addr = 120000;

            ::Buffer buffer;
            ::initBuffer(buffer_size, block_size, &buffer);

            std::size_t write_bytes{ 0 };
            std::size_t write_cnt{ 0 };
            unsigned char* write_blk{ nullptr };

            for(std::size_t i = 0; i != block_num1; ++i) {
                unsigned char* read_blk1 = ::readBlockFromDisk(addr1 + i, &buffer, "./block/");
                for(std::size_t j = 0; j != block_num2; ++j) {
                    unsigned char* read_blk2 = ::readBlockFromDisk(addr2 + j, &buffer, "./block/");
                    for(std::size_t p = 0; p != 7; ++p) {
                        std::int64_t value1 = *(std::int64_t*)(read_blk1 + p * 8);
                        for(std::size_t q = 0; q != 7; ++q) {
                            std::int64_t value2 = *(std::int64_t*)(read_blk2 + q * 8);
                            if(*(std::int32_t*)(read_blk1 + p * 8) != *(std::int32_t*)(read_blk2 + q * 8)) {
                                continue;
                            }
                            std::printf("%d, %d\t", *(std::int32_t*)(read_blk1 + p * 8), *(std::int32_t*)(read_blk1 + p * 8 + 4));
                            std::printf("%d, %d\n", *(std::int32_t*)(read_blk2 + q * 8), *(std::int32_t*)(read_blk2 + q * 8 + 4));
                            if(write_blk == nullptr) {
                                write_blk = ::getNewBlockInBuffer(&buffer);
                            }
                            *(std::int64_t*)(write_blk + write_bytes) = value1;
                            *(std::int64_t*)(write_blk + write_bytes + 8) = value2;
                            write_bytes += 16;
                            if(write_bytes == 48) {
                                while(write_bytes != 64) {
                                    *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                                    write_bytes += 4;
                                }
                                ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
                                write_bytes = 0;
                                write_blk = nullptr;
                            }
                        }
                    }
                    ::freeBlockInBuffer(read_blk2, &buffer);
                }
                ::freeBlockInBuffer(read_blk1, &buffer);
            }
            if(write_blk) {
                while(write_bytes != 64) {
                    *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                    write_bytes += 4;
                }
                ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
            }
            ::freeBuffer(&buffer);
        }

        void sort_merge_join(std::size_t addr1, std::size_t addr2) {
            static constexpr std::size_t buffer_size = 520;
            static constexpr std::size_t block_size = 64;
            static constexpr std::size_t write_addr = 110000;

            // step0: sort data block
            /* sorting::n_merge_sort(addr1, 16); */
            /* sorting::n_merge_sort(addr2, 32); */

            // step1: init buffer
            ::Buffer buffer;
            ::initBuffer(buffer_size, block_size, &buffer);

            std::size_t write_bytes{ 0 }, write_cnt{ 0 };
            unsigned char* write_blk{ nullptr };

            std::size_t read_bytes1{ 0 }, read_bytes2{ 0 };
            std::size_t index1{ 0 }, index2{ 0 };
            unsigned char* read_blk1{ nullptr }, *read_blk2{ nullptr };

            // check read_blk and try to free it
            auto update_read_blk = [&buffer](unsigned char*& read_blk, std::size_t& read_bytes, std::size_t& index) {
                if(read_blk && read_bytes == 56) {
                    ::freeBlockInBuffer(read_blk, &buffer);
                    read_blk = nullptr;
                    read_bytes = 0;
                    ++index;
                }
            };

            // write 128 bytes to write_blk and flush to disk if needed
            auto update_write_blk = [&](std::int64_t n1, std::int64_t n2) {
                if(write_blk == nullptr) {
                    write_blk = ::getNewBlockInBuffer(&buffer);
                }
                char* p1 = (char*)&n1;
                char* p2 = (char*)(&n2);
                std::printf("%d, %d\t%d, %d\n", *(std::int32_t*)(p1), *(std::int32_t*)(p1 + 4),
                                                *(std::int32_t*)(p2), *(std::int32_t*)(p2 + 4));
                *(std::int64_t*)(write_blk + write_bytes) = n1;
                *(std::int64_t*)(write_blk + write_bytes + 8) = n2;
                write_bytes += 16;
                if(write_bytes == 48) {
                    // idle memory, memset it
                    while(write_bytes != 64) {
                        *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                        write_bytes += 4;
                    }
                    ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
                    write_blk = nullptr;
                    write_bytes = 0;
                }
            };

            std::size_t dup_cnt{ 0 };
            std::int32_t prev_value{ 0 };
            bool handle_dup_data{ false };
            while(index1 != 16 && index2 != 32) {
                if(read_blk1 == nullptr) {
                    read_blk1 = ::readBlockFromDisk(addr1 + index1, &buffer, "./block/");
                }
                if(read_blk2 == nullptr) {
                    read_blk2 = ::readBlockFromDisk(addr2 + index2, &buffer, "./block/");
                }

                // step2: compare first column in two relationships
                std::int32_t n1 = *(std::int32_t*)(read_blk1 + read_bytes1);
                std::int32_t n2 = *(std::int32_t*)(read_blk2 + read_bytes2);
                if(n1 < n2) {
                    // handle_dup_data: represent whether need to copy prev data once
                    // dup_cnt: the number of data needed to copy
                    // prev_value: previous value

                    // step2.11: copy previous data
                    if(handle_dup_data && dup_cnt > 0 && n1 == prev_value) {
                        // step2.12: calculate the range needed to copy
                        std::size_t prev_block_cnt = (dup_cnt - write_bytes / 16) / 3;
                        std::size_t prev_value_cnt = 3;
                        if((dup_cnt - write_bytes / 16) % 3) {
                            ++prev_block_cnt;
                            prev_value_cnt = (dup_cnt - write_bytes / 16) % 3;
                        }
                        std::size_t start_addr{ write_addr + write_cnt - prev_block_cnt };
                        std::size_t end_addr{ write_addr + write_cnt };
                        std::size_t start_bytes{ (3 - prev_value_cnt) * 16 };
                        std::size_t end_bytes{ write_bytes };

                        std::printf("start position: %d block %d bytes\n", (int)start_addr, (int)start_bytes);
                        std::printf("end position: %d block %d bytes\n", (int)end_addr, (int)end_bytes);
                        std::printf("cur write_blk point to %d addr %d bytes\n", (int)(write_addr + write_cnt), (int)(write_bytes));

                        // step2.13: copy [start_addr, end_addr) because these data has writen on disk already
                        //           data in end_addr haven't beed writen to disk now, so can't read it from disk!!!!!!!
                        while(start_addr != end_addr) {
                            unsigned char* dup_blk = ::readBlockFromDisk(start_addr, &buffer, "./block/");
                            while(start_bytes != 48) {
                                *(std::int32_t*)(dup_blk + start_bytes + 4) = *(std::int32_t*)(read_blk1 + read_bytes1 + 4);
                                update_write_blk(*(std::int64_t*)(dup_blk + start_bytes), *(std::int64_t*)(dup_blk + start_bytes + 8));
                                start_bytes += 16;
                            }
                            start_bytes = 0;
                            ::freeBlockInBuffer(dup_blk, &buffer);
                            ++start_addr;
                        }
                        // step2.14: copy the data in end_addr to new block and write back to write_blk
                        //           could't read data from write_blk directly and write back!!!!!
                        //           because update_write_blk maybe change write_blk and update current block point!!!!
                        //           which will cause reading dup data error
                        //           need to copy to a temporary block and then write back to write_blk
                        if(start_addr == end_addr) {
                            std::printf("handle the duplicate data in write_blk\n");
                            unsigned char* blk = ::getNewBlockInBuffer(&buffer);
                            unsigned char* dup_blk = write_blk;
                            // write_blk has been modified due to write data, read dup data from disk instead of reading from write_blk
                            if(start_addr != write_addr + write_cnt) {
                                dup_blk = ::readBlockFromDisk(start_addr, &buffer, "./block/");
                            }
                            // step2.15: copy these data to a temporary block
                            for(auto bytes = start_bytes; bytes != end_bytes; bytes += 16) {
                                *(std::int64_t*)(blk + bytes - start_bytes) = *(std::int64_t*)(dup_blk + bytes);
                                *(std::int64_t*)(blk + bytes - start_bytes + 8) = *(std::int64_t*)(dup_blk + bytes + 8);
                            }
                            if(start_addr != write_addr + write_cnt) {
                                ::freeBlockInBuffer(dup_blk, &buffer);
                            }
                            // step2.16: write back to write_blk
                            for(std::size_t bytes = 0; bytes != end_bytes - start_bytes; bytes += 16) {
                                *(std::int32_t*)(blk + bytes + 4) = *(std::int32_t*)(read_blk1 + read_bytes1 + 4);
                                update_write_blk(*(std::int64_t*)(blk + bytes), *(std::int64_t*)(blk + bytes + 8));
                            }
                            ::freeBlockInBuffer(blk, &buffer);
                        }
                    }
                    // not equal, needn't copy dup data
                    else if(handle_dup_data && dup_cnt > 0 && n1 != prev_value) {
                        handle_dup_data = false;
                        dup_cnt = 0;
                    }
                    // record prev_value when n1 < n2 first
                    // R: 20 20
                    // S: 20 20 20 20 21
                    // when compare the first 20 of R and 20 of S, write (20, 20), (20, 20), (20, 20), (20, 20) to write_blk
                    // when compare the first 20 of R and 21 of S, assign 20 to prev_value and set handle_dup_data
                    // when handle the second of 20 of R, find it equals to prev_value(20) and handle_dup_data has been set
                    // so need copy (20, 20), (20, 20), (20, 20), (20, 20) to write_blk
                    // dup_cnt is used to record the number of data need to copy(4 in this example)
                    else {
                        prev_value = *(std::int32_t*)(read_blk1 + read_bytes1);
                        handle_dup_data = true;
                    }
                    read_bytes1 += 8;
                }
                else if(n1 > n2) {
                    // step2.2: clear record
                    dup_cnt = 0;
                    handle_dup_data = false;
                    read_bytes2 += 8;
                }
                else {
                    // step2.3: init record when n1 == n2 first
                    if(handle_dup_data) {
                        handle_dup_data = false;
                        dup_cnt = 0;
                    }
                    update_write_blk(*(std::int64_t*)(read_blk1 + read_bytes1), *(std::int64_t*)(read_blk2 + read_bytes2));

                    /* read_bytes1 += 8; */
                    ++dup_cnt;
                    read_bytes2 += 8;
                }
                update_read_blk(read_blk1, read_bytes1, index1);
                update_read_blk(read_blk2, read_bytes2, index2);
            }
            if(write_blk) {
                while(write_bytes != 64) {
                    *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                    write_bytes += 4;
                }
                ::writeBlockToDisk(write_blk, write_addr + write_cnt, &buffer, "./block/");
            }
            ::freeBuffer(&buffer);
        }

        void hash_join(std::size_t addr1, std::size_t addr2) {
            static constexpr std::size_t buffer_size = 520;
            static constexpr std::size_t block_size = 64;
            static constexpr std::size_t write_addr = 11000;

            ::Buffer buffer;
            ::initBuffer(buffer_size, block_size, &buffer);

            std::unordered_multimap<std::int32_t, std::int64_t> umm;
            for(std::size_t i = 0; i != 16; ++i) {
                unsigned char* blk = ::readBlockFromDisk(addr1 + i, &buffer, "./block/");
                for(std::size_t j = 0; j != 7; ++j) {
                    std::int32_t n1 = *(std::int32_t*)(blk + 8 * j);
                    std::int64_t n2 = *(std::int64_t*)(blk + 8 * j);
                    umm.emplace(n1, n2);
                }
                ::freeBlockInBuffer(blk, &buffer);
            }

            unsigned char* write_blk{ nullptr };
            std::size_t write_bytes{ 0 }, write_cnt{ 0 };

            for(std::size_t i = 0; i != 32; ++i) {
                unsigned char* read_blk = ::readBlockFromDisk(addr2 + i, &buffer, "./block/");
                for(std::size_t j = 0; j != 7; ++j) {
                    std::int32_t n1 = *(std::int32_t*)(read_blk + 8 * j);
                    std::int64_t n2 = *(std::int64_t*)(read_blk + 8 * j);
                    auto [begin, end] = umm.equal_range(n1);
                    for(auto it = begin; it != end; ++i) {
                        if(write_blk == nullptr) {
                            write_blk = ::getNewBlockInBuffer(&buffer);
                        }
                        *(std::int64_t*)(write_blk + write_bytes) = it->second;
                        *(std::int64_t*)(write_blk + write_bytes + 8) = n2;
                        write_bytes += 16;
                        if(write_bytes == 48) {
                            ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
                            write_blk = nullptr;
                            write_bytes = 0;
                        }
                    }
                }
            }
            if(write_blk) {
                while(write_bytes != 64) {
                    *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                    write_bytes += 4;
                }
                ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
            }
            ::freeBuffer(&buffer);
        }
    }

    namespace collection
    {
        void get_convergence(std::size_t addr1, std::size_t addr2) {
            static constexpr std::size_t buffer_size = 520;
            static constexpr std::size_t block_size = 64;
            static constexpr std::size_t write_addr = 70000;

            // step0: sort data block
            sorting::n_merge_sort(addr1, 16);
            sorting::n_merge_sort(addr2, 32);

            // step1: init buffer
            ::Buffer buffer;
            ::initBuffer(buffer_size, block_size, &buffer);

            std::size_t write_bytes = 0;
            std::size_t write_cnt = 0;
            std::int64_t prev_value = 0;
            bool first{ true };
            unsigned char* write_blk{ nullptr };

            std::size_t index1 = 0, index2 = 0;
            std::size_t read_bytes1 = 0, read_bytes2 = 0;
            unsigned char* read_blk1{ nullptr };
            unsigned char* read_blk2{ nullptr };

            auto update_read_blk = [&buffer](unsigned char*& read_blk, std::size_t& read_bytes, std::size_t& index, std::int64_t& value) {
                value = *(std::int64_t*)(read_blk + read_bytes);
                std::printf("read int32_t value: %d\n", *(std::int32_t*)(read_blk + read_bytes));
                read_bytes += 8;
                if(read_bytes == 56) {
                    ::freeBlockInBuffer(read_blk, &buffer);
                    read_blk = nullptr;
                    read_bytes = 0;
                    ++index;
                }
            };

            // step2: scan all data in read_blk1 and read_blk2, choose the less one and copy to write_blk
            while(index1 != 16 || index2 != 32) {
                // step3: it's allowed that one of both is nullptr(index1 == 16 || index2 == 32)
                //        in this case, only choose another data without comparsion
                if(index1 != 16 && read_blk1 == nullptr) {
                    read_blk1 = ::readBlockFromDisk(addr1 + index1, &buffer, "./block/");
                }
                if(index2 != 32 && read_blk2 == nullptr) {
                    read_blk2 = ::readBlockFromDisk(addr2 + index2, &buffer, "./block/");
                }
                std::printf("%d, %d\n", (int)index1, (int)index2);
                std::int64_t value;
                // step4.1: compare and choose the less one betwwn read_blk1 and read_blk2
                if(read_blk1 && read_blk2) {
                    std::int32_t n11 = *(std::int32_t*)(read_blk1 + read_bytes1);
                    std::int32_t n12 = *(std::int32_t*)(read_blk1 + read_bytes1 + 4);
                    std::int32_t n21 = *(std::int32_t*)(read_blk2 + read_bytes2);
                    std::int32_t n22 = *(std::int32_t*)(read_blk2 + read_bytes2 + 4);
                    // update_read_blk will assign read_blk's data to value
                    if((n11 == n21 && n12 < n22) || (n11 < n21)) {
                        update_read_blk(read_blk1, read_bytes1, index1, value);
                    }
                    else {
                        update_read_blk(read_blk2, read_bytes2, index2, value);
                    }
                }
                // step4.2: choose read_blk1's value
                else if(read_blk1) {
                    update_read_blk(read_blk1, read_bytes1, index1, value);
                }
                // step4.3: choose read_blk2's value
                else {
                    update_read_blk(read_blk2, read_bytes2, index2, value);
                }

                // prev_value is used to avoid duplicates
                if(first || (!first && prev_value != value)) {
                    first = false;
                    if(write_blk == nullptr) {
                        write_blk = ::getNewBlockInBuffer(&buffer);
                    }
                    *(std::int64_t*)(write_blk + write_bytes) = value;;
                    prev_value = value;

                    write_bytes += 8;
                    if(write_bytes == 56) {
                        ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
                        write_bytes = 0;
                        write_blk = nullptr;
                    }
                }

                std::printf("loop once\n");
            }
            if(write_blk) {
                while(write_bytes != 64) {
                    *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                    *(std::int32_t*)(write_blk + write_bytes + 4) = std::numeric_limits<std::int32_t>::max();
                    write_bytes += 8;
                }
                ::writeBlockToDisk(write_blk, write_addr + write_cnt, &buffer, "./block/");
            }

            ::freeBuffer(&buffer);
        }

        void get_intersection(std::size_t addr1, std::size_t addr2) {
            static constexpr std::size_t buffer_size = 520;
            static constexpr std::size_t block_size = 64;
            static constexpr std::size_t write_addr = 80000;

            // step0: sort data block
            sorting::n_merge_sort(addr1, 16);
            sorting::n_merge_sort(addr2, 32);

            // step1: init buffer
            ::Buffer buffer;
            ::initBuffer(buffer_size, block_size, &buffer);

            std::size_t write_bytes{ 0 };
            std::size_t write_cnt{ 0 };
            std::int64_t prev_value{ 0 };
            bool first{ true };
            unsigned char* write_blk{ nullptr };

            unsigned char* read_blk1{ nullptr };
            unsigned char* read_blk2{ nullptr };
            std::size_t index1{ 0 }, index2{ 0 };
            std::size_t read_bytes1{ 0 }, read_bytes2{ 0 };

            auto update_read_blk = [&buffer](unsigned char*& read_blk, std::size_t& read_bytes, std::size_t& index) {
                if(read_blk) {
                    if(read_bytes == 56) {
                        ::freeBlockInBuffer(read_blk, &buffer);
                        read_blk = nullptr;
                        read_bytes = 0;
                        ++index;
                    }
                }
            };
            while(index1 != 16 && index2 != 32) {
                // step2: it's not allowed that one of both is nullptr
                if(read_blk1 == nullptr) {
                    read_blk1 = ::readBlockFromDisk(addr1 + index1, &buffer, "./block/");
                }
                if(read_blk2 == nullptr) {
                    read_blk2 = ::readBlockFromDisk(addr2 + index2, &buffer, "./block/");
                }
                std::int64_t value;
                std::int32_t n11 = *(std::int32_t*)(read_blk1 + read_bytes1);
                std::int32_t n12 = *(std::int32_t*)(read_blk1 + read_bytes1 + 4);
                std::int32_t n21 = *(std::int32_t*)(read_blk2 + read_bytes2);
                std::int32_t n22 = *(std::int32_t*)(read_blk2 + read_bytes2 + 4);
                if((n11 > n21) || (n11 == n21 && n12 > n22)) {
                    read_bytes2 += 8;
                }
                else if((n11 < n21) || (n11 == n21 && n12 < n22)) {
                    read_bytes1 += 8;
                }
                // only when n1 == n2, copy n1 and n2 to write_blk
                else {
                    value = *(std::int64_t*)(read_blk1 + read_bytes1);
                    read_bytes1 += 8;
                    read_bytes2 += 8;

                    if(first || (!first && prev_value != value)) {
                        first = false;
                        if(write_blk == nullptr) {
                            write_blk = ::getNewBlockInBuffer(&buffer);
                        }
                        *(std::int64_t*)(write_blk + write_bytes) = value;
                        prev_value = value;
                        write_bytes += 8;
                        if(write_bytes == 56) {
                            ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
                            write_bytes = 0;
                            write_blk = nullptr;
                        }
                    }
                }
                update_read_blk(read_blk1, read_bytes1, index1);
                update_read_blk(read_blk2, read_bytes2, index2);
            }

            if(write_blk) {
                while(write_bytes != 64) {
                    *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                    *(std::int32_t*)(write_blk + write_bytes + 4) = std::numeric_limits<std::int32_t>::max();
                    write_bytes += 8;
                }
                ::writeBlockToDisk(write_blk, write_addr + write_cnt, &buffer, "./block/");
            }
            ::freeBuffer(&buffer);
        }

        void get_complement(std::size_t addr1, std::size_t addr2) {
            static constexpr std::size_t buffer_size = 520;
            static constexpr std::size_t block_size = 64;
            static constexpr std::size_t write_addr = 90000;

            // step0: sort data blcok
            sorting::n_merge_sort(addr1, 16);
            sorting::n_merge_sort(addr2, 32);

            // step1: init buffer
            ::Buffer buffer;
            ::initBuffer(buffer_size, block_size, &buffer);

            std::size_t write_bytes{ 0 }, write_cnt{ 0 };
            unsigned char* write_blk { nullptr };

            std::size_t read_bytes1{ 0 }, read_bytes2{ 0 };
            std::size_t index1{ 0 }, index2{ 0 };
            unsigned char* read_blk1{ nullptr }, *read_blk2{ nullptr };

            bool first{ true };
            std::int64_t prev_value{ 0 };

            auto update_read_blk = [&buffer](unsigned char*& read_blk, std::size_t& read_bytes, std::size_t& index) {
                if(read_bytes == 56 && read_blk) {
                    ::freeBlockInBuffer(read_blk, &buffer);
                    read_blk = nullptr;
                    read_bytes = 0;
                    ++index;
                }
            };

            // step2: choose the data in read_blk2 but not in read_blk1
            while(index2 != 32) {
                // step3: it's allowed thet read_blk1 is nullptr but not allowed read_blk2 is nullptr
                //        because read_blk2 is nullptr represent algorithm is over
                if(index1 != 16 && read_blk1 == nullptr) {
                    read_blk1 = ::readBlockFromDisk(addr1 + index1, &buffer, "./block/");
                }
                if(read_blk2 == nullptr) {
                    read_blk2 = ::readBlockFromDisk(addr2 + index2, &buffer, "./block/");
                }
                std::printf("%d, %d\n", (int)(index1), (int)(index2));
                bool done{ false };
                std::int64_t value;
                // step4.1: read_blk1 is exist, compare data until comparing results changing from < to >
                //          or > directly at the beginning
                if(read_blk1) {
                    std::int32_t n11 = *(std::int32_t*)(read_blk1 + read_bytes1);
                    std::int32_t n12 = *(std::int32_t*)(read_blk1 + read_bytes1 + 4);
                    std::int32_t n21 = *(std::int32_t*)(read_blk2 + read_bytes2);
                    std::int32_t n22 = *(std::int32_t*)(read_blk2 + read_bytes2 + 4);
                    // <, keep on scanning read_blk1
                    if((n11 < n21) || (n11 == n21 && n12 < n22)) {
                        read_bytes1 += 8;
                    }
                    // >, n2 is the target value in read_blk2 but not in read_blk1
                    else if((n11 > n21) || (n11 == n21 && n12 > n22)) {
                        value = *(std::int64_t*)(read_blk2 + read_bytes2);
                        read_bytes2 += 8;
                        done = true;
                    }
                    // ==, keep on scanning
                    else {
                        read_bytes1 += 8;
                        read_bytes2 += 8;
                    }
                }
                // step4.2: read_blk1 is nullptr, all distinct data in read_blk2 is target value
                else {
                    value = *(std::int64_t*)(read_blk2 + read_bytes2);
                    read_bytes2 += 8;
                    done = true;
                }

                // step5: copy to write_blk and write back to disk if needed
                if(done && (first || (!first && prev_value != value))) {
                    first = false;
                    if(write_blk == nullptr) {
                        write_blk = ::getNewBlockInBuffer(&buffer);
                    }
                    *(std::int64_t*)(write_blk + write_bytes) = value;
                    prev_value = value;
                    write_bytes += 8;
                    if(write_bytes == 56) {
                        ::writeBlockToDisk(write_blk, write_addr + write_cnt++, &buffer, "./block/");
                        write_bytes = 0;
                        write_blk = nullptr;
                    }
                }
                update_read_blk(read_blk1, read_bytes1, index1);
                update_read_blk(read_blk2, read_bytes2, index2);
            }

            if(write_blk) {
                while(write_bytes != 64) {
                    *(std::int32_t*)(write_blk + write_bytes) = std::numeric_limits<std::int32_t>::max();
                    *(std::int32_t*)(write_blk + write_bytes + 4) = std::numeric_limits<std::int32_t>::max();
                    write_bytes += 8;
                }
                ::writeBlockToDisk(write_blk, write_addr + write_cnt, &buffer, "./block/");
            }
            ::freeBuffer(&buffer);
        }
    }
}
