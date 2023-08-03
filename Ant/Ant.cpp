#include "Ant.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <set>


Ant::Ant(int ID) :ID(ID),path(city_num),move_count(city_num),total_distance(0.0), 
    total_distance_variance(0.0), total_weight_variance(0.0),random_height(top_height)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    generator.seed(static_cast<unsigned>(timestamp) + ID);
    open_table_city.resize(city_num, true);
    open_table_city[0] = false;
    line.resize(2, 0.0);
    clean_data();
}

Ant::~Ant()
{
}

// 初始化数据
void Ant::clean_data()
{
    path.clear();
    now_height = 0;
    current_distance = 0.0;

    int city_index = 0;  // 随机初始出生点
    current_city = city_index;
    path.push_back(city_index);
    random_height = (random_height == low_height) ? top_height : low_height;
}

// 选择第一个城市
int Ant::choice_first_city()
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

// 选择下一个城市(随机数)
int Ant::choice_next_city(const int flag)
{
    int next_city = -1;
    std::vector<double> select_citys_prob(city_num, 0.0);  // 存储去下个城市的概率
    double total_prob = 0.0;
    double distance = 1.0;  // 点到直线的距离=std::abs(y0 - k * x0 - b) / std::sqrt(k * k + 1);
    double is_segment = 0.0;  // 判断是否在同一扇区
    double is_angle = 1.0;  // 判断角度是否过大
    double temp_k = 0.0;
    int len = path.size();
    double angleRad = 0.0; // 两直线夹角
    // 计算去下一个城市的概率
    for (int i = 0; i < city_num; ++i) {
        if (open_table_city[i]) {
            try { 
                // 计算两直线夹角
                temp_k = (distance_y[i] - distance_y[path[len - 1]]) / (distance_x[i] - distance_x[path[len - 1]]);
                angleRad = std::atan(std::abs((temp_k - line[0]) / (1 + temp_k * line[0])));
                angleRad = angleRad * 180.0 / PI;
                if(label[i] == flag || label[i] == flag - 1 || label[i] == flag + 1) is_segment = 1.0;
                if (angleRad > 70) is_angle = 0.3;
                // 计算点到直线距离
                distance = std::abs(distance_y[i] - line[0] * distance_x[i] - line[1]) / std::sqrt(line[0] * line[0] + 1);
                // 计算概率：与信息素浓度成正比，与距离成反比(第二个是启发式信息)
                select_citys_prob[i] = pow(pheromone_graph[current_city][i], ALPHA)
                    * pow(1.0 / distance_graph[current_city][i], BETA) * is_angle * is_segment;
                    //* pow(1.0 / distance, C);
            }
            catch (const std::exception& e) {
                std::cerr << "Ant ID: " << ID << ", current city: " << current_city << ", target city: " << i << std::endl;
                std::exit(1);
            }
        }
    }

    // 选择概率最高的城市
    double max_prob = 0.0;
    for (int i = 0; i < select_citys_prob.size(); i++) {
        if (open_table_city[i] && select_citys_prob[i] > max_prob) {
            max_prob = select_citys_prob[i];
            next_city = i;
        }
    }
    
    return next_city;
}

// 计算权重方差
void Ant::cal_weight_variance() 
{
    for (const auto& raw : all_way) {
        double tempWeight = 0.0;
        for (int i = 1; i < raw.size(); i++) {
            tempWeight += need[raw[i]];
		}
        total_weight_variance += pow(tempWeight - meanWeight, 2);
	}
}

// 计算路径方差
void Ant::cal_distance_variance() {
    double meanDistance = 0.0;
    meanDistance = total_distance / (city_num - 1);

    // 计算方差
    double variance = 0.0;
    for (const auto& raw : all_way) {
        for (int i = 0; i < raw.size() - 1; i++) {
            variance += pow(distance_graph[raw[i]][raw[i + 1]] - meanDistance, 2);
        }
    }
    variance /= (city_num - 1);
    total_distance_variance = variance;
}

// 计算路径总距离
void Ant::cal_total_distance()
{
    double temp_distance = 0.0;
    for (const auto& raw : all_way) {
        for (int i = 0; i < raw.size() - 1; i++) {
            temp_distance += distance_graph[raw[i]][raw[i+1]];
        }
    }
    total_distance = temp_distance;
}

// 蚂蚁在城市间移动
void Ant::move(const int next_city)
{
    path.push_back(next_city);
    open_table_city[next_city] = false;
    now_height += need[next_city];
    current_distance += distance_graph[current_city][next_city];
    current_city = next_city;
    move_count--;
}

// 蚂蚁返回初始点
void Ant::move_origin()
{
    all_way.push_back(path);
    clean_data();
}

// 搜索路径
void Ant::search_path() {
    clean_data();
    // 搜素路径，遍历path完所有城市为止   每条路径上点数大于8
    int next_city = -1;
    int flag = 0;
    while (move_count > 1) {
        if (path.size() == 1) {
            next_city = choice_first_city();
            flag = label[next_city];
        }
        else{
            // 计算直线k,b  k=(y2 - y1) / (x2 - x1)   b = y1 - k * x1
            int len = path.size();
            line[0] = (distance_y[path[len - 2]] - distance_y[path[len - 1]]) / (distance_x[path[len - 2]] - distance_x[path[len - 1]]);
            line[1] = distance_y[path[len - 2]] - line[0] * distance_x[path[len - 2]];
            next_city = choice_next_city(flag);
        }


        if (next_city == -1) {
            move_origin();
           /* for (const auto& val : path) {
                open_table_city[val] = true;
			}
            move_count += (path.size() - 1);
            clean_data();*/
        } else if (now_height + need[next_city] < random_height &&
            current_distance + distance_graph[current_city][next_city] <= top_distance) {
            move(next_city);
        } else {
            move_origin();
        }
    }
    move_origin();
    cal_total_distance();
    //cal_distance_variance();
    //cal_weight_variance();
}
