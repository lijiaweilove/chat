#pragma once
#include <iostream>
#include "BaseShm.h"
#include "SeckeyNodeInfo.h"

class SeckeyShm : public BaseShm
{
public:
	// 通过传递进来的key创建/打开共享内存
	SeckeyShm(int key, int maxNode);
	// 通过路径创建/打开共享内存
	SeckeyShm(std::string name, int maxNode);
	~SeckeyShm();

	void shmInit();
	int shmWrite(SeckeyNodeInfo* pNodeInfo);
	SeckeyNodeInfo shmRead(std::string clientID, std::string serverID);
private:
	int m_maxNode;
};

