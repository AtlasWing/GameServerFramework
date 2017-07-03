#ifndef	__IPCM_MEM_VECTOR_H__
#define __IPCM_MEM_VECTOR_H__

#include <sstream>
#include <string>
#include <assert.h>
#include "util/tc_ex.h"
using namespace std;


/**
* 异常类
*/
struct memVectorException : public taf::TC_Exception
{
	memVectorException(const string &buffer) : taf::TC_Exception(buffer){};
	memVectorException(const string &buffer, int err) : taf::TC_Exception(buffer, err){};
	~memVectorException() throw(){};
};


/**
* 共享内存随机访问数据块
* 每快数据快大小相等
* 模板对象T只能是简单的数据类型, 需要具备bit-copy的语义才行
*/
template<typename T>
class MemVector
{
public:
	/**
	* 构造函数
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
	 * 计算需要的内存空间
	 * @param iCount: 数据个数
	 *
	 * @return size_t
	 */
	static size_t calcMemSize(size_t iCount)
	{
	    return sizeof(T) * iCount + sizeof(tagMemQueueHead);
	}

	/**
	* 初始化
	* @param pAddr: 指令队列空间的指针
	* @param iSize: 空间的指针
	* @param iBlockSize: 每个block的大小
	* @return void
	*/
	void create(void *pAddr, size_t iSize);

	/**
	* 连接上队列
	* @param pAddr: 指令队列空间的指针
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
	* 写数据
	*/	
	void writeData(T& t);

	/**
	* 删除数据
	*/	
	void deleteData(T& t);

	/**
	* 二分查找
	*/	
	int binarySearchDelete(T& t);


	/**
	* 二分查找
	*/	
	int binarySearchAdd(T& t);


	/**
	* 元素个数
	* @return size_t : 队列长度
	*/
	size_t size() { return _pHead->_iBlockCount; }

	/**
	* 共享内存长度
	* @return size_t : 共享内存长度
	*/
	size_t getMemSize() { return _pHead->_iSize; }
		
	/**
	* 获取头地址	
	*/
	void *getAddr() { return (void*)_pHead; }

	/**
	* 获取数据头地址	
	*/

	void *getDataAddr() {return (void *) _pData;}


	/**
	* 获取当前元素最大 下标
	*/
	size_t getMaxIndex() const {return _pHead->_iMaxIndex;}


	/**
	 * 重建
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
	 * 获取数据
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
		size_t _iSize;          //内存大小
		size_t _iBlockCount;    //元素个数
		size_t _iBlockSize;     //区块大小
		size_t _iMaxIndex;  //当前最大下标
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
	* 队列控制快
	*/
	tagMemQueueHead *_pHead;

	/**
	* 共享内存地址
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

		//右移
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

	//左移
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
		else //遇到重复插入，直接返回
		{
			return -1;
		}
	}

//	T lowValue = *(T*)((char*)_pData + low * _pHead->_iBlockSize);

	return low;
}

#endif
