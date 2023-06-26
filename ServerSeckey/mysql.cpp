#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
using namespace std;

int mysqlDB() {
    const char* host = "tcp://127.0.0.1:3306/mydb";
    const char* user = "root";
    const char* passwd = "981213";
    // 获取数据库驱动并连接数据库
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    sql::Connection* conn = driver->connect(host, user, passwd);
    if (conn != nullptr && conn->isClosed() == 0) {
        cout << "数据库连接成功！" << endl;
    } 
    // 执行插入
    const sql::SQLString sqlInsert = "INSERT INTO stuInfo(id, name, age) VALUES(4,'小明', 30);";
    sql::PreparedStatement* pstmt = conn->prepareStatement(sqlInsert);
    int ret = pstmt->executeUpdate();
    if (ret != 1) {
        cout << "插入失败！" << endl;
    }
    pstmt->close();
    //  修改数据
    sql::Statement* stmt = conn->createStatement();
    sql::SQLString sqlUpdate = "UPDATE stuInfo SET name = '天天向上' WHERE id = 3;";
    int effectRows = stmt->executeUpdate(sqlUpdate);
    //  删除数据
    sql::SQLString sqlDelete = "DELETE FROM stuInfo WHERE id = 4;";
    bool flag = stmt->execute(sqlDelete);
    // 执行查询
    sql::SQLString sqlSelect = "SELECT * FROM stuInfo";
    sql::ResultSet* res = stmt->executeQuery(sqlSelect);
    // 遍历结果集并输出数据
    while (res->next()) {
        int id = res->getInt("id");
        string name = res->getString("name");
        int age = res->getInt("age");
        cout << "ID: " << id << ", Name: " << name << ", Age: " << age << endl;
    }
    res->close();
    // 释放资源
    stmt->close();
    conn->close();
    delete conn;
}
