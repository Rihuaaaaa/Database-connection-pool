#include "MysqlConn.h"

MysqlConn::MysqlConn()
{
    m_conn = mysql_init(nullptr);
    //mysql_set_character_set(m_conn, "utf8");    //设置接口使用的编码
}

MysqlConn::~MysqlConn()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
    freeResult();
}

bool MysqlConn::connect(string user, string passwd, string dbName, string ip, unsigned short port)
{
    //c_str表示把string转换为char *，返回的指针类型与 m_conn指向的地址是相同的。
    MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
    
    return ptr != nullptr;
}

bool MysqlConn::update(string sql)      
{
    if (mysql_query(m_conn, sql.c_str()))   
    {
        return false;
    }
    return true;
}

bool MysqlConn::query(string sql)   //传入的是一个 select语句
{
    freeResult();       //上一次查询结束后需要先情况一次结果集。
    if (mysql_query(m_conn, sql.c_str()))   
    {
        return false;
    }
    m_result = mysql_store_result(m_conn);  //查询成功后会返回一个结果集， 需要用 MYSQL_RES * 类型进行接受。
    return true;
}

bool MysqlConn::next()  //要遍历的记录就在 m_result 指针指向的内存里。
{
    if (m_result != nullptr)
    {
        m_row = mysql_fetch_row(m_result);      // MYSQL_ROW 类型是一个二级指针，返回的就是一个指针数据
        if (m_row != nullptr)
        {
            return true;
        }
    }
    return false;
}

string MysqlConn::value(int index)
{
    int rowCount = mysql_num_fields(m_result);
    if (index >= rowCount || index < 0)
    {
        return string();    //直接返回空字符串
    }
    char* val = m_row[index];
    unsigned long length = mysql_fetch_lengths(m_result)[index];    //函数实现的是得到对应字段的长度，函数返回的是一个数组
    return string(val, length);
}

bool MysqlConn::transaction()
{
    return mysql_autocommit(m_conn, false);     //false 表明为自动提交
}

bool MysqlConn::commit()
{
    return mysql_commit(m_conn);
}

bool MysqlConn::rollback()
{
    return mysql_rollback(m_conn);
}



void MysqlConn::refreshAliveTime()
{
    m_alivetime = steady_clock::now();
}

long long MysqlConn::getAliveTime()
{
    nanoseconds res = steady_clock::now() - m_alivetime;        
    milliseconds millsec = duration_cast<milliseconds>(res);    //纳秒转换成毫秒
    return millsec.count();
}




void MysqlConn::freeResult()
{
    if (m_result)
    {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}
