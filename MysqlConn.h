#pragma once
#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <chrono>
using namespace std;
using namespace chrono;
class MysqlConn
{
public:
    // 初始化数据库连接
    MysqlConn();
    // 释放数据库连接
    ~MysqlConn();

    // 连接数据库
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306); //默认端口就是3306
    
    // 更新数据库: insert, update, delete 语句输入都是string类型,也就是我们要封装的类型
    bool update(string sql);
    
    // 查询数据库
    bool query(string sql);
    
    // 遍历查询得到的结果集 每调用一次next方法，就从结果集里取出一条记录
    bool next();
    
    // 得到结果集中的字段值 ， 记录里包含很多的字段，需要取出每一个字段的值
    string value(int index);
    
    // 事务操作
    bool transaction();
    
    // 提交事务
    bool commit();
    
    // 事务回滚 
    bool rollback();
    

    // 刷新起始的空闲时间点
    void refreshAliveTime();
    // 计算连接存活的总时长
    long long getAliveTime();

private:

    void freeResult();      //需要添加一个函数专门对 MYSQL_RES* 这块地址进行释放，因为每一次查询完后都需要进行一次释放，函数不需要提供给使用者，因此private

    MYSQL* m_conn = nullptr;
    MYSQL_RES* m_result = nullptr;
    MYSQL_ROW m_row = nullptr;

    steady_clock::time_point m_alivetime;       
};

