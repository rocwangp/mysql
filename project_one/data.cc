#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

std::vector<std::string> names = {
    "王", "李", "张", "可乐", "矿泉水", "芬达", "瓜子", "八宝粥", "笔记本",
    "哈哈", "键盘"
};
std::vector<std::string> ids;
int main()
{
    std::random_device rd;
    for(int i = 0; i < 5; ++i) {
        std::string id;
        for(int j = 0; j != 18; ++j) {
            id.append(1, (rd() % 10 + '0'));
        }
        ids.emplace_back(std::move(id));
    }

    std::ofstream fout{ "employee.txt", std::ios_base::out };
    for(int i = 0; i < 49; ++i) {
        std::string name;
        for(int i = 0; i < 2; ++i) {
            int n = rd() % names.size();
            name.append(names[n]);
        }
        std::string name_id;
        for(int i = 0; i < 18; ++i) {
            name_id.append(std::to_string(rd() % 10));
        }
        std::string address;
        address.append(1,(rd() % 26) + 'A');
        address.append("市");
        address.append(1, (rd() % 26) + 'a');
        address.append("区");
        address.append(1, (rd() % 10) + '0');
        address.append("路");

        std::string money = std::to_string(rd() % 10000);
        int dno = rd() % ids.size();
        std::string id = ids[dno];

        fout << name << "\t"
             << name_id << "\t"
             << address << "\t"
             << money << "\t"
             << id << "\t"
             << dno << "\n";
    }

    fout.close();
    /* fout.open("department.txt", std::ios_base::out); */
    /* fout << "研发部\t0\t" << ids[0] << "\t2018-01-01\n" */
    /*      << "市场部\t1\t" << ids[1] << "\t2011-01-03\n" */
    /*      << "人力资源部\t2\t" << ids[2] << "\t2017-05-04\n" */
    /*      << "投资并购部\t3\t" << ids[3] << "\t2015-11-20\n" */
    /*      << "美工部\t4\t" << ids[4] << "\t2016-12-12\n"; */

    /* fout.close(); */
    /* fout.open("project.txt", std::ios_base::out); */
    /* fout << "qq研发\tP1\t" << "深圳\t" << "0\n"; */
    /* fout << "市场调研\tP2\t" << "上海\t" << "1\n"; */
    /* fout << "魔术游戏研发\tP3\t" << "成都\t" << "2\n"; */
    /* fout << "并购管理\tP4\t" << "深圳\t" << "3\n"; */
    /* fout << "UI设计\tP5\t" << "北京\t" << "4\n"; */
    /* fout << "天美游戏研究\tP6\t" << "成都\t" << "5\n"; */
    /* fout << "校招管理\tP7\t" << "深圳\t" << "6\n"; */
    /* fout << "美术研究\tP8\t" << "哈尔滨\t" << "7\n"; */
    /* fout << "社招管理\tP9\t" << "南京\t" << "8\n"; */
    /* fout << "微信研发\tP10\t" << "广州\t" << "9\n"; */

    /* fout.close(); */
    return 0;
}
