#ifndef __TC_FILE_MUTEX_H
#define __TC_FILE_MUTEX_H

#include "util/tc_lock.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
* @file tc_file_mutex.h 
* @brief  �ļ�����. 
*  
* @author  jarodruan@tencent.com      
* 
*/
/////////////////////////////////////////////////

/**
* @brief  �쳣��
*/
struct TC_FileMutex_Exception : public TC_Lock_Exception
{
    TC_FileMutex_Exception(const string &buffer) : TC_Lock_Exception(buffer){};
    TC_FileMutex_Exception(const string &buffer, int err) : TC_Lock_Exception(buffer, err){};
    ~TC_FileMutex_Exception() throw() {};
};

/**
 * @brief  �ļ���, ע��:ֻ���ڽ��̼����.
 */
class TC_FileMutex
{
public:

	/**
     * @brief  ���캯��.
	 */
	TC_FileMutex();

	/**
     * @brief  ��������.
	 */
	virtual ~TC_FileMutex();

	/**
	 * @brief  ��ʼ���ļ���. 
	 *  
	 * @param filename ���������ļ�������
	 */
	void init(const std::string& filename);

    /**
	* @brief  �Ӷ���.
	* 
    *@return 0-�ɹ�������-1-����ʧ��
    */
    int rlock();

    /**
	* @brief  �����. 
	*  
    * @return 0-�ɹ�������-1-����ʧ��
    */
    int unrlock();

    /**
	* @brief  ���Զ���. 
	*  
    * @throws TC_FileMutex_Exception
    * @return  �����ɹ��򷵻�false, ���򷵻�false
    */
    bool tryrlock();

    /**
	* @brief  ��д��. 
	*  
    * @return int
    */
    int wlock();

    /**
    * @brief  ��д��.
    */
    int unwlock();

    /**
	* @brief  ����д��. 
	*  
	* @return bool�������ɹ��򷵻�false, ���򷵻�false 
	* @throws TC_FileMutex_Exception 
    */
    bool trywlock();

    /**
	* @brief  д��. 
	*  
    * @return int, 0 ��ȷ
    */
    int lock(){return wlock();};

    /**
    * @brief  ��д��.
    */
    int unlock();

    /**
	* @brief  ���Խ���. 
	*  
    * @throws TC_FileMutex_Exception
    * @return int, 0 ��ȷ
    */
    bool trylock() {return trywlock();};

protected:
	/**
	 * @brief  ������. 
	 *  
	 * @param fd       �����õ��ļ�������
	 * @param cmd      ��������ָ��
	 * @param type     ����״̬���ֱ�ΪF_RDLCK ,F_WRLCK ,F_UNLCK 
	 * @param offset   ƫ����
	 * @param whence   ��������ʼλ�ã����ַ�ʽ
	 * @param len      ��������Ĵ�С
	 * @return         int���ɹ��򷵻�0�����д����򷵻�-1. 
	 */
	int lock(int fd, int cmd, int type, off_t offset, int whence, off_t len);

	/**
	 * @brief  �Ƿ�������������. 
	 *  
	 * @param fd      �����õ��ļ�������
	 * @param type    ����״̬���ֱ�ΪF_RDLCK ,F_WRLCK ,F_UNLCK 
	 * @param offset  ƫ����
	 * @param whence  ��������ʼλ�ã����ַ�ʽ
	 * @param len     ��������Ĵ�С
	 * @return bool   ��������true����������false. 
	 */
	bool hasLock(int fd, int type, off_t offset, int whence, off_t len);

private:
	int _fd;
};

}

#endif

