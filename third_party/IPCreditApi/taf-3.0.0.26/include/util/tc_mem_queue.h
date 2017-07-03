#ifndef	__TC_MEM_QUEUE_H__
#define __TC_MEM_QUEUE_H__

#include <string>

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file   tc_mem_queue.h 
 * @brief  �����ڴ�ѭ��������. 
 *  
 * @author  jarodruan@tencent.com 
 */
	         
/////////////////////////////////////////////////
/**
* @brief �ڴ�ѭ������, ͬʱ��д������� 
*  
*  �ڴ�ѭ�����У����鲻Ҫֱ��ʹ�ø��࣬ͨ��jmem�����ʹ��
*  
* ���˱�������ʹ��kill�������в��ỵ����������һ������
*/
class TC_MemQueue
{
public:

    /**
    * @brief ���캯��
    */
    TC_MemQueue();

    /**
    * @brief ����
    */
    ~TC_MemQueue(){}

    /**
    * @brief ��ʼ��
    * @param pAddr ָ����пռ��ָ��
    * @param iSize �ռ��ָ��
    */
    void create(void *pAddr, size_t iSize);

    /**
    * @brief �����϶���
    * @param pAddr ָ����пռ��ָ��
    */
    void connect(void *pAddr, size_t iSize);

    /**
    * @brief ��������
    * @param sOut   ��������ݿ�
	* @return       bool,true:��ȷ, false: ����,���������,���п� 
    */
    bool pop_front(string &sOut);

    /**
    * @brief ��������
    * @param  sIn ��������ݿ�
    * @return     bool,true:��ȷ, false: ����, ������
    */
    bool push_back(const string &sIn);

    /**
    * @brief ��������
    * @param pvIn   ��������ݿ�
    * @param iSize  �������ݿ鳤��
    * @return       bool,true:��ȷ, false: ����, ������
    */
    bool push_back(const char *pvIn, size_t iSize);

    /**
    * @brief �����Ƿ���
    * @param iSize �������ݿ鳤��
    * @return      bool,true:��, false: ����
    */
    bool isFull(size_t iSize);

    /**
    * @brief �����Ƿ��
    * @return  bool,true: ��, false: ����
    */
    bool isEmpty();

    /**
    * @brief ������Ԫ�ظ���, ������������²���֤һ����ȷ
    * @return size_t, Ԫ�ظ���
    */
    size_t elementCount();

    /**
	* @brief ���г���(�ֽ�), 
	*   	 С���ܴ洢������(�ܴ洢�����Ȱ����˿��ƿ�)
    * @return size_t�����г���
    */
    size_t queueSize();

    /**
    * @brief �����ڴ泤��
    * @return size_t �����ڴ泤��
    */
    size_t memSize() const {return _size;};

protected:
    /**
     * @brief �޸ľ����ֵ
     * @param iModifyAddr   ��Ҫ���޸ĵ�ֵ
     * @param iModifyValue  �滻��ֵ
     */
    void update(void* iModifyAddr, size_t iModifyValue);

	/**
     * @brief �޸ľ����ֵ
     * @param iModifyAddr   ��Ҫ���޸ĵ�ֵ
     * @param iModifyValue  �滻��ֵ
     */
    void update(void* iModifyAddr, bool bModifyValue);

    /**
     * @brief �޸ĸ��µ��ڴ���
     */
    void doUpdate(bool bUpdate = false);

protected:

    /**
    *  @brief ���п��ƽṹ
    */
    struct CONTROL_BLOCK
    {
        size_t iMemSize;            /**�ڴ��С*/
        size_t iTopIndex;           /**����Ԫ������*/
        size_t iBotIndex;           /**�ײ�Ԫ������*/
        size_t iPushCount;          /**�����н���Ԫ�صĸ���*/
        size_t iPopCount;           /**�����е���Ԫ�صĸ���*/
    }__attribute__((packed));

    /**
     * @brief ��Ҫ�޸ĵĵ�ַ
     */
    struct tagModifyData
    {
        size_t  _iModifyAddr;       /**�޸ĵĵ�ַ*/
        char    _cBytes;            /**�ֽ���*/
        size_t  _iModifyValue;      /**ֵ*/
    }__attribute__((packed));

    /**
     * @brief �޸����ݿ�ͷ��
     */
    struct tagModifyHead
    {
        char            _cModifyStatus;         /**�޸�״̬: 0:Ŀǰû�����޸�, 1: ��ʼ׼���޸�, 2:�޸����, û��copy���ڴ���*/
        size_t          _iNowIndex;             /**���µ�Ŀǰ������, ���ܲ���10��*/
        tagModifyData   _stModifyData[5];       /**һ�����5���޸�*/
    }__attribute__((packed));

    /**
    * ���п��ƿ�(�ڴ�����)
    */
    CONTROL_BLOCK   *_pctrlBlock;

    /**
    * �ڴ����ݵ�ַ
    */
    void            *_paddr;

    /**
    * �����ڴ�
    */
    size_t		    _size;

    /**
     * �޸����ݿ�
     */
    tagModifyHead   *_pstModifyHead;


};

}

#endif
