#include <iostream>
#include "clientOP.h"
using namespace std;


int usage() {
    int nSel = -1;
    cout << "/************************/" << endl;
    cout << "/ 1.密钥协商  2.密钥校验" << endl;
    cout << "/ 3.密钥注销  4.密钥查看" << endl;
    cout << "/ 5.退出" << endl;
    cout << "/************************/" << endl;
    cin >> nSel;
    while (cin.get() != '\n');  //清空输入缓冲区
    return nSel;
}


int main()
{
    clientOP client;
    while (1) {
        int num = usage();
        switch (num) {
        case 1:
            client.seckeyAgree();
            break;
        case 2:
            client.seckeyCheck();
            break;
        case 3:
            client.seckeyCancel();
            break;
        case 4:
            cout << "密钥查看" << endl;
            break;
        default:
            break;
        }
        cout << "客户端退出" << endl;
        return 0;
    }
}