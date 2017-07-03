#ifndef _TC_LOCK_H
#define _TC_LOCK_H

#include <string>
#include <stdexcept>
#include <cerrno>
#include "util/tc_ex.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
/**
 * @file tc_lock.h 
 * @brief  ���� 
 * @author  jarodruan@tencent.com 
 */           
/////////////////////////////////////////////////


/**
* @brief  ���쳣
*/
struct TC_Lock_Exception : public TC_Exception
{
    TC_Lock_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Lock_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Lock_Exception() throw() {};
};

/**
 * @brief  ��ģ�����������������ʹ�ã�
 * ����ʱ�������������ʱ�����
 */
template <typename T>
class TC_LockT
{
public:

    /**
	 * @brief  ���캯��������ʱ����
	 *  
     * @param mutex ������
     */
    TC_LockT(const T& mutex) : _mutex(mutex)
    {
        _mutex.lock();
        _acquired = true;
    }

    /**
     * @brief  ����������ʱ����
     */
    virtual ~TC_LockT()
    {
        if (_acquired)
        {
            _mutex.unlock();
        }
    }

    /**
     * @brief  ����, ����Ѿ�����,���׳��쳣
     */
    void acquire() const
    {
        if (_acquired)
        {
            throw TC_Lock_Exception("thread has locked!");
        }
        _mutex.lock();
        _acquired = true;
    }

    /**
     * @brief  ��������.
     *
     * @return  �ɹ�����true�����򷵻�false
     */
    bool tryAcquire() const
    {
        _acquired = _mutex.tryLock();
        return _acquired;
    }

    /**
     * @brief  �ͷ���, ���û���Ϲ���, ���׳��쳣
     */
    void release() const
    {
        if (!_acquired)
        {
            throw TC_Lock_Exception("thread hasn't been locked!");
        }
        _mutex.unlock();
        _acquired = false;
    }

    /**
     * @brief  �Ƿ��Ѿ�����.
     *
     * @return  ����true�Ѿ����������򷵻�false
     */
    bool acquired() const
    {
        return _acquired;
    }

protected:

    /**
	 * @brief ���캯��
	 * ���������Բ�������TC_LockT����
	 *  
     */
    TC_LockT(const T& mutex, bool) : _mutex(mutex)
    {
        _acquired = _mutex.tryLock();
    }

private:

    // Not implemented; prevents accidental use.
    TC_LockT(const TC_LockT&);
    TC_LockT& operator=(const TC_LockT&);

protected:
    /**
     * ������
     */
    const T&        _mutex;

    /**
     * �Ƿ��Ѿ�����
     */
    mutable bool _acquired;
};

/**
 * @brief  ��������
 */
template <typename T>
class TC_TryLockT : public TC_LockT<T>
{
public:

    TC_TryLockT(const T& mutex) : TC_LockT<T>(mutex, true)
    {
    }
};

/**
 * @brief  ����, �����κ�������
 */
class TC_EmptyMutex
{
public:
    /**
	* @brief  д��.
	*  
    * @return int, 0 ��ȷ
    */
    int lock()  const   {return 0;}

    /**
    * @brief  ��д��
    */
    int unlock() const  {return 0;}

    /**
	* @brief  ���Խ���. 
	*  
    * @return int, 0 ��ȷ
    */
    bool trylock() const {return true;}
};

/**
 * @brief  ��д������ģ����
 * ����ʱ�������������ʱ�����
 */

template <typename T>
class TC_RW_RLockT
{
public:
    /**
	 * @brief  ���캯��������ʱ����
	 *
     * @param lock ������
     */
	TC_RW_RLockT(T& lock)
		: _rwLock(lock),_acquired(false)
	{
		_rwLock.ReadLock();
		_acquired = true;
	}

    /**
	 * @brief ����ʱ����
     */
	~TC_RW_RLockT()
	{
		if (_acquired)
		{
			_rwLock.Unlock();
		}
	}
private:
	/**
	 *������
	 */
	const T& _rwLock;

    /**
     * �Ƿ��Ѿ�����
     */
    mutable bool _acquired;

	TC_RW_RLockT(const TC_RW_RLockT&);
	TC_RW_RLockT& operator=(const TC_RW_RLockT&);
};

template <typename T>
class TC_RW_WLockT
{
public:
    /**
	 * @brief  ���캯��������ʱ����
	 *
     * @param lock ������
     */
	TC_RW_WLockT(T& lock)
		: _rwLock(lock),_acquired(false)
	{
		_rwLock.WriteLock();
		_acquired = true;
	}
    /**
	 * @brief ����ʱ����
     */
	~TC_RW_WLockT()
	{
		if(_acquired)
		{
			_rwLock.Unlock();
		}
	}
private:
	/**
	 *������
	 */
	const T& _rwLock;
    /**
     * �Ƿ��Ѿ�����
     */
    mutable bool _acquired;

	TC_RW_WLockT(const TC_RW_WLockT&);
	TC_RW_WLockT& operator=(const TC_RW_WLockT&);
};

};
#endif

