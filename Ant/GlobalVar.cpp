#include "GlobalVar.h"

//ALPHA:信息启发因子，值越大，则蚂蚁选择之前走过的路径可能性就越大
//，值越小，则蚁群搜索范围就会减少，容易陷入局部最优
//BETA : Beta值越大，蚁群越就容易选择局部较短路径，这时算法收敛速度会
//加快，但是随机性不高，容易得到局部的相对最优
//RH0 : 挥发率
//Q : 每条路径上释放的信息素Q / 路径长度    
float ALPHA = 3.0f;  // 表示信息素的重要程度 [1，4]
float BETA = 3.0f;  //  表示启发式因子的重要程度  [3，5]	
float RHO = 0.20f;  //  信息素的挥发速度 0~1   122-133
float Q = 100.0f;  
int MAX_GEN = 60;  // 运行代数  60

int ant_num = 123;  // 蚂蚁数量   123个城市  
int top_height = 13;  // 最大载重量
int top_distance = 400000000;  // 最大行驶距离
int thread_num = 10;  // 线程数量
std::vector<std::vector<int>> all_way;  // 记录所有最终路径
std::vector<double> total_way(MAX_GEN, 0);

//vector<double> distance_y = { 13.1, 8.5, 3.4, 16.6, 15.2, 11.6, 10.6, 7.6, 8.4, 2.1, 5.2, 16.9, 2.6, 8.7, 11, 1, 2.8, 19.8, 15.1, 5.6, 14.8 };
//vector<double> distance_x = { 14.2, 12.8, 18.4, 15.4, 18.9, 15.5, 3.9, 10.6, 8.6, 12.5, 13.8, 6.7, 14.8, 1.8, 17.1, 7.4, 0.2, 11.9, 13.2, 6.4, 9.6 };

// 城市距离和信息素
int city_num;
vector<double> distance_x;
vector<double> distance_y;
vector<int> need;
vector<vector<double>> distance_graph;
vector<vector<double>> pheromone_graph;