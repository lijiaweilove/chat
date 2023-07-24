#include "Ant.h"
#include <iostream>
#include <chrono>


Ant::Ant(int ID) :ID(ID),path(city_num),open_table_city(city_num)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    generator.seed(static_cast<unsigned>(timestamp) + ID);
    clean_data();
}

Ant::~Ant()
{
}

// 初始化数据
void Ant::clean_data()
{
    path.clear();
    total_distance = 0.0;
    move_count = 0;
    current_city = -1;
    for (int i = 0; i < city_num; i++) {
		open_table_city[i] = true;
	}

    int city_index = 0;  // 随机初始出生点
    current_city = city_index;
    path.push_back(city_index);
    open_table_city[city_index] = false;
    move_count = 1;
}

// 选择下一个城市(随机数)
int Ant::choice_next_city()
{
    int next_city = -1;
    std::vector<double> select_citys_prob(city_num, 0.0);  // 存储去下个城市的概率
    double total_prob = 0.0;
    // 获取去下一个城市的概率
    for (int i = 0; i < city_num; ++i) {
        if (open_table_city[i]) {
            try { 
                // 计算概率：与信息素浓度成正比，与距离成反比(第二个是启发式信息)
                select_citys_prob[i] = pow(pheromone_graph[current_city][i], ALPHA) * pow(1.0 / distance_graph[current_city][i], BETA);
                total_prob += select_citys_prob[i];
            }
            catch (const std::exception& e) {
                std::cerr << "Ant ID: " << ID << ", current city: " << current_city << ", target city: " << i << std::endl;
                std::exit(1);
            }
        }
    }
    // 轮盘选择城市
    if (total_prob > 0.0) {
        //  产生一个随机概率,0.0-total_prob
        std::uniform_real_distribution<double> distribution(0.0, total_prob);  // 左闭右闭区间;
        double temp_prob = distribution(generator);
        for (int i = 0; i < city_num; ++i) {
            if (open_table_city[i]) {
                //  轮次相减
                temp_prob -= select_citys_prob[i];
                if (temp_prob < 0.0) {
                    next_city = i;
                    break;
                }
            }
        }
    }

    if (next_city == -1) {
        std::uniform_int_distribution<int> distribution(1, city_num);
        next_city = distribution(generator);
        while (!open_table_city[next_city]) {  // if==False,说明已经遍历过了
            next_city = distribution(generator);
        }
    }
    return next_city;
}

// 计算路径总距离
void Ant::cal_total_distance()
{
    double temp_distance = 0.0;
    for (int i = 1; i < city_num; i++) {
        int start = path[i - 1];
        int end = path[i];
        temp_distance += distance_graph[start][end];
    }
    total_distance = temp_distance;
}

// 蚂蚁在城市间移动
void Ant::move(int next_city)
{
    path.push_back(next_city);
    open_table_city[next_city] = false;
    total_distance += distance_graph[current_city][next_city];
    current_city = next_city;
    move_count++;
}

// 搜索路径
void Ant::search_path() {
    clean_data();
    // 搜素路径，遍历path完所有城市为止
    while (move_count < city_num) {
    // while (move_count < city_num) {
        // 移动到下一个城市
        int next_city = choice_next_city();
        move(next_city);
    }
    // 计算路径总长度
    cal_total_distance();
}
