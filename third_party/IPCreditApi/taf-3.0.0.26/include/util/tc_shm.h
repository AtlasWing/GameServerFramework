#ifndef	__TC_SHM_H__
#define __TC_SHM_H__

#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include "util/tc_ex.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file  tc_shm.h 
 * @brief  �����ڴ��װ��. 
 *  
 * @author  jarodruan@tencent.com 
 */
/////////////////////////////////////////////////


/**
* @brief �����ڴ��쳣��.
*/
struct TC_Shm_Exception : public TC_Exception
{
    TC_Shm_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Shm_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_Shm_Exception() throw() {};
};

/** 
* @brief  �����ڴ������࣬˵��: 
* 1 �������ӹ����ڴ�, �����ڴ��Ȩ���� 0666 
* 2 _bOwner=false: ����ʱ��detach�����ڴ� 
* 3 _bOwner=true: ����ʱdetach�����ڴ�
*/
class TC_Shm
{
public:

    /**
	* @brief ���캯��.
	*  
	* @param bOwner  �Ƿ�ӵ�й����ڴ�,Ĭ��Ϊfalse 
    */
    TC_Shm(bool bOwner = false) : _bOwner(bOwner),_shmSize(0),_shmKey(0),_bCreate(true), _pshm(NULL),_shemID(-1) {}

    /**
	* @brief ���캯��. 
	*  
    * @param iShmSize �����ڴ��С
    * @param iKey     �����ڴ�Key
    * @throws         TC_Shm_Exception
    */
    TC_Shm(size_t iShmSize, key_t iKey, bool bOwner = false);

    /**
    * @brief ��������.
    */
    ~TC_Shm();

    /**
	* @brief ��ʼ��. 
	*  
    * @param iShmSize   �����ڴ��С
    * @param iKey       �����ڴ�Key
    * @param bOwner     �Ƿ�ӵ�й����ڴ�
    * @throws           TC_Shm_Exception
    * @return ��
    */
    void init(size_t iShmSize, key_t iKey, bool bOwner = false);

	/** 
	* @brief �жϹ����ڴ�����ͣ����ɵĹ����ڴ�,���������ϵĹ����ڴ�
	* ��������ɵĹ����ڴ�,��ʱ���Ը�����Ҫ����ʼ�� 
	*  
    * @return  true,���ɹ����ڴ�; false, �����ϵĹ����ڴ�
    */
    bool iscreate()     {return _bCreate;}

    /**
	* @brief  ��ȡ�����ڴ��ָ��.
	*  
    * @return   void* �����ڴ�ָ��
    */
    void *getPointer() {return _pshm;}

    /**
	* @brief  ��ȡ�����ڴ�Key.
	*  
    * @return key_t* ,�����ڴ�key
    */
    key_t getkey()  {return _shmKey;}

    /**
	* @brief  ��ȡ�����ڴ�ID.
	* 
    * @return int ,�����ڴ�Id
    */
    int getid()     {return _shemID;}

    /**
	*  @brief  ��ȡ�����ڴ��С.
	*  
    * return size_t,�����ڴ��С
    */
    size_t size()   {return _shmSize;}

	/** 
	*  @brief ��������ڴ棬�ڵ�ǰ�����н�������ڴ�
    * �����ڴ��ڵ�ǰ��������Ч
    * @return int
    */
    int detach();

	/** 
	 *  @brief ɾ�������ڴ�.
	 * 
     * ��ȫɾ�������ڴ�
     */
    int del();

protected:

    /**
     * �Ƿ�ӵ�й����ڴ�
     */
    bool            _bOwner;

    /**
    * �����ڴ��С
    */
    size_t          _shmSize;

    /**
    * �����ڴ�key
    */
    key_t           _shmKey;

    /**
    * �Ƿ������ɵĹ����ڴ�
    */
    bool            _bCreate;

    /**
    * �����ڴ�
    */
    void            *_pshm;

    /**
    * �����ڴ�id
    */
    int             _shemID;

};

}

#endif
