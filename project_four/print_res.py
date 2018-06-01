#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os
import sys

def check_sort_res(addr, count):
    prev_value = 0
    for i in range(0, count):
        filepath = "./block/" + str(addr + i) + ".blk"
        res = os.popen("od -A d -c -t d4 -w56 " + filepath)

        line = res.readlines()
        for j in range(0, len(line)):
            if j % 2 == 0:
                continue
            s = line[j].split();
            for p in range(0, len(s), 2) :
                value = int(s[p])
                if prev_value > value:
                    print_sort_res(addr, count)
                    print(prev_value)
                    print(s)
                    return
                else:
                    prev_value = value
            break
    print("done")


def print_sort_res(addr, count):
    for i in range(0, count):
        filepath = "./block/" + str(addr + i) + ".blk"
        res = os.popen("od -A d -c -t d4 -w56 " + filepath)
        line = res.readlines()
        for j in range(0, len(line)):
            if j % 2 == 0:
                continue
            s = line[j].split();
            for p in range(0, len(s), 2) :
                print(s[p], s[p + 1])
            break
    print("done")

def print_value_count(addr, count, value):
    cnt = 0
    for i in range(0, count):
        filepath = "./block/" + str(addr + i) + ".blk"
        res = os.popen("od -A d -c -t d4 " + filepath)
        line = res.readlines()
        for j in range(0, 14):
            if(j % 2 == 0):
                continue
            n = int(line[j].split()[0])
            if n == value:
                cnt = cnt + 1

    print(cnt)
    print("done")


def print_file(addr, count):
    for i in range(0, count):
        filepath = "./block/" + str(addr + i) + ".blk"
        print(filepath)
        os.system("od -A d -c -t d4 -w56 " + filepath)
    print("done")

if __name__ == '__main__':
    addr = int(sys.argv[1])
    num = int(sys.argv[2])
    # print_file(int(sys.argv[1]), int(sys.argv[2]))
    print_sort_res(addr, num)
    check_sort_res(int(sys.argv[1]), int(sys.argv[2]));
    # print_res(20000, 32)
