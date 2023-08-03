#include "Vrp.h"
#include <limits>
#include <iostream>
#include <mutex>
#include "ThreadPool.h"
#include "Astar.h"
std::mutex best_ant_mutex;
std::mutex pheromone_graph_mutex;


Vrp::Vrp():iter(1), ants(ant_num)
{
    //  计算城市之间的距离,创建蚂蚁对象
    for (int i = 0; i < city_num; ++i) {
        for (int j = 0; j < city_num; ++j) {
            double temp_distance = pow((distance_x[i] - distance_x[j]), 2) + pow((distance_y[i] - distance_y[j]), 2);
            distance_graph[i][j] = pow(temp_distance, 0.5);
        }
    }
    for (int ID = 0; ID < ant_num; ++ID) {
        ants[ID] = std::make_shared<Ant>(ID);
    }
    
    // 分扇区并找到最远点
    allocate_segment();

    //  初始最优解,最大距离
    best_ant = std::make_shared<Ant>(-1);
    best_ant->total_distance = std::numeric_limits<double>::max();
    //best_ant->total_distance_variance = std::numeric_limits<double>::max();
    //best_ant->total_weight_variance = std::numeric_limits<double>::max();
}


Vrp::~Vrp()
{
}

// 开始搜索
void Vrp::search_path()
{
    ThreadPool pool(thread_num);
    while (iter < MAX_GEN) {
        // 遍历每一只蚂蚁
        for (auto& ant : ants) {
            auto shared_ant = std::make_shared<Ant>(*ant);
            pool.enqueue([this, shared_ant] {
                shared_ant->search_path();  // 搜索一条路径
                // 与当前最优蚂蚁比较,需进行深拷贝,浅拷贝只是将引用计数加1
                best_ant_mutex.lock();
                if (shared_ant->total_distance < best_ant->total_distance) {
                    best_ant = std::make_shared<Ant>(*shared_ant);
                }
                best_ant_mutex.unlock();
                // 更新信息素
                update_pheromone_gragh(shared_ant);
          });
        }
        total_way[iter] = best_ant->total_distance / 10;
        iter++;
    }
}


void Vrp::update_pheromone_gragh(std::shared_ptr<Ant> ant)
{
    // 获取每只蚂蚁在其路径上留下的信息素   all_way为空
    vector<vector<double>> temp_pheromone(city_num, vector<double>(city_num, 0.0));
    double temp = Q / ant->total_distance;
    for (const auto& raw : ant->all_way) {
        if (segment_max.count(raw[raw.size() - 1]) > 0) {  // 判断路线终点是否为最远点
            temp = 2 * Q / (ant->total_distance);
		}
        for (int i = 1; i < raw.size(); i++) {
            int start = raw[i - 1];
            int end = raw[i];
            // 在路径上的每两个相邻城市间留下信息素，与路径总距离反比
            temp_pheromone[start][end] += temp;
            temp_pheromone[end][start] = temp_pheromone[start][end];
        }
    }
    // 更新所有城市之间的信息素，旧信息素衰减加上新迭代信息素, temp_pheromone是ij边上信息素的增量
    std::lock_guard<std::mutex> lock(pheromone_graph_mutex);
    for (int i = 0; i < city_num; i++) {
        for (int j = 0; j < city_num; j++) {
            pheromone_graph[i][j] = pheromone_graph[i][j] * (1 - RHO) + RHO * temp_pheromone[i][j];
        }
    }
}


void Vrp::allocate_segment()
{
    const double cx = distance_x[0], cy = distance_y[0];  // 原点坐标
    label.resize(city_num, -1);

    const double segment_step = (2 * PI) / segment_num;
    // 分配扇区
    for (unsigned int i = 1; i < city_num; ++i)
    {
        double angle = std::atan2(distance_y[i] - cy, distance_x[i] - cx);  // 点与原点之间夹角-PI~PI
        if (angle < 0) angle += 2 * PI;  // 映射到0~2π范围
        const unsigned int segment_index = angle / segment_step;
        label[i] = segment_index + 1;
    }
    // 计算每个扇区最远点
    for (unsigned int i = 1; i < segment_num + 1; ++i) {
        int temp_segment = 0, temp_index = 0;
        for (unsigned int j = 1; j < city_num; ++j) {
            if (label[j] == i && distance_graph[0][j] > temp_segment) {
                temp_segment = distance_graph[0][j];
                temp_index = j;
            }
        }
        if (temp_index != 0) {
            segment_max.insert(temp_index);
        }
    }
}


//void Vrp::search_path()
//{
//    //设置起始和结束点
//    Point start(distance_x[0], distance_y[0]);
//    Point end(6, 10);
//    Astar astar();
//    std::vector<std::shared_ptr<Point>> path = astar.findPath(start, end);
//
//    if (!path.empty()) {
//        std::cout << "找到路径：\n";
//        for (const auto& node : path) {
//            std::cout << "(" << node->x << ", " << node->y << ") ";
//        }
//        std::cout << std::endl;
//    }
//    else {
//        std::cout << "未找到路径" << std::endl;
//    }
//
//}