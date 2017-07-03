#ifndef	__IPCM_MEM_VECTOR_H__
#define __IPCM_MEM_VECTOR_H__

#include <sstream>
#include <string>
#include <assert.h>
#include "util/tc_ex.h"
using namespace std;


/**
* �쳣��
*/
struct memVectorException : public taf::TC_Exception
{
	memVectorException(const string &buffer) : taf::TC_Exception(buffer){};
	memVectorException(const string &buffer, int err) : taf::TC_Exception(buffer, err){};
	~memVectorException() throw(){};
};


/**
* �����ڴ�����������ݿ�
* ÿ�����ݿ��С���
* ģ�����Tֻ���Ǽ򵥵���������, ��Ҫ�߱�bit-copy���������
*/
template<typename T>
class MemVector
{
public:
	/**
	* ���캯��
	*/
	MemVector() : _pHead(NULL) ,_pData(NULL)
	{
	}

	/**
	 *
	 * @param mv
	 */
	MemVector(const MemVector &mv)
	: _pHead(mv._pHead) ,_pData(mv._pData)
	{

	}

	/**
	 *
	 * @param mv
	 *
	 * @return bool
	 */
	bool operator==(const MemVector &mv)
	{
	    return _pHead == mv._pHead && _pData == mv._pData;
	}

	/**
	 *
	 * @param mv
	 *
	 * @return bool
	 */
	bool operator!=(const MemVector &mv)
	{
	    return _pHead != mv._pHead || _pData != mv._pData;
	}

	/**
	 * ������Ҫ���ڴ�ռ�
	 * @param iCount: ���ݸ���
	 *
	 * @return size_t
	 */
	static size_t calcMemSize(size_t iCount)
	{
	    return sizeof(T) * iCount + sizeof(tagMemQueueHead);
	}

	/**
	* ��ʼ��
	* @param pAddr: ָ����пռ��ָ��
	* @param iSize: �ռ��ָ��
	* @param iBlockSize: ÿ��block�Ĵ�С
	* @return void
	*/
	void create(void *pAddr, size_t iSize);

	/**
	* �����϶���
	* @param pAddr: ָ����пռ��ָ��
	* @return bool
	*/
	bool connect(void *pAddr) { init(pAddr); return _pHead->_iBlockSize == sizeof(T); }


    void reset(void* pAddr, size_t iSize);
    void reset()
    {
        if (_pHead)
        {
            _pHead->_iMaxIndex = 0;
        }
    }
	/**
	* д����
	*/	
	void writeData(T& t);

	/**
	* ɾ������
	*/	
	void deleteData(T& t);

	/**
	* ���ֲ���
	*/	
	int binarySearchDelete(T& t);


	/**
	* ���ֲ���
	*/	
	int binarySearchAdd(T& t);


	/**
	* Ԫ�ظ���
	* @return size_t : ���г���
	*/
	size_t size() { return _pHead->_iBlockCount; }

	/**
	* �����ڴ泤��
	* @return size_t : �����ڴ泤��
	*/
	size_t getMemSize() { return _pHead->_iSize; }
		
	/**
	* ��ȡͷ��ַ	
	*/
	void *getAddr() { return (void*)_pHead; }

	/**
	* ��ȡ����ͷ��ַ	
	*/

	void *getDataAddr() {return (void *) _pData;}


	/**
	* ��ȡ��ǰԪ����� �±�
	*/
	size_t getMaxIndex() const {return _pHead->_iMaxIndex;}


	/**
	 * �ؽ�
	 */
	void clear();

	/**
	 *
	 * @param simple
	 *
	 * @return string
	 */
	string desc() const;

	/**
	 * ��ȡ����
	 * @param pData
	 * @param iDataLen
	 */
	const T& operator[](size_t iIndex)
	{
		if(iIndex >= _pHead->_iBlockCount)
		{
			ostringstream s;
			s << string("[MemVector::get] index beyond : index = ") << iIndex << " > " << _pHead->_iBlockCount;
			throw memVectorException(s.str());

		}

		return *(T*)((char*)_pData + iIndex * _pHead->_iBlockSize);
	}


	struct tagMemQueueHead
	{
		size_t _iSize;          //�ڴ��С
		size_t _iBlockCount;    //Ԫ�ظ���
		size_t _iBlockSize;     //�����С
		size_t _iMaxIndex;  //��ǰ����±�
	}__attribute__((packed));

protected:

	/**
	 *
	 * @param pAddr
	 */
	void init(void *pAddr)
	{
	    _pHead = static_cast<tagMemQueueHead*>(pAddr);
	    _pData = (char*)_pHead + sizeof(tagMemQueueHead);
	}


	/**
	* ���п��ƿ�
	*/
	tagMemQueueHead *_pHead;

	/**
	* �����ڴ��ַ
	*/
	void            *_pData;
};

template<typename T>
void MemVector<T>::create(void *pAddr, size_t iSize)
{
	size_t iBlockSize = sizeof(T);

	if(iSize <= sizeof(tagMemQueueHead) || ((iSize - sizeof(tagMemQueueHead)) / iBlockSize == 0))
	{
	}
	init(pAddr);

	memset(pAddr, 0x00, iSize);

	_pHead->_iSize         = iSize;
	_pHead->_iBlockCount   = (iSize - sizeof(tagMemQueueHead)) / iBlockSize;
	_pHead->_iBlockSize     = iBlockSize;
	_pHead->_iMaxIndex	 = 0;
}

template<typename T>
void MemVector<T>::reset(void* pAddr, size_t iSize)
{
    size_t iBlockSize = sizeof(T);
    init(pAddr);
    _pHead->_iSize = iSize;
    _pHead->_iBlockCount = (iSize - sizeof(tagMemQueueHead)) / iBlockSize;
    _pHead->_iBlockSize = iBlockSize;
}

template<typename T>
void MemVector<T>::clear()
{
	assert(_pHead);

	memset(_pData, 0x00, _pHead->_iBlockSize * _pHead->_iBlockCount);

	_pHead->_iMaxIndex = 0;
}

template<typename T>
string MemVector<T>::desc() const
{
	ostringstream s;
	s << "[MemVector] [_iSize=" << _pHead->_iSize << "] "
	  << "[_iBlockCount="  << _pHead->_iBlockCount << "] "
	  << "[_iBlockSize="  << _pHead->_iBlockSize << "] "
	  << endl;
	s << "[~MemVector]";

	return s.str();
}

template<typename T>
void MemVector<T>::writeData(T& t)
{
	if(_pHead->_iMaxIndex >= _pHead->_iBlockCount)
	{
		ostringstream s;
		s << string("[MemVector::get] index beyond : index = ") << _pHead->_iMaxIndex << " > " << _pHead->_iBlockCount;
		throw memVectorException(s.str());
	}

	if(_pHead->_iMaxIndex == 0)
	{
		*(T*)((char*)_pData) = t;
		_pHead->_iMaxIndex++;
		return;
	}
	

	//int position = binarySearchAdd(t);
	int position = _pHead->_iMaxIndex;
	
	if (position != -1)
	{
		T* begin = (T*)((char*)_pData + position  * _pHead->_iBlockSize);
		T* end = (T*)((char*)_pData + (_pHead->_iMaxIndex - 1) * _pHead->_iBlockSize);

		//����
		while(end >= begin)
		{
			T* next =  end + 1;
			*next = *end;
			--end;
		}

		*begin = t;
		_pHead->_iMaxIndex++;
	}
}

template<typename T>
void MemVector<T>::deleteData(T& t)
{
	int position = binarySearchDelete(t);
	if(position < 0)
	{
		ostringstream s;
		s <<"data is not existed:"<<t<<endl;
		throw memVectorException(s.str());
	}

	T* begin = (T*)((char*)_pData + position  * _pHead->_iBlockSize);
	T* end = (T*)((char*)_pData + (_pHead->_iMaxIndex - 1) * _pHead->_iBlockSize);

	//����
	while(begin <= end)
	{
		T* next = begin + 1; 
		*begin = * next;
		begin++;
	}

	_pHead->_iMaxIndex--;
}

template<typename T>
int  MemVector<T>::binarySearchDelete(T& t)
{
	int low = 0;
	int high = _pHead->_iMaxIndex - 1;
	//int high = size();

	while(low <= high)
	{
		int mid = (low + high) / 2;
		T* p= (T*)((char*)_pData + mid * _pHead->_iBlockSize);
		
		if(*p == t)
		{
			return mid;
		}
		
		if(t < *p )
		{
			high = mid - 1;
		}
		
		if(*p < t)
		{
			low = mid + 1;
		}
	}
	return -1;
}

template<typename T>
int  MemVector<T>::binarySearchAdd(T& value)
{
	int low = 0;
	int high = _pHead->_iMaxIndex - 1;
	int mid;

	while(low <= high )
	{
		mid = (low + high) / 2;
				
		T midValue = *(T*)((char*)_pData + mid * _pHead->_iBlockSize);

		if(value < midValue)
		{
			high = mid - 1;
		}
		else if(midValue < value)
		{
			low = mid + 1;
		}
		else //�����ظ����룬ֱ�ӷ���
		{
			return -1;
		}
	}

//	T lowValue = *(T*)((char*)_pData + low * _pHead->_iBlockSize);

	return low;
}

#endif
