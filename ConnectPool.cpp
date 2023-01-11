#include "ConnectPool.h"
#include <json/json.h>      //使用json数据格式来处理连接数据库服务器所对应的各种连接信息
#include <fstream>
#include <thread>
using namespace Json;


ConnectionPool* ConnectionPool::getConnectPool()
{
    static ConnectionPool pool;
    return &pool;
}


bool ConnectionPool::parseJsonFile()
{
    ifstream ifs("dbconf.json");

    Reader rd;
    Value root;
    rd.parse(ifs, root);
    if (root.isObject())
    {
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["userName"].asString();
        m_passwd = root["password"].asString();
        m_dbName = root["dbName"].asString();
        m_minSize = root["minSize"].asInt();
        m_maxSize = root["maxSize"].asInt();
        m_maxIdleTime = root["maxIdleTime"].asInt();
        m_timeout = root["timeout"].asInt();
        return true;
    }
    return false;
}


shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connectionQueue.empty())
    {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))
        {
            if (m_connectionQueue.empty())
            {
                //return nullptr;
                continue;
            }
        }
    }
    shared_ptr<MysqlConn> connptr(m_connectionQueue.front(), [this](MysqlConn* conn) {
        lock_guard<mutex> locker(m_mutexQ);     //C++11新增的一个模板类，使用这个类，可以简化互斥锁 lock() 和 unlock() 的写法，同时也更安全,缺点就是无法控制锁定的范围。
        conn->refreshAliveTime();
        m_connectionQueue.push(conn);
        }
    );     //匿名函数指定删除器 捕获列表里用this表示捕获该类中的变量
    m_connectionQueue.pop();
    m_cond.notify_all();
    return connptr;
}

ConnectionPool::~ConnectionPool()
{
    while (!m_connectionQueue.empty())
    {
        MysqlConn* conn = m_connectionQueue.front();    //队列中的连接实例依次取出来进行释放
        m_connectionQueue.pop();
        delete conn;
    }
}


ConnectionPool::ConnectionPool()        //构造函数
{
    // 加载json配置文件
    if (!parseJsonFile())
    {
        return;
    }

    for (int i = 0; i < m_minSize; ++i)
    {
        addConnection();        //初始化 m_minSize个数据库连接
    }

    thread producer(&ConnectionPool::produceConnection, this);  //子线程生产数据库连接,参数即指定子线程工作的任务函数
    thread recycler(&ConnectionPool::recycleConnection, this);  //检测有无需要销毁的数据库连接

    //线程分离
    producer.detach();
    recycler.detach();
}


void ConnectionPool::addConnection()
{
    MysqlConn* conn = new MysqlConn;    //一个连接对应一个 MysqlConn对象 
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);    //加载配置文件已经得到了这些属性

    conn->refreshAliveTime();   //记录创建连接时的时间戳

    m_connectionQueue.push(conn);   //往连接池 push一个连接实例
}


void ConnectionPool::produceConnection()    
{
    while (true)    //循环生产数据库连接
    {
        unique_lock<mutex> locker(m_mutexQ);
        while (m_connectionQueue.size() >= m_minSize)
        {
            m_cond.wait(locker);    //连接池数量已经够了， 阻塞。
        }

        addConnection();    //如果不够，生产数据库连接。
        m_cond.notify_all();
    }
}

void ConnectionPool::recycleConnection()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(500));  //指定休眠时间

        lock_guard<mutex> locker(m_mutexQ);
        while (m_connectionQueue.size() > m_minSize)
        {
            MysqlConn* conn = m_connectionQueue.front();
            if (conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connectionQueue.pop();
                delete conn;
            }
            else
            {
                break;
            }
        }
    }
}
