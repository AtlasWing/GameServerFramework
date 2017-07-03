// **********************************************************************
// WUP version 1.0.5 by WSRD Tencent.
// **********************************************************************

#ifndef __WUP_SYMBIAN_H__
#define __WUP_SYMBIAN_H__

#include <stdlib.h>
#include "Jce_sym.h"
#include "RequestF_sym.h"

using namespace taf;

namespace wup
{

//���taf����ֵ��key
#define STATUS_RESULT_CODE (Char*)"STATUS_RESULT_CODE"
#define STATUS_RESULT_DESC (Char*)"STATUS_RESULT_DESC"

////////////////////////////////////////////////////////////////////////////////////////////////
//����ֵ����
// extern const taf::Int32 WUP_SUCCESS        ;      //�ɹ�
// extern const taf::Int32 WUP_ATTR_NOT_FOUND ;      //���Բ�����
// extern const taf::Int32 WUP_ENCODE_ERROR   ;      //�������
// extern const taf::Int32 WUP_DECODE_ERROR   ;      //�������
// extern const taf::Int32 WUP_RUNTIME_ERROR  ;      //��������ʱ����

#define  WUP_SUCCESS          0      //�ɹ�
#define  WUP_ATTR_NOT_FOUND  -1      //���Բ�����
#define  WUP_ENCODE_ERROR    -2      //�������
#define  WUP_DECODE_ERROR    -3      //�������
#define  WUP_RUNTIME_ERROR   -4      //��������ʱ����


/////////////////////////////////////////////////////////////////////////////////
// ���Է�װ��

class UniAttribute
{
public:
    IMPORT_C UniAttribute();
    IMPORT_C virtual ~UniAttribute();

public:
    /**
     * �������ֵ
     * 
     * @param T:   ��������
     * @param name:��������
     * @param t:   ����ֵ
     * @return��    ��������������ֵ����
     */

    IMPORT_C Int32 putAttr(taf::JceOutputStream &os, const String &key);
    
#define DO_PUT  {_os->reset(); _os->write(t, 0); return putAttr(*_os, name);  }    
    
    IMPORT_C Int32 put (const String& name, const Bool &t);
    IMPORT_C Int32 put (const String& name, const Char &t);
    IMPORT_C Int32 put (const String& name, const Short &t);
    IMPORT_C Int32 put (const String& name, const Int32 &t);
    IMPORT_C Int32 put (const String& name, const Int64 &t);
    IMPORT_C Int32 put (const String& name, const Float &t);
    IMPORT_C Int32 put (const String& name, const Double &t);
    IMPORT_C Int32 put (const String& name, const String &t);
    IMPORT_C Int32 put (const String& name, const JceStructBase &t);

    //add by edwardsu
    IMPORT_C Int32 put (const String& name, const UInt8 &t);
    IMPORT_C Int32 put (const String& name, const UInt16 &t);
    IMPORT_C Int32 put (const String& name, const UInt32 &t);
    //end add
 
    template < typename T >
    Int32 put (const String& name, const JArray<T> &t)
        { DO_PUT }

    template <typename K, typename V >
    Int32 put (const String& name, const JMapWrapper<K, V> &t)
        { DO_PUT }
    /**
     * ��ȡ����ֵ
     * 
     * @param T:   ��������
     * @param name:��������
     * @param t:   ����ֵ�������
     * @return��    ��������������ֵ����
     */

    IMPORT_C Int32 getAttr(taf::JceInputStream &is, const String &key);
 
#define DO_GET \
    int ret = getAttr(*_is, name); \
    if(ret == WUP_SUCCESS) \
    { \
        TRAPD(error, _is->read(t, 0, true);); \
        if(error) \
        { \
            _sLastError = _is->getLastError(); \
            return WUP_DECODE_ERROR; \
        } \
        return WUP_SUCCESS; \
    } \
    else \
        return ret; \
    
    IMPORT_C Int32 get (const String& name,  Bool &t);
    IMPORT_C Int32 get (const String& name,  Char &t);
    IMPORT_C Int32 get (const String& name,  Short &t);
    IMPORT_C Int32 get (const String& name,  Int32 &t);
    IMPORT_C Int32 get (const String& name,  Int64 &t);
    IMPORT_C Int32 get (const String& name,  Float &t);
    IMPORT_C Int32 get (const String& name,  Double &t);
    IMPORT_C Int32 get (const String& name,  String &t);
    IMPORT_C Int32 get (const String& name,  JceStructBase &t);

    //add by edwardsu
    IMPORT_C Int32 get (const String& name,  UInt8 &t);
    IMPORT_C Int32 get (const String& name,  UInt16 &t);
    IMPORT_C Int32 get (const String& name,  UInt32 &t);
    //end add
    
    template < typename T >
    Int32 get (const String& name,  JArray<T> &t) 
        {DO_GET}

    template <typename K, typename V >
    Int32 get (const String& name,  JMapWrapper<K, V> &t)
        {DO_GET}


    
    /**
     * ��ȡ����ֵ�������ڵ����Է���ȱʡֵ
     * 
     * @param T:   ��������
     * @param name:��������
     * @param t:   ����ֵ�������
     * @param def: Ĭ��ֵ
     * @return��    ��������������ֵ����
     */
    template<typename T> Int32 getByDefault(const String& name, T& t, const T& def)
    {
        Int32 iRet = get(name, t);
        if(iRet == WUP_ATTR_NOT_FOUND)
        {
            t = def;
            return WUP_SUCCESS;
        }
        else
        {
            return iRet;
        }
    }
    
    
    /**
     *���ȫ������ֵ
     */
    IMPORT_C void clear() ;
    
    /** ����
     * 
     * @param buff���������������
     * @return��    ��������������ֵ����
     */
    IMPORT_C Int32 encode(String& buff);

    /** ����
     * 
     * @param buff���������������
     * @return��    ��������������ֵ����
     */
    //howlpan IMPORT_C Int32 encode(JArray<char>& buff);
    IMPORT_C Int32 encode(JArray<Char>& buff);

    /** ����
     * 
     * @param buff�������ű�������bufferָ��
     * @param len�� ����buff���ȣ��������������
     * @return��    ��������������ֵ����
     */
    IMPORT_C Int32 encode(char* buff, unsigned int & len);

    /** ����
     * 
     * @param buff���������ֽ�����bufferָ��
     * @param len�� �������ֽ����ĳ���
     * @return��    ��������������ֵ����
     */
    //howlpan IMPORT_C Int32 decode(const char* buff, unsigned int len);
    IMPORT_C Int32 decode(const Char* buff, unsigned int len);

    /**
     * ����
     * 
     * @param buff�� ��������ֽ���
     * @return��    ��������������ֵ����
     */
    //howlpan IMPORT_C Int32 decode(const JArray<char>& buff);
    IMPORT_C Int32 decode(const JArray<Char>& buff);

    /**
     * ��ȡ���е�����
     * @return const JMapWrapper<String,JMapWrapper<String,JArray<char>>>& ������map
     */
    IMPORT_C const JMapWrapper<String, JArray<Char> >& getData() const;

    /**
     * �ж����Լ����Ƿ�Ϊ��
     * 
     * @return bool
     */
    IMPORT_C bool isEmpty();

    /**
     * ��ȡ���Լ��ϴ�С
     * 
     * @return size_t�����ϴ�С
     */
    IMPORT_C unsigned int size();
    /**
     * ��ȡ���һ�������������Ϣ
     * 
     * @return string�������������Ϣ
     */
    IMPORT_C String getLastError();

protected:
    taf::JceOutputStream* _os;
    taf::JceInputStream* _is;

    JMapWrapper<String, JArray<signed char> > _data;
    
    String _sLastError; 
};

/////////////////////////////////////////////////////////////////////////////////
// ���󡢻�Ӧ����װ��

struct UniPacket : protected RequestPacket, public UniAttribute
{
    /**
     * ���캯��
     */
    IMPORT_C UniPacket() ;
    
    /**
     * ����������ɻ�Ӧ�������ṹ������ؼ���������Ϣ
     * 
     * @return UniPacket�� ��Ӧ��
     */
    //UniPacket createResponse();
    
    IMPORT_C UniPacket* createResponseLC();


    IMPORT_C UniPacket* createResponseL();
    
    void ConstructL()
    {

    }
    /**
     * ���룬����İ�ͷ4���ֽ�Ϊ�������ĳ��ȣ������ֽ���
     * 
     * @param buff�� �������������
     * @return��    ��������������ֵ����
     */
    IMPORT_C Int32 encode(String& buff);

    /**
     * ���룬����İ�ͷ4���ֽ�Ϊ�������ĳ��ȣ������ֽ���
     * 
     * @param buff�� �������������
     * @return��    ��������������ֵ����
     */
    IMPORT_C Int32 encode(JArray<char>& buff);

    /**
     * ���룬����İ�ͷ4���ֽ�Ϊ�������ĳ��ȣ������ֽ���
     * @throw runtime_error
     * @param buff����ű�������bufferָ��
     * @param len�� ����buff���ȣ��������������
     * @return��    ��������������ֵ����
     */
    IMPORT_C Int32 encode(char* buff, unsigned int & len);

    /** ����
     * 
     * @param buff���������ֽ�����bufferָ��
     * @param len�� �������ֽ����ĳ���
     * @return��    ��������������ֵ����
     */
    IMPORT_C Int32 decode(const signed char* buff, unsigned int len);

public:
    /**
     * ��ȡ��ϢID
     * @return Int32
     */
    IMPORT_C Int32 getRequestId() const;
    /**
     * ��������ID
     * @param value
     */
    IMPORT_C void setRequestId(Int32 value) ;
    /**
     * ��ȡ��������
     * @return const String&
     */
    IMPORT_C const String& getServantName() const;
    /**
     * ���ö�������
     * @param value
     */
    IMPORT_C void setServantName(const String& value);
    /**
     * ��ȡ������
     * @return const String&
     */
    IMPORT_C const String& getFuncName() const;
    /**
     * ���÷�����
     * @param value
     */
    IMPORT_C void setFuncName(const String& value);

protected:
    /**
     * �ڲ�����
     */
    void doEncode(taf::JceOutputStream & os);
};

/////////////////////////////////////////////////////////////////////////////////
// ����TAF�ķ���ʱʹ�õ���

struct TafUniPacket : public UniPacket
{

public:
    IMPORT_C TafUniPacket();
    IMPORT_C ~TafUniPacket();

public:
    /**
     * ����Э��汾
     * @param value
     */
    IMPORT_C void setTafVersion(Short value);
    /**
     * ���õ�������
     * @param value
     */
    IMPORT_C void setTafPacketType(Char value);
    /**
     * ������Ϣ����
     * @param value
     */
    IMPORT_C void setTafMessageType(Int32 value);
    /**
     * ���ó�ʱʱ��
     * @param value
     */
    IMPORT_C void setTafTimeout(Int32 value);
    /**
     * ���ò�����������
     * @param value
     */
    IMPORT_C void setTafBuffer(const JArray<Char>& value);
    /**
     * ����������
     * @param value
     */
    IMPORT_C void setTafContext(const JMapWrapper<String, String>& value);
    /**
     * ����������Ϣ��״ֵ̬
     * @param value
     */
    IMPORT_C void setTafStatus(const JMapWrapper<String, String>& value);
    /**
     * ��ȡЭ��汾
     * @return Short
     */
    IMPORT_C Short getTafVersion() const;
    /**
     * ��ȡ��������
     * @return Char
     */
    IMPORT_C Char getTafPacketType() const ;
    /**
     * ��ȡ��Ϣ����
     * @return Int32
     */
    IMPORT_C Int32 getTafMessageType() const ;
    /**
     * ��ȡ��ʱʱ��
     * @return Int32
     */
    IMPORT_C Int32 getTafTimeout() const;
    /**
     * ��ȡ�������������
     * @return const JArray<Char>&
     */
    IMPORT_C const JArray<Char>& getTafBuffer() const;
    /**
     * ��ȡ������
     * @return const JMapWrapper<String,String>&
     */
    IMPORT_C const JMapWrapper<String, String>& getTafContext() const;
    /**
     * ��ȡ������Ϣ��״ֵ̬
     * @return const JMapWrapper<String,String>&
     */
    IMPORT_C const JMapWrapper<String, String>& getTafStatus() const;

    /**
     * ��ȡ����taf�ķ���ֵ
     * @retrun taf::Int32
     */
    IMPORT_C taf::Int32 getTafResultCode() const;

    /**
     * ��ȡ����taf�ķ�������
     * @retrun string
     */
    IMPORT_C String getTafResultDesc() const;

};


}
////////////////////////////////////////////////////////////////////////////////////////////////
#endif
