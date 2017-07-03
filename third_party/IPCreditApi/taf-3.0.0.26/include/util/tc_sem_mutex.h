#ifndef __TC_SEM_MUTEX_H
#define __TC_SEM_MUTEX_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "util/tc_lock.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
* @file tc_sem_mutex.h 
* @brief �ź�������. 
*  
* @author jarodruan@tencent.com 
*/              
/////////////////////////////////////////////////

/**
* @brief �ź������쳣��
*/
struct TC_SemMutex_Exception : public TC_Lock_Exception
{
    TC_SemMutex_Exception(const string &buffer) : TC_Lock_Exception(buffer){};
    TC_SemMutex_Exception(const string &buffer, int err) : TC_Lock_Exception(buffer, err){};
    ~TC_SemMutex_Exception() throw() {};
};

/**
* @brief ���̼���, �ṩ����������:���������ų���. 
*  
* 1 ������ͬ��key, ��ͬ���̳�ʼ��ʱ���ӵ���ͬ��sem��
* 2 ����IPC���ź���ʵ��
* 3 �ź���������SEM_UNDO����, �����̽���ʱ���Զ������ź���
*/
class TC_SemMutex
{
public:
    /**
     * @brief ���캯��
     */
    TC_SemMutex();

    /**
	* @brief ���캯��. 
	*  
    * @param iKey, key
    * @throws TC_SemMutex_Exception
    */
    TC_SemMutex(key_t iKey);

    /**
	* @brief ��ʼ��. 
	*  
    * @param iKey, key
    * @throws TC_SemMutex_Exception
    * @return ��
     */
    void init(key_t iKey);

    /**
	* @brief ��ȡ�����ڴ�Key. 
	*  
    * @return key_t ,�����ڴ�key
    */
    key_t getkey() const {return _semKey;}

    /**
	* @brief ��ȡ�����ڴ�ID. 
	*  
    * @return int ,�����ڴ�Id
    */
    int getid() const   {return _semID;}

    /**
	* @brief �Ӷ���.
	* 
    *@return int
    */
    int rlock() const;

    /**
	* @brief �����. 
	*  
    * @return int
    */
    int unrlock() const;

    /**
	* @brief ���Զ���. 
	*  
    * @return bool : �����ɹ��򷵻�false, ���򷵻�false
    */
    bool tryrlock() const;

    /**
	* @brief ��д��. 
	*  
    * @return int
    */
    int wlock() const;

    /**
    * @brief ��д��
    */
    int unwlock() const;

    /**
	* @brief ����д��. 
	*  
    * @throws TC_SemMutex_Exception
    * @return bool : �����ɹ��򷵻�false, ���򷵻�false
    */
    bool trywlock() const;

    /**
	* @brief д��. 
	*  
    * @return int, 0 ��ȷ
    */
    int lock() const        {return wlock();};

    /**
    * @brief ��д��
    */
    int unlock() const      {return unwlock();};

    /**
	* @brief  ���Խ���. 
	*  
    * @throws TC_SemMutex_Exception
    * @return int, 0 ��ȷ
    */
    bool trylock() const    {return trywlock();};

protected:
    /**
     * �ź���ID
     */
    int _semID;

    /**
     * �ź���key
     */
    key_t _semKey;
};

}

#endif
