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
    // ��ʼ�����ݿ�����
    MysqlConn();
    // �ͷ����ݿ�����
    ~MysqlConn();

    // �������ݿ�
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306); //Ĭ�϶˿ھ���3306
    
    // �������ݿ�: insert, update, delete ������붼��string����,Ҳ��������Ҫ��װ������
    bool update(string sql);
    
    // ��ѯ���ݿ�
    bool query(string sql);
    
    // ������ѯ�õ��Ľ���� ÿ����һ��next�������ʹӽ������ȡ��һ����¼
    bool next();
    
    // �õ�������е��ֶ�ֵ �� ��¼������ܶ���ֶΣ���Ҫȡ��ÿһ���ֶε�ֵ
    string value(int index);
    
    // �������
    bool transaction();
    
    // �ύ����
    bool commit();
    
    // ����ع� 
    bool rollback();
    

    // ˢ����ʼ�Ŀ���ʱ���
    void refreshAliveTime();
    // �������Ӵ�����ʱ��
    long long getAliveTime();

private:

    void freeResult();      //��Ҫ���һ������ר�Ŷ� MYSQL_RES* ����ַ�����ͷţ���Ϊÿһ�β�ѯ�����Ҫ����һ���ͷţ���������Ҫ�ṩ��ʹ���ߣ����private

    MYSQL* m_conn = nullptr;
    MYSQL_RES* m_result = nullptr;
    MYSQL_ROW m_row = nullptr;

    steady_clock::time_point m_alivetime;       
};

