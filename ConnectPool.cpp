#include "ConnectPool.h"
#include <json/json.h>      //ʹ��json���ݸ�ʽ�������������ݿ����������Ӧ�ĸ���������Ϣ
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
        lock_guard<mutex> locker(m_mutexQ);     //C++11������һ��ģ���࣬ʹ������࣬���Լ򻯻����� lock() �� unlock() ��д����ͬʱҲ����ȫ,ȱ������޷����������ķ�Χ��
        conn->refreshAliveTime();
        m_connectionQueue.push(conn);
        }
    );     //��������ָ��ɾ���� �����б�����this��ʾ��������еı���
    m_connectionQueue.pop();
    m_cond.notify_all();
    return connptr;
}

ConnectionPool::~ConnectionPool()
{
    while (!m_connectionQueue.empty())
    {
        MysqlConn* conn = m_connectionQueue.front();    //�����е�����ʵ������ȡ���������ͷ�
        m_connectionQueue.pop();
        delete conn;
    }
}


ConnectionPool::ConnectionPool()        //���캯��
{
    // ����json�����ļ�
    if (!parseJsonFile())
    {
        return;
    }

    for (int i = 0; i < m_minSize; ++i)
    {
        addConnection();        //��ʼ�� m_minSize�����ݿ�����
    }

    thread producer(&ConnectionPool::produceConnection, this);  //���߳��������ݿ�����,������ָ�����̹߳�����������
    thread recycler(&ConnectionPool::recycleConnection, this);  //���������Ҫ���ٵ����ݿ�����

    //�̷߳���
    producer.detach();
    recycler.detach();
}


void ConnectionPool::addConnection()
{
    MysqlConn* conn = new MysqlConn;    //һ�����Ӷ�Ӧһ�� MysqlConn���� 
    conn->connect(m_user, m_passwd, m_dbName, m_ip, m_port);    //���������ļ��Ѿ��õ�����Щ����

    conn->refreshAliveTime();   //��¼��������ʱ��ʱ���

    m_connectionQueue.push(conn);   //�����ӳ� pushһ������ʵ��
}


void ConnectionPool::produceConnection()    
{
    while (true)    //ѭ���������ݿ�����
    {
        unique_lock<mutex> locker(m_mutexQ);
        while (m_connectionQueue.size() >= m_minSize)
        {
            m_cond.wait(locker);    //���ӳ������Ѿ����ˣ� ������
        }

        addConnection();    //����������������ݿ����ӡ�
        m_cond.notify_all();
    }
}

void ConnectionPool::recycleConnection()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(500));  //ָ������ʱ��

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
