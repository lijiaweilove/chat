#pragma once
#include <vector>
#include <random>
#include <ctime>
#include "GlobalVar.h"

class Ant
{
public:
    double total_distance;                // 当前路径的总距离
    std::vector<int> path;                // 当前蚂蚁的路径
private:
    int ID;                               // ID
    int move_count;                       // 移动次数
    int current_city;                     // 当前停留的城市
    std::vector<bool> open_table_city;    // 探索城市的状态
    std::default_random_engine generator; // 随机数生成器
public:
	Ant(int ID);
	~Ant();
	void clean_data();
    int choice_next_city();
    void cal_total_distance();
    void move(int next_city);
    void search_path();
};

