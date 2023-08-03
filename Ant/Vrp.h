#pragma once
#include <string>
#include <memory>
#include "GlobalVar.h"
#include "Ant.h"

class Vrp
{
public:
    std::shared_ptr<Ant> best_ant;
private:
    vector <std::shared_ptr<Ant>> ants;
    int iter;
public:
	Vrp();
	~Vrp();
    void search_path();
    void update_pheromone_gragh(std::shared_ptr<Ant> ant);
private:
    void allocate_segment();
};

