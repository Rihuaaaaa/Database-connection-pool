#include <iostream>
#include <memory>
#include <string>
#include "MysqlConn.h"
#include "ConnectPool.h"
using namespace std;
// // 1. ���߳�: ʹ��/��ʹ�����ӳ�
// // 2. ���߳�: ʹ��/��ʹ�����ӳ�

// void op1(int begin, int end)
// {
//     for (int i = begin; i < end; ++i)
//     {
//         MysqlConn conn;
//         conn.connect("root", "root", "testdb", "192.168.237.131");
//         char sql[1024] = { 0 };
//         sprintf(sql, "insert into person values(%d, 25, 'man', 'tom')", i);
//         conn.update(sql);
//     }
// }

// void op2(ConnectionPool* pool, int begin, int end)
// {
//     for (int i = begin; i < end; ++i)
//     {
//         shared_ptr<MysqlConn> conn = pool->getConnection();
//         char sql[1024] = { 0 };
//         sprintf(sql, "insert into person values(%d, 25, 'man', 'tom')", i);
//         conn->update(sql);
//     }
// }

// void test1()
// {
// #if 1
//     // �����ӳ�, ���߳�, ��ʱ: 21037278300 ����, 21037 ����
//     steady_clock::time_point begin = steady_clock::now();
//     op1(0, 5000);
//     steady_clock::time_point end = steady_clock::now();
//     auto length = end - begin;
//     cout << "�����ӳ�, ���߳�, ��ʱ: " << length.count() << " ����, "
//         << length.count() / 1000000 << " ����" << endl;
// #else
//     // ���ӳ�, ���߳�, ��ʱ: 8838406500 ����, 8838 ����
//     ConnectionPool* pool = ConnectionPool::getConnectPool();
//     steady_clock::time_point begin = steady_clock::now();
//     op2(pool, 0, 5000);
//     steady_clock::time_point end = steady_clock::now();
//     auto length = end - begin;
//     cout << "���ӳ�, ���߳�, ��ʱ: " << length.count() << " ����, "
//         << length.count() / 1000000 << " ����" << endl;

// #endif
// }

// void test2()
// {
// #if 0
//     // �����ӳ�, �൥�߳�, ��ʱ: 13277417000 ����, 13277 ����
//     MysqlConn conn;
//     conn.connect("root", "root", "testdb", "192.168.237.131");
//     steady_clock::time_point begin = steady_clock::now();
//     thread t1(op1, 0, 1000);
//     thread t2(op1, 1000, 2000);
//     thread t3(op1, 2000, 3000);
//     thread t4(op1, 3000, 4000);
//     thread t5(op1, 4000, 5000);
//     t1.join();
//     t2.join();
//     t3.join();
//     t4.join();
//     t5.join();
//     steady_clock::time_point end = steady_clock::now();
//     auto length = end - begin;
//     cout << "�����ӳ�, �൥�߳�, ��ʱ: " << length.count() << " ����, "
//         << length.count() / 1000000 << " ����" << endl;

// #else
//     // ���ӳ�, �൥�߳�, ��ʱ: 3938502100 ����, 3938 ����
//     ConnectionPool* pool = ConnectionPool::getConnectPool();
//     steady_clock::time_point begin = steady_clock::now();
//     thread t1(op2, pool, 0, 1000);
//     thread t2(op2, pool, 1000, 2000);
//     thread t3(op2, pool, 2000, 3000);
//     thread t4(op2, pool, 3000, 4000);
//     thread t5(op2, pool, 4000, 5000);
//     t1.join();
//     t2.join();
//     t3.join();
//     t4.join();
//     t5.join();
//     steady_clock::time_point end = steady_clock::now();
//     auto length = end - begin;
//     cout << "���ӳ�, �൥�߳�, ��ʱ: " << length.count() << " ����, "
//         << length.count() / 1000000 << " ����" << endl;

// #endif
// }

int query()
{
    MysqlConn conn; // ���������������� Mysql������
    conn.connect("rootname", "password", "testdb", "ip");     
    
    string sql = "insert into personl values(1, 24,'man', 'firsttest')";
    bool flag = conn.update(sql);   //update �����Ƕ���õĸ������
    cout<< flag << endl;
    
    sql = "select * from personl";
    conn.query(sql);   //ִ���� query�� conn�ᱣ������

    while (conn.next())
    {
        cout << conn.value(0) << endl;  //value()����ȡ�����ֶε�ֵ
    }
     return 0;
}



int main()
{
    query();
    return 0;
}