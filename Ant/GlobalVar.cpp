#include "GlobalVar.h"

//ALPHA:信息启发因子，值越大，则蚂蚁选择之前走过的路径可能性就越大
//，值越小，则蚁群搜索范围就会减少，容易陷入局部最优
//BETA : Beta值越大，蚁群越就容易选择局部较短路径，这时算法收敛速度会
//加快，但是随机性不高，容易得到局部的相对最优
//RH0 : 挥发率
//Q : 每条路径上释放的信息素Q / 路径长度    
const double ALPHA = 4.0;  // 信息素的重要程度 [1，4]
const double BETA = 2.0;  //  距离倒数的重要程度  [3，5]	
const double C = 2.0;  //  点到直线距离倒数的重要程度  [5，10]
const double RHO = 0.20;  //  信息素的挥发速度 0~1   122-133
const double Q = 100.0;
const int MAX_GEN = 60;  // 运行代数  60

const int ant_num = 123;  // 蚂蚁数量   123个城市  
const int top_height = 8;  // 最大载重量
const int low_height = 7;  // 最小载重量
const int top_distance = 10000;  // 最大行驶距离   11390  9493
const int thread_num = 10;  // 线程数量
const int segment_num = 16;  // 扇区数量



double meanWeight;  // 平均载重量
const double PI = std::atan(1.0) * 4.0;
std::unordered_set<int> segment_max;  // 每个扇区的最远城市
std::vector<int> label;  // 当前点属于第几个扇形
std::vector<double> total_way(MAX_GEN, 0);
// 城市距离和信息素
int city_num;
vector<double> distance_x;  // 坐标
vector<double> distance_y;
vector<int> need;
vector<vector<double>> distance_graph;
vector<vector<double>> pheromone_graph;