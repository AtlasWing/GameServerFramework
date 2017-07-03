#ifndef __TC_FIFO_H
#define __TC_FIFO_H
#include <string>


namespace taf
{

/////////////////////////////////////////////////
/** 
 *@file   tc_fifo.h
 *@brief  FIFO��װ��. 
 * 
 *@author kevintian@tencent.com
 */
/////////////////////////////////////////////////

/** 
 *@brief  �ܵ�������. 
 */
class TC_Fifo
{
public:

   /**
	* @brief �ܵ�������ö������.  
    * �����˶Թܵ��Ĳ������ͣ�EM_WRITE��д�ܵ���EM_READ �����ܵ�
    */
	enum ENUM_RW_SET
	{
		EM_WRITE = 1,
		EM_READ  = 2
	};

public:
   /**
	* @brief ���캯��. 
	*  
    * @param bOwener : �Ƿ�ӵ�йܵ���Ĭ��Ϊture
    */
	TC_Fifo(bool bOwener = true);

   /**
    * @brief ��������. 
    */
	~TC_Fifo();

public:
	/**
	 * @brief ��FIFO. 
	 *  
	 * @param sPath Ҫ�򿪵�FIFO�ļ���·�� 
	 * @param enRW  �ܵ���������
	 * @param mode  ��FIFO�ļ���Ȩ�� ��Ĭ��Ϊ�ɶ���д
	 * @return      0-�ɹ�,-1-ʧ�� 
	 */
	int open(const std::string & sPath, ENUM_RW_SET enRW, mode_t mode = 0777);

	/**
	 * @brief �ر�fifo 
	 */
	void close();

	/**
	 * @brief ��ȡFIFO���ļ�������. 
	 * 
	 * @return  FIFO���ļ�������
	 */
	int fd() const { return _fd; }

	/**
	 * @brief ������, ����ȡ�ɹ�ʱ������ʵ�ʶ�ȡ���ֽ�����������ص�ֵ��0�������Ѿ������ļ��Ľ�����С��0��ʾ�����˴���
	 * @param buffer     ��ȡ������
	 * @param max_size   ��ȡ���ݵĴ�С 
	 * @return           ���������ݳ��� ,С��0���ʾʧ��
	 */
	int read(char * szBuff, const size_t sizeMax);

	/**
	 * @brief ��ܵ�д����. 
	 * 
	 * @param szBuff       Ҫд�������
	 * @param sizeBuffLen  ���ݵĴ�С
	 * @return             ����0����ʾд�˲��ֻ���ȫ������ 
	 * 				       С��0����ʾ���ִ���
	 */
	int write(const char * szBuff, const size_t sizeBuffLen);

private:
   /**
	* FIFO�ļ���·��
	*/
	std::string		_sPathName;

   /**
	* �Ƿ�ӵ��FIFO
	*/
	bool			_bOwner;

   /**
	* FIFO���ļ��Ĳ�������
	*/
	ENUM_RW_SET		_enRW;

   /**
	* FIFO���ļ�������
	*/
	int 			_fd;
};

}
#endif

