#!/usr/bin/python
# -*- coding: UTF-8 -*-
import os

def print_error(addr, count):
    print("error.................")
    for i in range(0, count):
        print(i, '................')
        filepath = "./block/" + str(addr + i) + ".blk"
        os.system("od -A d -c -t d4 -w8 " + filepath)

def check_sort_res(addr, count):
    prev_value = 0
    for i in range(0, count):
        filepath = "./block/" + str(addr + i) + ".blk"
        res = os.popen("od -A d -c -t d4 -w8 " + filepath)

        line = res.readlines()
        for j in range(0, 14):
            if(j % 2 == 0):
                continue
            # print(line[j].split())
            if len(line[j].split()) > 2:
                continue
            value = int(line[j].split()[0])
            if prev_value > value:
                print_error(addr, count)
                print(i, j, prev_value, value)
                return
            else:
                prev_value = value
    print("done")


def print_sort_res(addr, count):
    for i in range(0, count):
        filepath = "./block/" + str(addr + i) + ".blk"
        res = os.popen("od -A d -c -t d4 -w8 " + filepath)
        line = res.readlines()
        for j in range(0, 14):
            if(j % 2 == 0):
                continue
            try :
                print("[" + str(i) + "," + str(j) + "]:", int(line[j].split()[0]), int(line[j].split()[1]))
            finally:
                print(line[j])

    print("done")

def print_value_count(addr, count, value):
    cnt = 0
    for i in range(0, count):
        filepath = "./block/" + str(addr + i) + ".blk"
        res = os.popen("od -A d -c -t d4 -w8 " + filepath)
        line = res.readlines()
        for j in range(0, 14):
            if(j % 2 == 0):
                continue
            n = int(line[j].split()[0])
            if n == value:
                cnt = cnt + 1

    print(cnt)
    print("done")



if __name__ == '__main__':
    check_sort_res(10000, 5)
    # print_res(20000, 32)
