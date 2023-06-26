#pragma once
#include <iostream>

class BaseShm
{
public:
	// 通过key打开共享内存
	BaseShm(int key);
	// 通过传递进来的key创建/打开共享内存
	BaseShm(int key, int size);
	// 通过路径打开共享内存
	BaseShm(std::string name);
	// 通过路径创建/打开共享内存
	BaseShm(std::string name, int size);
	~BaseShm();
	void* mapShm();
	int unmapShm();
	int delShm();
private:
	int getShmID(key_t key, int shmSize, int flag);
private:
	int m_shmID;
protected:
	void* m_shmAddr;
};

