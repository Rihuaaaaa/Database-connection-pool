#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "MysqlConn.h"
using namespace std;
class ConnectionPool
{
public:
    static ConnectionPool* getConnectPool();

    ConnectionPool(const ConnectionPool& obj) = delete;   //ɾ��Ĭ�Ͽ�������
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;  //ɾ��Ĭ�ϸ�ֵ����

    shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();

private:
    ConnectionPool();   //����ʽ��������Ĭ�ϲ���ʵ��������ʲôʱ����ʲôʱ��new��

    bool parseJsonFile();       //����json�ļ��ĺ���������Ҫ��¶���û�
    
    //�������߳�ִ�к���
    void produceConnection();
    void recycleConnection();
    
    //��ʼ�� m_minSize�����ݿ����Ӻ���
    void addConnection();

    //����������Ҫ������ ������ MysqlConn�ࣨʹ��json�ļ�����һ��9�����ݣ�
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short m_port;
    //���ݿ����ӳ�������
    int m_minSize;  
    int m_maxSize;  
    //���ӳ�ʱʱ�� �����Ŀ���ʱ��
    int m_timeout;
    int m_maxIdleTime;


    //���ӳأ��洢��ʵ������д�õ� MysqlConn*����
    queue<MysqlConn*> m_connectionQueue;     

    //������Դ�������ӳض��У������Ҫ�������������������ӳ�
    mutex m_mutexQ;             
    condition_variable m_cond;
};

