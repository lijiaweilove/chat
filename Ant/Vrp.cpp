#include "Vrp.h"
#include <limits>
#include <iostream>
#include "ThreadPool.h"


Vrp::Vrp():iter(1), ants(ant_num)
{
    //  计算城市之间的距离
    for (int i = 0; i < city_num; ++i) {
        for (int j = 0; j < city_num; ++j) {
            double temp_distance = pow((distance_x[i] - distance_x[j]), 2) + pow((distance_y[i] - distance_y[j]), 2);
            distance_graph[i][j] = pow(temp_distance, 0.5);
        }
    }
    for (int ID = 0; ID < ant_num; ++ID) {
        ants[ID] = std::make_shared<Ant>(ID);
    }
    //  初始最优解,最大距离
    best_ant = std::make_shared<Ant>(-1);
    best_ant->total_distance = std::numeric_limits<double>::max();
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
            pool.enqueue([this, &ant] {
                ant->search_path();  // 搜索一条路径
            // 与当前最优蚂蚁比较
            if (ant->total_distance < best_ant->total_distance) {
                // 更新最优解
                //不能简单的复制,会导致best_ant和ant共享同一个路径对象,只是将引用计数加1
                //当ant在下一次迭代中重新搜索路径时,它的路径会被修改,从而影响到best_ant
                best_ant = std::make_shared<Ant>(*ant);
            }
          });
        }
        total_way[iter] = best_ant->total_distance / 10;
        // 更新信息素
        update_pheromone_gragh();
        iter++;
    }
}

void Vrp::update_pheromone_gragh()
{
    // 获取每只蚂蚁在其路径上留下的信息素
    vector<vector<double>> temp_pheromone(city_num, vector<double>(city_num, 0.0));
    for (const auto& ant : ants) {
        for (int i = 1; i < city_num; i++) {
            int start = ant->path[i - 1];
            int end = ant->path[i];
            // 在路径上的每两个相邻城市间留下信息素，与路径总距离反比
            temp_pheromone[start][end] += Q / ant->total_distance;
            temp_pheromone[end][start] = temp_pheromone[start][end];
        }
    }

    // 更新所有城市之间的信息素，旧信息素衰减加上新迭代信息素, temp_pheromone是ij边上信息素的增量
    for (int i = 0; i < city_num; i++) {
        for (int j = 0; j < city_num; j++) {
            pheromone_graph[i][j] = pheromone_graph[i][j] * (1 - RHO) + RHO * temp_pheromone[i][j];
        }
    }
}
