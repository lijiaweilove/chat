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


void plot(std::vector<std::vector<int>> &best_path) {
    // 绘制节点
    //for (auto& j: segment_max) {
    //    plt::plot({ distance_x[j] }, { distance_y[j] }, "bo");  // 'bo'表示蓝色圆点
    //}
    for (int i = 0; i < city_num; i++) {
        plt::plot({ distance_x[i] }, { distance_y[i] }, "bo");  // 'bo'表示蓝色圆点
    }
    // 绘制车辆路径
    int n = 0;
    std::vector<std::vector<double>> x_vehicle(best_path.size());
    std::vector<std::vector<double>> y_vehicle(best_path.size());
    std::vector<std::string> colors = { "b", "g", "r", "c", "m", "y", "k","orange", "purple", 
        "brown", "pink", "gray", "olive", "teal" ,"silver", "gold", "navy", "lavender", "turquoise",
        "indigo", "maroon", "lime", "orchid", "salmon", "cyan" };
    for (int i = 0; i < best_path.size(); i++) {
        for (const auto& way : best_path[i]) {
            x_vehicle[i].push_back(distance_x[way]);
            y_vehicle[i].push_back(distance_y[way]);
        }
        plt::plot(x_vehicle[i], y_vehicle[i], colors[i]);
    }
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
    distance_x.resize(city_num, 0.0);
    distance_y.resize(city_num, 0.0);
    need.resize(city_num, 1);
    need[0] = 0;
    // 计算平均需求
    for (int i = 1; i < city_num;i++) {
        meanWeight += need[i];
    }
    meanWeight /= top_height;
    distance_graph.resize(city_num, vector<double>(city_num, 0.0));
    pheromone_graph.resize(city_num, vector<double>(city_num, 1.0));
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);
    double x = 0.0, y = 0.0;
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
    std::vector<std::vector<int>> best_path = vrp->best_ant->all_way; // 得出的最优TSP路径
    std::cout << "总距离:" << vrp->best_ant->total_distance << std::endl;
    //std::cout << "最优路径：" << std::endl;
    //for (const auto& row : best_path) {
    //    for (int value : row) {
    //        std::cout << value << " ";
    //    }
    //    std::cout << std::endl;
    //}

    // 保存路径
    std::ofstream outFile("route.txt");
    if (!outFile.is_open()) {
		std::cout << "无法打开文件" << std::endl;
		return 1;
	}
    for (const auto& row : best_path) {
        for (int value : row) {
			outFile << value << " ";
		}
		outFile << std::endl;
	}
    outFile.close();

    //  计算时间
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double timeInSeconds = duration.count();
    std::cout << "代码运行时间：" << timeInSeconds << "秒" << std::endl;

    //画迭代图
    //std::vector<int> iteration_nums(MAX_GEN,0);
    //for (int i = 0; i < MAX_GEN; i++) {
    //    iteration_nums[i] = i;
    //}
    //plt::plot(iteration_nums, total_way, "b-");
    //plt::ylim(1, 30000);
    //plt::show();
    
    //  画结果图
    std::cout << best_path.size() << std::endl;
    plot(best_path);
    return 0;
}