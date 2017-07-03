#ifndef __TC_MALLOC_CHUNK_H
#define __TC_MALLOC_CHUNK_H

#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

namespace taf
{
	static const size_t kPageShift  = 15;
	static const size_t kNumClasses = 78;
	static const size_t kPageSize   = 1 << kPageShift;
	static const size_t kMaxSize    = 256 * 1024;
	static const size_t kAlignment  = 8;
	static const size_t kMaxPages	= 1 << (20 - kPageShift);
	static const size_t kPageAlignment = 64;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	* ������ڴ��С������ʵ�ʵõ����ڴ��С��ӳ��
	* ʵ�ʵõ����ڴ��С���ڴ��С�����Ĵ�С����ӳ��
	* �ڴ��С�����Ĵ�С�������ڴ��С�����Ҫ���ڴ�ҳ��ӳ��
	*/
	class SizeMap
	{
	public:
		SizeMap() { Init(); }	

		/*
		*size�����ĳߴ����
		*/
		inline int SizeClass(int size)
		{
			return class_array_[ClassIndex(size)];
		}

		/*
		*�ߴ����cl��Ӧ�ĳߴ��С
		*/
		inline size_t ByteSizeForClass(size_t cl)
		{
			return class_to_size_[cl];
		}

		/*
		*�ߴ����cl��Ӧ��ҳ��
		*/
		inline size_t class_to_pages(size_t cl)
		{
			return class_to_pages_[cl];
		}

	private:
		static inline size_t ClassIndex(int s)
		{
			const bool big = (s > kMaxSmallSize);
			const size_t  add_amount= big ? (127 + (120<<7)) : 7;
			const size_t  shift_amount = big ? 7 : 3;
			return (s + add_amount) >> shift_amount;
		}

		static inline int LgFloor(size_t n)
		{
			int log = 0;
			for (int i = 4; i >= 0; --i)
			{
				int shift = (1 << i);
				size_t x = n >> shift;
				if (x != 0)
				{
					n = x;
					log += shift;
				}
			}

			return log;
		}

		/*
		*��ʼ��
		*/
		void Init();

		size_t AlignmentForSize(size_t size);

		int NumMoveSize(size_t size);
	private:
		static const int kMaxSmallSize		= 1024;
		static const size_t kClassArraySize = ((kMaxSize + 127 + (120 << 7)) >> 7) + 1;
		size_t			class_to_size_[kNumClasses];
		size_t			class_to_pages_[kNumClasses];
		unsigned char	class_array_[kClassArraySize];

	};
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class Static
	{
	public:
		static SizeMap* sizemap()
		{
			return &_sizemap;
		}
	private:
		static SizeMap _sizemap;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	* TC_Span�ڴ������
	*
	* ���ڷ���sizeof(TC_Span)��С���ڴ��
	*/
	class TC_SpanAllocator
	{
	public:
		TC_SpanAllocator() : _pHead(NULL), _pData(NULL) {}

		void* getHead() const { return _pHead; }

		/**
		 * ��ʼ��
		 * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
		 * @param mem_size, �ڴ��С
		 */
		void create(void* pAddr, size_t iSpanSize, size_t iSpanCount);

		/**
		 * ������
		 * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
		 */
		void connect(void* pAddr);

		/**
		 * �ؽ�
		 */
		void rebuild();

		/**
		 * �Ƿ��п���block
		 *
		 * @return bool
		 */
		bool isSpanAvailable() const { return _pHead->_spanAvailable > 0; }

		/**
		 * ����һ������
		 *
		 * @return void*
		 */
		void* allocate();

		/**
		 * �ͷ�����
		 * @param pAddr
		 */
		void deallocate(void *pAddr);

		struct tagSpanAlloc
		{
			size_t  _iSpanSize;            /**span�����С*/
			size_t  _iSpanCount;           /**span����*/
			size_t  _firstAvailableSpan;   /**��һ�����õ�span����*/
			size_t  _spanAvailable;        /**����span����*/
		}__attribute__((packed));

		static size_t getHeadSize() { return sizeof(tagSpanAlloc); }

	protected:
		/**
		 * ��ʼ��
		 */
		void init(void *pAddr);

		//��ֹcopy����
		TC_SpanAllocator(const TC_SpanAllocator &mcm);

		//��ֹ����
		TC_SpanAllocator &operator=(const TC_SpanAllocator &mcm);

	private:
		/**
		 * ����ͷָ��
		 */
		tagSpanAlloc    *_pHead;

		/**
		 * ������ָ��
		 */
		unsigned char   *_pData;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	* �ڴ������
	*
	* ���������ڴ水32K��С��ҳ
	* ��ͨ��TC_Span��PageMap���й���
	*/
	class TC_Page
	{
	public:
		struct tagShmFlag
		{
			bool         _bShmProtectedArea;	/*�Ƿ����ñ�����*/
			int          _iShmFlag;				/*�ڴ�ռ��ʶ���ڴ�δʹ��ǰΪ0��ʹ�ú�Ϊ1*/
			size_t       _iShmMemSize;			/*�ڴ�ռ��С*/
			size_t       _iShmSpanAddr;			/*TC_Span�ڴ�������ʼ��ַ����Ե�ַ*/
			size_t       _iShmPageMapAddr;		/*PageMap�ڴ�������ʼ��ַ����Ե�ַ*/
			size_t       _iShmPageAddr;			/*Data�����ڴ�������ʵ��ַ����Ե�ַ*/
			size_t       _iShmSpanNum;			/*TC_Span�ڴ�����span�ĸ���*/
			size_t       _iShmPageMapNum;		/*PageMap�ڴ�����map�ĸ���*/
			size_t       _iShmPageNum;			/*Data�ڴ�����ҳ�ĸ���*/
		}__attribute__((packed));

		struct tagModifyData
		{
			size_t       _iModifyAddr;			/*�޸ĵĵ�ַ*/
			char         _cBytes;				/*�ֽ���*/
			size_t       _iModifyValue;			/*ֵ*/
		}__attribute__((packed));

		struct tagModifyHead
		{
			char           _cModifyStatus;		/*�޸�״̬: 0:Ŀǰû�����޸�, 1: ��ʼ׼���޸�, 2:�޸����, û��copy���ڴ���*/
			size_t         _iNowIndex;			/*���µ�Ŀǰ������*/
			tagModifyData  _stModifyData[32];	/*һ�����32���޸�*/
		}__attribute__((packed));

		struct TC_Span
		{
			size_t       start;					/*span�������ҳ�ڴ����ʼҳ��*/
			size_t       length;				/*ҳ�ĸ���*/
			size_t       next;
			size_t       prev;
			size_t       objects;				/*���ڷ����ڴ�*/
			unsigned int refcount;				/*����sizeclass���С����spanӵ�е��ڴ��ĸ���*/
			unsigned int sizeclass;				/*�ڴ��С�����*/
			unsigned int location;				/*����ʶ���span�ǿ��У����Ǵ���ʹ����*/

			enum { IN_USE, ON_FREELIST };
		}__attribute__((packed));	

		struct TC_CenterList
		{
			size_t       size_class;			/*�ڴ��С�����*/
			TC_Span      empty;					/*��������*/
			TC_Span      nonempty;				/*�ǿ�������*/
		}__attribute__((packed));

	public:
		TC_Page() : _pShmFlagHead(NULL),_pModifyHead(NULL),_pCenterCache(NULL),_pLarge(NULL),_pFree(NULL),_pSpanMemHead(NULL),_pPageMap(NULL),_pData(NULL) {}

		/**
		 * ��ʼ��
		 * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
		 * @param iShmMemSize, �ڴ��С
		 * @param bProtectedArea, �Ƿ�ʹ�ñ�����
		 */
		void create(void *pAddr, size_t iShmMemSize, bool bProtectedArea);

		/**
		 * ������
		 * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
		 */
		void connect(void *pAddr);

		/**
		 * �ؽ�
		 */
		void rebuild();

		/**
		 * ����һ������
		 * @param iClassSize,��Ҫ������ڴ��С���
		 * @param iAllocSize, ��������ݿ��С
		 * @param iPageId, ���ڴ�������TC_Span����ʼҳ��
		 * @param iIndex, ���ڴ�������TC_Span�İ�iAllocSize��С���ֺ�ĵ�iIndex��
		 * @return void *
		 */
		void* fetchFromSpansSafe(size_t iClassSize, size_t &iAllocSize, size_t &iPageId, size_t &iIndex);

		/**
		 * �ͷ��ڴ�, ���ݸ��ڴ�����TC_Span����ʼҳ�͸�TC_Span����С��𻮷ֺ�ĵ�iIndex��
		 * @param iPageId, ���ڴ�������TC_Span����ʼҳ��
		 * @param iIndex, ���ڴ�������TC_Span�İ�iAllocSize��С���ֺ�ĵ�iIndex��
		 */
		void  releaseToSpans(size_t iPageId, size_t iIndex);

		/**
		 * �ͷ��ڴ�, ���Ե�ַ
		 * @param pAddr
		 */
		void  releaseToSpans(void* pObject);

		/**
		 * ���ݸ��ڴ�����TC_Span����ʼҳ�͸�TC_Span����С��𻮷ֺ�ĵ�iIndex���õ����ڴ�����ʼ���Ե�ַ
		 * @param iPageId, ���ڴ�������TC_Span����ʼҳ��
		 * @param iIndex, ���ڴ�������TC_Span�İ�iAllocSize��С���ֺ�ĵ�iIndex��
		 */
		void* getAbsolute(size_t iPageId, size_t iIndex);

		/**
		 * �޸ĸ��µ��ڴ���
		 */
		void doUpdate(bool bUpdate = false);

		/**
		 * ������ڴ�����ݵ�ҳ�ڴ������
		 */
		inline size_t getPageNumber();

		/**
		 * ������ڴ�����ݵ�ҳ�ڴ�Ĵ�С
		 */
		inline size_t getPageMemSize();

		/**
		 * ���TC_Page�������ڴ�Ľ���λ��
		 */
		inline size_t getPageMemEnd();

		/**
		 * ����TC_Page���ڴ����С��С
		 */
		static size_t getMinMemSize() 
		{ 
			return sizeof(tagShmFlag) + sizeof(tagModifyHead) + sizeof(TC_CenterList) * kNumClasses + sizeof(TC_Span) + sizeof(TC_Span) * kMaxPages + TC_SpanAllocator::getHeadSize() + sizeof(TC_Span) + sizeof(size_t) + kPageSize; 
		}

	protected:

		//��ֹcopy����
		TC_Page(const TC_Page &mcm);

		//��ֹ����
		TC_Page &operator=(const TC_Page &mcm);

		/**
		 * ��ʼ��
		 */
		void init(void *pAddr);

		/**
		 * ��ҳ��ʼ�����ڴ�����ݵ��ڴ�
		 */
		void initPage(void *pAddr);


		/////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 * ��ʼ��list˫������
		 */
		inline void DLL_Init(TC_Span* list, size_t iIndex);

		/**
		 * ��˫��������ɾ��spanָ��Ľڵ�
		 */
		inline void DLL_Remove(TC_Span* span);

		/**
		 * ��spanָ��Ľڵ���뵽˫������list��
		 */
		inline void DLL_Prepend(TC_Span* list, TC_Span* span);

		/**
		 * ˫������list�Ƿ�Ϊ��
		 */
		inline bool DLL_IsEmpty(TC_Span* list, size_t iIndex) 
		{
		  return list->next == iIndex;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 * �õ�PageMap�е�k��Ԫ�ش�ŵ�ָ��ֵ����ʵ��������ǿ�Data�ڴ����е�k���ڴ�ҳ�����ĸ�TC_Span����
		 */
		size_t Get(size_t k) const
		{
			return _pPageMap[k];
		}

		/**
		 * ����PageMap�е�k��Ԫ�ش�ŵ�ָ��ֵ����ʵ��������ǿ�Data�ڴ����е�k���ڴ�ҳ�����ĸ�TC_Span����
		 */
		void Set(size_t k, size_t v)
		{
			_pPageMap[k] = v;
		}

		/**
		 * ȷ��nδ����ҳ�ڴ����Ŀ��k֮��
		 */
		bool Ensure(size_t k, size_t n)
		{
			return n <= _pShmFlagHead->_iShmPageMapNum -k;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 * create��rebuild��ʱ�����ã����ڳ�ʼ��ʱ�����е��ڴ�ҳӳ�䵽_pLarge��
		 */
		bool UseShmMem();

		/**
		 * �õ��ڴ�ҳpPageId�����ĸ�TC_Span����
		 */
		TC_Span* GetDescriptor(size_t pPageId);

		/**
		 * ��_pFree��_pLarge�����в���n���������ڴ�ҳ��ͨ��TC_Span����
		 */
		TC_Span* SearchFreeAndLargeLists(size_t n);

		/**
		 * ����n���������ڴ�ҳ��ͨ��TC_Span���أ���������SearchFreeAndLargeLists����
		 */
		TC_Span* New(size_t n);

		/**
		 * ��_pLarge�����з���n���������ڴ�ҳ��ͨ��TC_Span����
		 */
		TC_Span* AllocLarge(size_t n);

		/**
		 * ��span��������ڴ�ҳ�У������n�������ڴ�
		 */
		TC_Span* Carve(TC_Span* span, size_t n);

		/**
		 * ����span��������ڴ�ҳ����Ŀ������span���뵽_pLarge��_pFree������
		 */
		void PrependToFreeList(TC_Span* span);

		/**
		 * ��span����������������ɾ�����������MergeIntoFreeList
		 */
		void Delete(TC_Span* span);

		/**
		 * ��span����������������ɾ�������Ҳ鿴��span��������ڴ�ҳǰ���������ڴ�ҳ�Ƿ���У����ǣ�����кϲ�����
		 */
		void MergeIntoFreeList(TC_Span* span);

		/**
		 * ����span��������ڴ�ҳ��Ҫ���зָ���ڴ��С���
		 */
		void RegisterSizeClass(TC_Span* span, size_t sc);

		/**
		 * ��span�������ڴ�ҳ��ӳ�䵽_pPageMap��
		 */
		void RecordSpan(TC_Span* span);

		//////////////////////////////////////////////////////////////////////////////////////////////////

		/**
		 * ����һ������
		 * @param iClassSize,��Ҫ������ڴ��С���
		 * @param iAllocSize, ��������ݿ��С
		 * @param iPageId, ���ڴ�������TC_Span����ʼҳ��
		 * @param iIndex, ���ڴ�������TC_Span�İ�iAllocSize��С���ֺ�ĵ�iIndex��
		 * @return void *
		 */
		void* FetchFromSpans(size_t iClassSize, size_t &iAllocSize, size_t &iPageId, size_t &iIndex);

		/**
		 * ��iClassSize�����ڴ��С�ָ��ڴ�ҳ
		 */
		int Populate(size_t iClassSize);

		/**
		 * �޸ľ����ֵ
		 * @param iModifyAddr
		 * @param iModifyValue
		 */
		//inline void update(void* iModifyAddr, size_t iModifyValue);
		template<typename T>
		void update(void* iModifyAddr, T iModifyValue)
		{
			_pModifyHead->_cModifyStatus = 1;
			_pModifyHead->_stModifyData[_pModifyHead->_iNowIndex]._iModifyAddr  = reinterpret_cast<size_t>(iModifyAddr) - reinterpret_cast<size_t>(_pShmFlagHead);
			_pModifyHead->_stModifyData[_pModifyHead->_iNowIndex]._iModifyValue = iModifyValue;
			_pModifyHead->_stModifyData[_pModifyHead->_iNowIndex]._cBytes       = sizeof(iModifyValue);
			_pModifyHead->_iNowIndex++;

			assert(_pModifyHead->_iNowIndex < sizeof(_pModifyHead->_stModifyData) / sizeof(tagModifyData));
		}
	
	private:
		/**
		 * ͷ���ڴ��ͷָ��
		 */
		tagShmFlag                *_pShmFlagHead;
		/**
		 * �������ڴ��ͷָ��
		 */
		tagModifyHead             *_pModifyHead;
		/**
		 * ������������ͷָ��
		 */
		TC_CenterList             *_pCenterCache;
		/**
		 * ����ڴ�ҳ������ָ�루���ڵ���1MB�ڴ�������
		 */
		TC_Span                   *_pLarge;
		/**
		 * С���ڴ�ҳ������ͷָ�루С��1MB���ڴ�飩
		 */
		TC_Span                   *_pFree;
		/**
		 * ���ڷ���TC_Span���ڴ������ͷָ��
		 */
		TC_SpanAllocator		  *_pSpanMemHead;
		/**
		 * ���������ڴ���ӳ����ڴ���ͷָ��
		 */
		//void                      **_pPageMap;
		size_t					  *_pPageMap;
		/**
		 * ��������ͷָ��
		 */
		void                      *_pData;

		/**
		 * ���ڷ���TC_Span���ڴ������
		 */
		TC_SpanAllocator		  _spanAlloc;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	class TC_MallocChunkAllocator
	{
	public:
		TC_MallocChunkAllocator():_pHead(NULL),_pChunk(NULL),_nallocator(NULL) {}

		~TC_MallocChunkAllocator()
		{
			clear();
		}

		void clear()
		{
			if(_nallocator)
			{
				delete _nallocator;
				_nallocator = NULL;
			}

			_pHead = NULL;
			_pChunk = NULL;
		}

		/**
		 * ��ʼ��
		 * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
		 * @param iSize, �ڴ��С
		 * @param bProtectedArea, �Ƿ�ʹ�ñ�����,Ĭ��ʹ��
		 */
		void create(void *pAddr, size_t iSize, bool bProtectedArea = true);

		/**
		 * ������
		 * @param pAddr, ��ַ, ����Ӧ�ó���ľ��Ե�ַ
		 */
		void connect(void *pAddr);

		/**
		 * ��չ�ռ�
		 * 
		 * @param pAddr, �Ѿ��ǿռ䱻��չ֮��ĵ�ַ
		 * @param iSize
		 */
		void append(void *pAddr, size_t iSize);

		void* getHead() const { return _pHead; }

		/**
		 * �ܼ��ڴ��С, �����������ӵ��ڴ��Ĵ�С
		 *
		 * @return size_t
		 */
		size_t getMemSize() const { return _pHead->_iTotalSize; }

		/**
		 * ���ڴ�����ݵ��ܼ��ڴ��С, �����������ӵ��ڴ��Ĵ�С
		 * 
		 */
		size_t getAllCapacity();

		/**
		 * ÿ��chunk�Ĵ�С, �����������ӵ��ڴ��Ĵ�С
		 *
		 * vector<size_t>
		 */
		void   getSingleCapacity(vector<size_t> &vec_capacity);

		/**
		 * ���ݸ��ڴ�����TC_Span����ʼҳ�͸�TC_Span����С��𻮷ֺ�ĵ�iIndex��������ɾ��Ե�ַ
		 * @param iPageId
		 * @param iIndex
		 * @return void*
		 */
		void* getAbsolute(size_t iPageId, size_t iIndex);

		/**
		 * ����һ������,���Ե�ַ
		 *
		 * @param iNeedSize,��Ҫ����Ĵ�С
		 * @param iAllocSize, ��������ݿ��С
		 * @return void*
		 */
		void* allocate(size_t iNeedSize, size_t &iAllocSize);

		/**
		 * �ͷ�����, ���Ե�ַ
		 * @param pAddr
		 */
		void  deallocate(void* pAddr);

		/**
		 * ����һ������
		 * @param iNeedSize,��Ҫ����Ĵ�С
		 * @param iAllocSize, ��������ݿ��С
		 * @param iPageId, ���ڴ�������TC_Span����ʼҳ��
		 * @param iIndex, ���ڴ�������TC_Span�İ�iAllocSize��С���ֺ�ĵ�iIndex��
		 * @return void *
		 */
		void* allocate(size_t iNeedSize, size_t &iAllocSize, size_t &iPageId, size_t &iIndex);

		/**
		 * �ͷ��ڴ�, ���ݸ��ڴ�����TC_Span����ʼҳ�͸�TC_Span����С��𻮷ֺ�ĵ�iIndex��
		 * @param iPageId, ���ڴ�������TC_Span����ʼҳ��
		 * @param iIndex, ���ڴ�������TC_Span�İ�iAllocSize��С���ֺ�ĵ�iIndex��
		 */
		void  deallocate(size_t iPageId, size_t iIndex);

		/**
		 * �ؽ�
		 */
		void rebuild();

		/**
		 * �޸ĸ��µ��ڴ���
		 */
		void doUpdate(bool bUpdate = false);

		/**
		 * ͷ���ڴ��
		 */
		struct tagChunkAllocatorHead
		{
			bool   _bProtectedArea;
			size_t _iSize;
			size_t _iTotalSize;
			size_t _iNext;
		}__attribute__((packed));

		/**
		 * ͷ���ڴ���С
		 */
		static size_t getHeadSize() { return sizeof(tagChunkAllocatorHead); }

		/**
		 * ���ݸ����ڴ���������ڴ���СҪ��С�ں����ķ���ֵ
		 */
		static size_t getNeedMinSize() { return sizeof(tagChunkAllocatorHead) + TC_Page::getMinMemSize(); }
	protected:
		void init(void *pAddr);

		void _connect(void *pAddr);

		TC_MallocChunkAllocator *lastAlloc();

		//��ֹcopy����
		TC_MallocChunkAllocator(const TC_MallocChunkAllocator &);

		//��ֹ����
		TC_MallocChunkAllocator& operator=(const TC_MallocChunkAllocator &);

	private:
		/**
		 * ͷָ��
		 */
		tagChunkAllocatorHead   *_pHead;
		/**
		 *  chunk��ʼ��ָ��
		 */
		void                    *_pChunk;
		/**
		 *  chunk������
		 */
		TC_Page                 _page;
		/**
		 * �����Ķ�������
		 */
		TC_MallocChunkAllocator *_nallocator;
	};
	
}
#endif
