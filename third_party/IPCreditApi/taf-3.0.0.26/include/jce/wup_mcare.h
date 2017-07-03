// **********************************************************************
// WUP For MCare version 1.0.0 by WSRD Tencent.
// **********************************************************************
#ifndef __WUP_SYMBIAN_H__
#define __WUP_SYMBIAN_H__

#include <stdlib.h>
#include "Jce_mcare.h"
#include "RequestF_mcare.h"

/************************************************************************************************** 
 *  WUPЭ�鷵��ֵ����
 */
enum ENUM_WUP_ERRORCODE
{
    WUP_SUCCESS         =  0,   //�ɹ�
    WUP_ATTR_NOT_FOUND  = -1,   //����û���ҵ�
    WUP_ENCODE_ERROR    = -2,   //�������
    WUP_DECODE_ERROR    = -3,   //�������
    WUP_RUNTIME_ERROR   = -4,   //��������ʱ����
};

/************************************************************************************************** 
 *  WUPЭ���ڲ�������
 */
#define STATUS_RESULT_CODE (Char*)"STATUS_RESULT_CODE"
#define STATUS_RESULT_DESC (Char*)"STATUS_RESULT_DESC"

/************************************************************************************************** 
 *  WUP���Է�װ��
 */
class UniAttribute
{
public:
    UniAttribute();
    virtual ~UniAttribute();

public:
    /**
     * �������ֵ
     * 
     * @param T:   ��������
     * @param name:��������
     * @param t:   ����ֵ
     * @return��   ��������������ֵ����
     */
    Int32 putAttr(JceOutputStream &os, const String &key, const String & type_name);
    
    #define DO_PUT  {_os->reset(); _os->write(t, 0); return putAttr(*_os, name, getStructName(t));  }    
	Int32 put(const String & name, const Bool &t);
	Int32 put(const String & name, const Char &t);
	Int32 put(const String & name, const Short &t);
	Int32 put(const String & name, const Int32 &t);
	Int32 put(const String & name, const Int64 &t);
	Int32 put(const String & name, const Float &t);
	Int32 put(const String & name, const Double &t);
	Int32 put(const String & name, const String &t);
	Int32 put(const String & name, const JceStructBase &t);
    Int32 put(const String & name, const UInt8 &t);
    Int32 put(const String & name, const UInt16 &t);
    Int32 put(const String & name, const UInt32 &t);

    template <typename T>
    Int32 put (const String& name, const JArray<T> &t) { DO_PUT }

    template <typename K, typename V>
    Int32 put (const String& name, const JMapWrapper<K, V> &t) { DO_PUT }

    /**
     * ��ȡ����ֵ
     * 
     * @param T:   ��������
     * @param name:��������
     * @param t:   ����ֵ�������
     * @return��   ��������������ֵ����
     */
    Int32 getAttr(JceInputStream &is, const String &key, const String & type_name);
 
    #define DO_GET \
    int ret = getAttr(*_is, name, getStructName(t)); \
    if(ret == WUP_SUCCESS) \
    { \
        ret = _is->read(t, 0, true);\
        if(ret!=WUP_SUCCESS) \
        { \
            _sLastError = _is->getLastError(); \
            return WUP_DECODE_ERROR; \
        } \
        return WUP_SUCCESS; \
    } \
    else \
        return ret; \
    
    Int32 get(const String& name,  Bool &t);
    Int32 get(const String& name,  Char &t);
    Int32 get(const String& name,  Short &t);
    Int32 get(const String& name,  Int32 &t);
    Int32 get(const String& name,  Int64 &t);
    Int32 get(const String& name,  Float &t);
    Int32 get(const String& name,  Double &t);
    Int32 get(const String& name,  String &t);
    Int32 get(const String& name,  JceStructBase &t);
    Int32 get(const String& name,  UInt8 &t);
    Int32 get(const String& name,  UInt16 &t);
    Int32 get(const String& name,  UInt32 &t);

    template < typename T >
    Int32 get (const String& name,  JArray<T> &t) {DO_GET}

    template <typename K, typename V >
    Int32 get (const String& name,  JMapWrapper<K, V> &t) {DO_GET}
    
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
    void clear() ;
    
    /** ����
     * 
     * @param buff���������������
     * @return��    ��������������ֵ����
     */
    Int32 encode(String& buff);

    /** ����
     * 
     * @param buff���������������
     * @return��    ��������������ֵ����
     */
    //howlpan IMPORT_C Int32 encode(JArray<char>& buff);
    Int32 encode(JArray<Char>& buff);

    /** ����
     * 
     * @param buff�������ű�������bufferָ��
     * @param len�� ����buff���ȣ��������������
     * @return��    ��������������ֵ����
     */
    Int32 encode(Char * buff, unsigned int & len);

    /** ����
     * 
     * @param buff���������ֽ�����bufferָ��
     * @param len�� �������ֽ����ĳ���
     * @return��    ��������������ֵ����
     */
    Int32 decode(const Char * buff, unsigned int len);

    /**
     * ����
     * 
     * @param buff�� ��������ֽ���
     * @return��    ��������������ֵ����
     */
    Int32 decode(const JArray<Char>& buff);

    /**
     * ��ȡ���е�����
     * @return const JMapWrapper<String,JMapWrapper<String,JArray<char>>>& ������map
     */
    const JMapWrapper<String, JMapWrapper<String, JArray<Char> > >& getData() const;

    /**
     * �ж����Լ����Ƿ�Ϊ��
     * 
     * @return bool
     */
    bool isEmpty();

    /**
     * ��ȡ���Լ��ϴ�С
     * 
     * @return size_t�����ϴ�С
     */
    unsigned int size();
    /**
     * ��ȡ���һ�������������Ϣ
     * 
     * @return string�������������Ϣ
     */
    String getLastError();

protected:
    JceOutputStream* _os;
    JceInputStream* _is;

    JMapWrapper<String, JMapWrapper<String, JArray<signed char> > > _data;
    
    String _sLastError; 
};

/////////////////////////////////////////////////////////////////////////////////
// ���󡢻�Ӧ����װ��
class UniPacket : protected RequestPacket, public UniAttribute
{
public:
    /**
     * ���캯��
     */
    UniPacket() ;
    virtual ~UniPacket() {}

    UniPacket* createResponseLC();

    UniPacket* createResponseL();
    
    /**
     * ���룬����İ�ͷ4���ֽ�Ϊ�������ĳ��ȣ������ֽ���
     * 
     * @param buff�� �������������
     * @return��    ��������������ֵ����
     */
    Int32 encode(String& buff);

    /**
     * ���룬����İ�ͷ4���ֽ�Ϊ�������ĳ��ȣ������ֽ���
     * 
     * @param buff�� �������������
     * @return��    ��������������ֵ����
     */
    Int32 encode(JArray<Char>& buff);

    /**
     * ���룬����İ�ͷ4���ֽ�Ϊ�������ĳ��ȣ������ֽ���
     * @throw runtime_error
     * @param buff����ű�������bufferָ��
     * @param len�� ����buff���ȣ��������������
     * @return��    ��������������ֵ����
     */
    Int32 encode(Char * buff, unsigned int & len);

    /** ����
     * 
     * @param buff���������ֽ�����bufferָ��
     * @param len�� �������ֽ����ĳ���
     * @return��    ��������������ֵ����
     */
    Int32 decode(const Char * buff, unsigned int len);

public:
    /**
     * ��ȡ��ϢID
     * @return Int32
     */
    Int32 getRequestId() const;

    /**
     * ��������ID
     * @param value
     */
    void setRequestId(Int32 value) ;

    /**
     * ��ȡ��������
     * @return const String&
     */
    const String& getServantName() const;

    /**
     * ���ö�������
     * @param value
     */
    void setServantName(const String& value);

    /**
     * ��ȡ������
     * @return const String&
     */
    const String& getFuncName() const;

    /**
     * ���÷�����
     * @param value
     */
    void setFuncName(const String& value);

protected:
    /**
     * �ڲ�����
     */
    Int32 doEncode(JceOutputStream & os);
};

// ����TAF�ķ���ʱʹ�õ���
struct TafUniPacket : public UniPacket
{
public:
    TafUniPacket();
    ~TafUniPacket();

public:
    /**
     * ����Э��汾
     * @param value
     */
    void setTafVersion(Short value);

    /**
     * ���õ�������
     * @param value
     */
    void setTafPacketType(Char value);

    /**
     * ������Ϣ����
     * @param value
     */
    void setTafMessageType(Int32 value);

    /**
     * ���ó�ʱʱ��
     * @param value
     */
    void setTafTimeout(Int32 value);

    /**
     * ���ò�����������
     * @param value
     */
    void setTafBuffer(const JArray<Char>& value);

    /**
     * ����������
     * @param value
     */
    void setTafContext(const JMapWrapper<String, String>& value);

    /**
     * ����������Ϣ��״ֵ̬
     * @param value
     */
    void setTafStatus(const JMapWrapper<String, String>& value);

    /**
     * ��ȡЭ��汾
     * @return Short
     */
    Short getTafVersion() const;

    /**
     * ��ȡ��������
     * @return Char
     */
    Char getTafPacketType() const ;

    /**
     * ��ȡ��Ϣ����
     * @return Int32
     */
    Int32 getTafMessageType() const ;

    /**
     * ��ȡ��ʱʱ��
     * @return Int32
     */
    Int32 getTafTimeout() const;

    /**
     * ��ȡ�������������
     * @return const JArray<Char>&
     */
    const JArray<Char>& getTafBuffer() const;

    /**
     * ��ȡ������
     * @return const JMapWrapper<String,String>&
     */
    const JMapWrapper<String, String>& getTafContext() const;

    /**
     * ��ȡ������Ϣ��״ֵ̬
     * @return const JMapWrapper<String,String>&
     */
    const JMapWrapper<String, String>& getTafStatus() const;

    /**
     * ��ȡ����taf�ķ���ֵ
     * @retrun taf::Int32
     */
    Int32 getTafResultCode() const;

    /**
     * ��ȡ����taf�ķ�������
     * @retrun string
     */
    JString getTafResultDesc() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////
#endif

