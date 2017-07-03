
#ifndef __TC_SQueue_H__
#define __TC_SQueue_H__

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include <iostream>

/**
 * �ṹ����queue����һ�߶�һ��д������¿��Բ��ü��������߳�(����)��ȫ��
 * ������ͬʱ��д����Ҫ����
 * ����һ��������ǣ���һ�߶�һ��д������²�һ��׼ȷ��ȫ׼ȷ 
 * �����wbl��squeueǨ�ƶ���
*/

namespace taf {

class TC_SQueue
{
public:
	TC_SQueue() {_header = NULL;_data = NULL;}
	~TC_SQueue() {_header = NULL;_data = NULL;}

	void attach(char* pBuf, unsigned long iBufSize) throw (TC_Exception)
	{
		if(iBufSize <= sizeof(Header)+MarkLen+ReserveLen) {
			throw TC_Exception("TC_SQueue::attach fail:iBufSize is too small");
		}

		_header = (Header *)pBuf;
		_data = pBuf+sizeof(Header);
		if(_header->iBufSize != iBufSize - sizeof(Header))
			throw TC_Exception("TC_SQueue::attach fail: iBufSize != iBufSize - sizeof(Header);");
		if(_header->iReserveLen != ReserveLen)
			throw TC_Exception("TC_SQueue::attach fail: iReserveLen != ReserveLen");
		if(_header->iBegin >= _header->iBufSize)
			throw TC_Exception("TC_SQueue::attach fail: iBegin > iBufSize - sizeof(Header);");
		if(_header->iEnd > iBufSize - sizeof(Header))
			throw TC_Exception("TC_SQueue::attach fail: iEnd > iBufSize - sizeof(Header);");
	}

	void create(char* pBuf, unsigned long iBufSize) throw (TC_Exception)
	{
		if(iBufSize <= sizeof(Header)+MarkLen+ReserveLen) {
			throw TC_Exception("TC_SQueue::create fail:iBufSize is too small");
		}

		_header = (Header *)pBuf;
		_data = pBuf+sizeof(Header);
		_header->iBufSize = iBufSize - sizeof(Header);
		_header->iReserveLen = ReserveLen;
		_header->iBegin = 0;
		_header->iEnd = 0;
		_header->iNum = 0;
	}

	bool pop(string& buffer)
	{
		unsigned long iEnd=_header->iEnd;
		unsigned tmp_num;
		if(_header->iBegin == iEnd) {
            _header->iNum = 0;
			return false;
        }
		else if(_header->iBegin<iEnd) {
			assert(_header->iBegin+MarkLen < iEnd);
			unsigned long len = GetLen(_data+_header->iBegin);
			assert(_header->iBegin+MarkLen+len <= iEnd);
			
			buffer.assign(_data+_header->iBegin+MarkLen, len);
			_header->iBegin += len+MarkLen;
			tmp_num = _header->iNum;
			if(tmp_num > 0) 
				_header->iNum = tmp_num-1;
		} else {
			// ���ֶ�
			assert(iEnd+ReserveLen <= _header->iBegin);
			unsigned long len = 0;
			unsigned long new_begin = 0;
			char *data_from = NULL;
			char *data_to = NULL;
			assert(_header->iBegin < _header->iBufSize);
			// �����ֶ�Ҳ���ֶ�
			if(_header->iBegin+MarkLen > _header->iBufSize) { 
				char tmp[16];
				memcpy(tmp,_data+_header->iBegin,_header->iBufSize-_header->iBegin);
				memcpy(tmp+_header->iBufSize-_header->iBegin,_data,_header->iBegin+MarkLen-_header->iBufSize);
				len = GetLen(tmp);
				data_from = _data+(_header->iBegin+MarkLen-_header->iBufSize); //
				new_begin = _header->iBegin+MarkLen-_header->iBufSize+len;
				assert(new_begin <= iEnd);
			} else {
				len = GetLen(_data+_header->iBegin);
				data_from = _data+_header->iBegin+MarkLen;
				if(data_from == _data+_header->iBufSize) data_from = _data;
				if(_header->iBegin+MarkLen+len < _header->iBufSize) { 
					new_begin = _header->iBegin+MarkLen+len;
				} else { // ���ݱ��ֶ�
					new_begin = _header->iBegin+MarkLen+len-_header->iBufSize;
					assert(new_begin <= iEnd);
				}
			}
			data_to = _data+new_begin;

			if(data_to > data_from) {
				assert(data_to - data_from == (long)len);
				buffer.assign(data_from, len);
			} else {
				buffer.assign(data_from, _data+_header->iBufSize-data_from);
				buffer.append(_data, data_to-_data);
				assert(_header->iBufSize-(data_from-data_to)== len);
			}
			_header->iBegin = new_begin;
			tmp_num = _header->iNum;
			if(tmp_num > 0) 
				_header->iNum = tmp_num-1;
		}

		return true;
	}
	
	bool push(const string& buffer)
	{
		return push(buffer.c_str(), buffer.length());
	}
	// д��ʹ��
	bool push(const char *buffer,unsigned long len)
	{
		if(len == 0) return true;
		unsigned long iBegin = _header->iBegin;
		if(_header->iEnd == iBegin) {
            _header->iNum = 0;
			if(MarkLen+len+ReserveLen>_header->iBufSize) 
				return false;
		} else if(_header->iEnd > iBegin) {
			assert(iBegin+MarkLen < _header->iEnd);
			if(_header->iBufSize - _header->iEnd + iBegin < MarkLen+len+ReserveLen)
				return false;
		} else {
			assert(_header->iEnd+ReserveLen <= iBegin);
			if(iBegin - _header->iEnd < MarkLen+len+ReserveLen)
				return false;
		}

		// �����ֶα��ֶ�
		if(_header->iEnd+MarkLen > _header->iBufSize) {
			char tmp[16]; SetLen(tmp,len);
			memcpy(_data+_header->iEnd,tmp,_header->iBufSize-_header->iEnd);
			memcpy(_data,tmp+_header->iBufSize-_header->iEnd,_header->iEnd+MarkLen-_header->iBufSize);
			memcpy(_data+_header->iEnd+MarkLen-_header->iBufSize,buffer,len);
			_header->iEnd = len+_header->iEnd+MarkLen-_header->iBufSize;
			assert(_header->iEnd+ReserveLen <= iBegin);
			_header->iNum++;
		} 
		// ���ݱ��ֶ�
		else if(_header->iEnd+MarkLen+len > _header->iBufSize){
			SetLen(_data+_header->iEnd,len);
			memcpy(_data+_header->iEnd+MarkLen,buffer,_header->iBufSize-_header->iEnd-MarkLen);
			memcpy(_data,buffer+_header->iBufSize-_header->iEnd-MarkLen,len-(_header->iBufSize-_header->iEnd-MarkLen));
			_header->iEnd = len-(_header->iBufSize-_header->iEnd-MarkLen);
			assert(_header->iEnd+ReserveLen <= iBegin);
			_header->iNum++;
		} else {
			SetLen(_data+_header->iEnd,len);
			memcpy(_data+_header->iEnd+MarkLen,buffer,len);
			_header->iEnd = (_header->iEnd+MarkLen+len)%_header->iBufSize;
			_header->iNum++;
		}
		return true;
	}

	// ����ʹ��
	//bool isEmpty() const {unsigned long iEnd=_header->iEnd;return _header->iBegin == iEnd;}
	bool empty() const {unsigned long iEnd=_header->iEnd;return _header->iBegin == iEnd;}
	// д��ʹ��
	bool full(unsigned long len) const
	{
		unsigned long iBegin = _header->iBegin;
		if(len==0) return false;

		if(_header->iEnd == iBegin) {
			if(len+MarkLen+ReserveLen > _header->iBufSize) return true;
			return false;
		} else if(_header->iEnd > iBegin) {
			assert(iBegin+MarkLen < _header->iEnd);
			return _header->iBufSize - _header->iEnd + iBegin < MarkLen+len+ReserveLen;
		}
		assert(_header->iEnd+ReserveLen <= iBegin);
		return (iBegin - _header->iEnd < MarkLen+len+ReserveLen);
	}
	// ���ض������Ԫ����������һ������׼ȷ��ֻ����Ϊ�ο�
	unsigned long size() const 
    {
        if (empty()) {
			_header->iNum = 0;
        }
        return _header->iNum;
    } 

private:
	unsigned long GetLen(char *buf) {unsigned long u; memcpy((void *)&u,buf,MarkLen); return u;}
	void SetLen(char *buf,unsigned long u) {memcpy(buf,(void *)&u,MarkLen);}

private:
	const static unsigned long ReserveLen = 8;
	const static unsigned long MarkLen = sizeof(unsigned long);
	struct Header
	{
		unsigned long iBufSize;
		unsigned long iReserveLen; // must be 8
		unsigned long iBegin;
		unsigned long iEnd;
		unsigned long iNum;		// ����һ��������ǣ���һ��׼ȷ
	};

	Header *_header;
	char *_data;
};

}
#endif
