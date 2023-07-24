#include <iostream>
#include <memory>
#include <chrono>
#include <string>
#include <fstream>
#include <random>
#include "Vrp.h"
#include "GlobalVar.h"
#include "Ant.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;


void plot() {
    // 绘制节点
    for (int i = 0; i < city_num; i++) {
        plt::plot({ distance_x[i] }, { distance_y[i] }, "bo");  // 'bo'表示蓝色圆点
    }
    // 绘制车辆路径
    std::vector<std::string> colors = { "b", "g", "r", "c", "m", "y", "k","orange", "purple", 
        "brown", "pink", "gray", "olive", "teal" };
    int n = 0;
    std::vector<std::vector<double>> x_vehicle(all_way.size());
    std::vector<std::vector<double>> y_vehicle(all_way.size());
    for (int i = 0; i < all_way.size(); i++) {
        for (const auto& way : all_way[i]) {
            x_vehicle[i].push_back(distance_x[way]);
            y_vehicle[i].push_back(distance_y[way]);
        }
        plt::plot(x_vehicle[i], y_vehicle[i], colors[i]);
    }
    //// 添加标签
    //for (int i = 0; i < city_num; i++) {
    //    plt::annotate(std::to_string(i), distance_x[i], distance_y[i]);
    //}
    plt::xlabel("X");
    plt::ylabel("Y");
    plt::show();
}

int main()
{
    auto start = std::chrono::steady_clock::now();
    // 读取并初始化数据
    std::ifstream inputFile("output.txt");
    if (!inputFile.is_open()) {
        std::cout << "无法打开文件" << std::endl;
        return 1;
    }
    int lineCount = 0;
    std::string line;
    while (std::getline(inputFile, line)) {
        lineCount++;
    }
    city_num = lineCount;
    distance_x.resize(city_num, 0);
    distance_y.resize(city_num, 0);
    need.resize(city_num + 1, 1);
    need[0] = 0;
    distance_graph.resize(city_num, vector<double>(city_num, 0.0));
    pheromone_graph.resize(city_num, vector<double>(city_num, 1.0));
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);
    double x, y;
    int num = 0;
    while (inputFile >> x >> y) { 
        distance_x[num] = x;
        distance_y[num] = y;
        num++;
    }
    inputFile.close();

    // 蚁群算法
    std::shared_ptr<Vrp> vrp = std::make_shared<Vrp>();
    vrp->search_path();
    std::vector<int> best_path = vrp->best_ant->path; // 得出的最优TSP路径
  /*  std::cout << "最优路径：";
    for (int i : best_path) {
        std::cout << i << " ";
    }
    std::cout << std::endl;*/
    std::vector<int> final_way = { 0 }; // 记录最终路径
    int now_height = 0;  // 当前车的当前载重量
    double now_distance = 0.0f;  // 当前车的当前行驶的距离
    int k = 1;  // 记录用几辆车
    double all_distance = 0.0f; // 所有车合起来行驶的距离

    for (int i = 1; i < city_num; i++) {  //  分解TSP路径为VRP路径
        if (now_height == 0) {  // 从初始点出发
            now_height += need[best_path[i]];
            now_distance = distance_graph[0][best_path[i]];
            final_way.push_back(best_path[i]);
        }
        else if (now_height + need[best_path[i]] <= top_height &&
            (now_distance + distance_graph[best_path[i - 1]][best_path[i]]) <= top_distance) {
            // 可以前往下一地点(满足最大载重量和距离)
            final_way.push_back(best_path[i]);
            now_height += need[best_path[i]];
            now_distance += distance_graph[best_path[i - 1]][best_path[i]];
        }
        else {
            //  让上一辆车返回，并开始新的一辆
            all_way.push_back(final_way);
            std::cout << "第" << k << "辆车的路径为";
            for (const int& j : final_way) {
                std::cout << j << " ";
            }
            std::cout << "，载重" << now_height << "，行驶距离" << now_distance << std::endl;
            all_distance += now_distance;
            k++;
            now_height = need[best_path[i]];
            now_distance = distance_graph[0][best_path[i]];
            final_way = { 0, best_path[i] };
        }

        if (i == city_num - 1) { 
            std::cout << "第" << k << "辆车的路径为";
            for (int j : final_way) {
                std::cout << j << " ";
            }
            std::cout << "，载重" << now_height << "，行驶距离" << now_distance << std::endl;
            all_distance += now_distance;
        }
    }
    std::cout << "所有车合起来行驶的距离为" << all_distance << std::endl;  
    all_way.push_back(final_way);
    //  计算时间
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double timeInSeconds = duration.count();
    std::cout << "代码运行时间：" << timeInSeconds << "秒" << std::endl;

    // 画迭代图
    //std::vector<int> iteration_nums(MAX_GEN,0);
    //for (int i = 0; i < MAX_GEN; i++) {
    //    iteration_nums[i] = i;
    //}
    //plt::plot(iteration_nums, total_way, "b-");
    //plt::ylim(1, 20000);
    //plt::show();
    
    //  画结果图
    plot();
    return 0;
}