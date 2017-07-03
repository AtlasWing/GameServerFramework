#ifndef __TC_THREAD_RWLOCK_H
#define __TC_THREAD_RWLOCK_H

#include <pthread.h>
#include "util/tc_lock.h"

#if !defined(linux) && (defined(__linux) || defined(__linux__))
#define linux
#endif

using namespace std;

namespace taf
{

/////////////////////////////////////////////////
/**
 * @file tc_thread_rwlock.h
 * @brief ��д��
 *
 */

/////////////////////////////////////////////////

/**
 * @brief��д���쳣��
 */
struct TC_ThreadRW_Exception : public TC_Exception
{
    TC_ThreadRW_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ThreadRW_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_ThreadRW_Exception() throw() {};
};


#if !defined(linux) || (defined __USE_UNIX98 || defined __USE_XOPEN2K)
class TC_ThreadRWLocker
{
public:
	/**
     * @brief ���캯��
     */
	TC_ThreadRWLocker();

    /**
     * @brief ��������
     */
	~TC_ThreadRWLocker();

	/**
	 *@brief ������,����pthread_rwlock_rdlock
	 *return : ʧ�������쳣TC_ThreadRW_Exception
	 */
	void ReadLock() const;

	/**
	 *@brief д����,����pthread_rwlock_wrlock
	 *return : ʧ�������쳣TC_ThreadRW_Exception
	 */
	void WriteLock() const;

	/**
	 *@brief ���Զ�����,����pthread_rwlock_tryrdlock
	 *return : ʧ�������쳣TC_ThreadRW_Exception
	 */
	void TryReadLock() const;

	/**
	 *@brief ����д����,����pthread_rwlock_trywrlock
	 *return : ʧ�������쳣TC_ThreadRW_Exception
	 */
	void TryWriteLock() const ;

	/**
	 *@brief ����,����pthread_rwlock_unlock
	 *return : ʧ�������쳣TC_ThreadRW_Exception
	 */
	void Unlock() const;

private:

	mutable pthread_rwlock_t m_sect;

	// noncopyable
	TC_ThreadRWLocker(const TC_ThreadRWLocker&);
	TC_ThreadRWLocker& operator=(const TC_ThreadRWLocker&);

};

typedef TC_RW_RLockT<TC_ThreadRWLocker> TC_ThreadRLock;
typedef TC_RW_WLockT<TC_ThreadRWLocker> TC_ThreadWLock;

#endif

}
#endif


