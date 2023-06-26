#include "SeckeyShm.h"
#include <sys/shm.h>

SeckeyShm::SeckeyShm(int key, int maxNode) :BaseShm(key, maxNode * sizeof(SeckeyNodeInfo)), m_maxNode(maxNode)
{
}

SeckeyShm::SeckeyShm(std::string name, int maxNode) :BaseShm(name, maxNode), m_maxNode(maxNode)
{
}

SeckeyShm::~SeckeyShm()
{
}

void SeckeyShm::shmInit()
{
    if (m_shmAddr != NULL) {
        memset(m_shmAddr, 0, sizeof(SeckeyNodeInfo));
    }
}

int SeckeyShm::shmWrite(SeckeyNodeInfo* pNodeInfo)
{
    int ret = -1;
    //关联共享内存
    SeckeyNodeInfo* pAddr = static_cast<SeckeyNodeInfo*>(mapShm());
    if (pAddr == nullptr) {
        return ret;
    }
    SeckeyNodeInfo* pNode = nullptr;
    //判断传入的秘钥是否已经存在
    for (int i = 0; m_maxNode; i++) {
        pNode += i;
        if (strcmp(pNodeInfo->clientID, pNode->clientID) == 0 &&
            strcmp(pNodeInfo->serverID, pNode->serverID)) {
            memcpy(pNode, pNodeInfo, sizeof(pNodeInfo));
            unmapShm();
            return 0;
        }
    }
    // 若没有找到对应的信息, 找一个空节点将秘钥信息写入
    SeckeyNodeInfo tempNode;
    //判断传入的秘钥是否已经存在
    int i = 0;
    for (i = 0; m_maxNode; i++) {
        pNode += i;
        if (memcmp(&tempNode, pNode, sizeof(SeckeyNodeInfo)) == 0) {
            ret = 0;
            memcpy(pNode, pNodeInfo, sizeof(pNodeInfo));
            break;
        }
    }
    if (i == m_maxNode) return -1;
    unmapShm();
    return ret;
}

SeckeyNodeInfo SeckeyShm::shmRead(std::string clientID, std::string serverID)
{
    //关联共享内存
    SeckeyNodeInfo* pAddr = static_cast<SeckeyNodeInfo*>(mapShm());
    if (pAddr == nullptr) {
        return SeckeyNodeInfo();
    }
    SeckeyNodeInfo info;
    SeckeyNodeInfo* pNode = nullptr;
    //循环遍历读取
    for (int i = 0; m_maxNode; i++) {
        pNode += i;
        if (strcmp(pNode->clientID, clientID.data()) == 0 &&
            strcmp(pNode->serverID, serverID.data()) == 0) {
            info = *pNode;
            break;
        }
    }
    unmapShm();
    return info;
}
