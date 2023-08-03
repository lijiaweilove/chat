#pragma once
#include <vector>
#include <random>
#include <ctime>
#include "GlobalVar.h"

class Ant
{
public:
    double current_distance;                // 当前路径的总距离
    std::vector<int> path;                // 当前蚂蚁的路径
    std::vector<std::vector<int>> all_way;  // 记录所有最终路径
    double total_distance;  // 记录所有最终路径长度
    int now_height;  // 当前车的当前载重量
    double total_distance_variance;  // 总距离方差
    double total_weight_variance;  // 总权重方差
    int move_count;                       // 城市数量
    std::vector<double> line;  // 直线的k,b
private:
    int ID;                               // ID
    int current_city;                     // 当前停留的城市
    std::vector<bool> open_table_city;    // 探索城市的状态
    std::default_random_engine generator; // 随机数生成器
    int random_height;	 // 最大载重量
public:
	Ant(int ID);
	~Ant();
	void clean_data();
    int choice_first_city();
    int choice_next_city(const int flag);
    void cal_total_distance();
    void move(const int next_city);
    void search_path();
    void move_origin();
    void cal_distance_variance();
    void cal_weight_variance();
};

