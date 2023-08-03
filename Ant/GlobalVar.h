#pragma once
#include <vector>
#include <unordered_set>
using std::vector;

//ALPHA:信息启发因子，值越大，则蚂蚁选择之前走过的路径可能性就越大
//，值越小，则蚁群搜索范围就会减少，容易陷入局部最优
//BETA : Beta值越大，蚁群越就容易选择局部较短路径，这时算法收敛速度会
//加快，但是随机性不高，容易得到局部的相对最优
//RH0 : 挥发率
//Q : 每条路径上释放的信息素Q / 路径长度

extern const double ALPHA;
extern const double BETA;
extern const double C;
extern const double RHO;
extern const double Q;
extern const int MAX_GEN;
extern const int ant_num; 
extern const int top_height;
extern const int low_height;
extern const int top_distance;  
extern const int thread_num;
extern double meanWeight;
extern const int segment_num;
extern const double PI;
extern std::unordered_set<int> segment_max;
extern std::vector<int> label;
extern std::vector<double> total_way;

extern int city_num;
extern vector<double> distance_x;
extern vector<double> distance_y;
extern vector<int> need;
extern vector<vector<double>> distance_graph;
extern vector<vector<double>> pheromone_graph;

