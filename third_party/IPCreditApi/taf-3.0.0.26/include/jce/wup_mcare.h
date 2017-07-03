// **********************************************************************
// WUP For MCare version 1.0.0 by WSRD Tencent.
// **********************************************************************
#ifndef __WUP_SYMBIAN_H__
#define __WUP_SYMBIAN_H__

#include <stdlib.h>
#include "Jce_mcare.h"
#include "RequestF_mcare.h"

/************************************************************************************************** 
 *  WUP协议返回值定义
 */
enum ENUM_WUP_ERRORCODE
{
    WUP_SUCCESS         =  0,   //成功
    WUP_ATTR_NOT_FOUND  = -1,   //属性没有找到
    WUP_ENCODE_ERROR    = -2,   //编码错误
    WUP_DECODE_ERROR    = -3,   //解码错误
    WUP_RUNTIME_ERROR   = -4,   //其他运行时错误
};

/************************************************************************************************** 
 *  WUP协议内部常定义
 */
#define STATUS_RESULT_CODE (Char*)"STATUS_RESULT_CODE"
#define STATUS_RESULT_DESC (Char*)"STATUS_RESULT_DESC"

/************************************************************************************************** 
 *  WUP属性封装类
 */
class UniAttribute
{
public:
    UniAttribute();
    virtual ~UniAttribute();

public:
    /**
     * 添加属性值
     * 
     * @param T:   属性类型
     * @param name:属性名称
     * @param t:   属性值
     * @return：   处理结果，见返回值定义
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
     * 获取属性值
     * 
     * @param T:   属性类型
     * @param name:属性名称
     * @param t:   属性值输出参数
     * @return：   处理结果，见返回值定义
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
     * 获取属性值，不存在的属性返回缺省值
     * 
     * @param T:   属性类型
     * @param name:属性名称
     * @param t:   属性值输出参数
     * @param def: 默认值
     * @return：    处理结果，见返回值定义
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
     *清除全部属性值
     */
    void clear() ;
    
    /** 编码
     * 
     * @param buff：编码结果输出参数
     * @return：    处理结果，见返回值定义
     */
    Int32 encode(String& buff);

    /** 编码
     * 
     * @param buff：编码结果输出参数
     * @return：    处理结果，见返回值定义
     */
    //howlpan IMPORT_C Int32 encode(JArray<char>& buff);
    Int32 encode(JArray<Char>& buff);

    /** 编码
     * 
     * @param buff：输出存放编码结果的buffer指针
     * @param len： 输入buff长度，输出编码结果长度
     * @return：    处理结果，见返回值定义
     */
    Int32 encode(Char * buff, unsigned int & len);

    /** 解码
     * 
     * @param buff：待解码字节流的buffer指针
     * @param len： 待解码字节流的长度
     * @return：    处理结果，见返回值定义
     */
    Int32 decode(const Char * buff, unsigned int len);

    /**
     * 解码
     * 
     * @param buff： 待解码的字节流
     * @return：    处理结果，见返回值定义
     */
    Int32 decode(const JArray<Char>& buff);

    /**
     * 获取已有的属性
     * @return const JMapWrapper<String,JMapWrapper<String,JArray<char>>>& ：属性map
     */
    const JMapWrapper<String, JMapWrapper<String, JArray<Char> > >& getData() const;

    /**
     * 判断属性集合是否为空
     * 
     * @return bool
     */
    bool isEmpty();

    /**
     * 获取属性集合大小
     * 
     * @return size_t：集合大小
     */
    unsigned int size();
    /**
     * 获取最后一个错误的描述信息
     * 
     * @return string：错误的描述信息
     */
    String getLastError();

protected:
    JceOutputStream* _os;
    JceInputStream* _is;

    JMapWrapper<String, JMapWrapper<String, JArray<signed char> > > _data;
    
    String _sLastError; 
};

/////////////////////////////////////////////////////////////////////////////////
// 请求、回应包封装类
class UniPacket : protected RequestPacket, public UniAttribute
{
public:
    /**
     * 构造函数
     */
    UniPacket() ;
    virtual ~UniPacket() {}

    UniPacket* createResponseLC();

    UniPacket* createResponseL();
    
    /**
     * 编码，结果的包头4个字节为整个包的长度，网络字节序
     * 
     * @param buff： 编码结果输出参数
     * @return：    处理结果，见返回值定义
     */
    Int32 encode(String& buff);

    /**
     * 编码，结果的包头4个字节为整个包的长度，网络字节序
     * 
     * @param buff： 编码结果输出参数
     * @return：    处理结果，见返回值定义
     */
    Int32 encode(JArray<Char>& buff);

    /**
     * 编码，结果的包头4个字节为整个包的长度，网络字节序
     * @throw runtime_error
     * @param buff：存放编码结果的buffer指针
     * @param len： 输入buff长度，输出编码结果长度
     * @return：    处理结果，见返回值定义
     */
    Int32 encode(Char * buff, unsigned int & len);

    /** 解码
     * 
     * @param buff：待解码字节流的buffer指针
     * @param len： 待解码字节流的长度
     * @return：    处理结果，见返回值定义
     */
    Int32 decode(const Char * buff, unsigned int len);

public:
    /**
     * 获取消息ID
     * @return Int32
     */
    Int32 getRequestId() const;

    /**
     * 设置请求ID
     * @param value
     */
    void setRequestId(Int32 value) ;

    /**
     * 获取对象名称
     * @return const String&
     */
    const String& getServantName() const;

    /**
     * 设置对象名称
     * @param value
     */
    void setServantName(const String& value);

    /**
     * 获取方法名
     * @return const String&
     */
    const String& getFuncName() const;

    /**
     * 设置方法名
     * @param value
     */
    void setFuncName(const String& value);

protected:
    /**
     * 内部编码
     */
    Int32 doEncode(JceOutputStream & os);
};

// 调用TAF的服务时使用的类
struct TafUniPacket : public UniPacket
{
public:
    TafUniPacket();
    ~TafUniPacket();

public:
    /**
     * 设置协议版本
     * @param value
     */
    void setTafVersion(Short value);

    /**
     * 设置调用类型
     * @param value
     */
    void setTafPacketType(Char value);

    /**
     * 设置消息类型
     * @param value
     */
    void setTafMessageType(Int32 value);

    /**
     * 设置超时时间
     * @param value
     */
    void setTafTimeout(Int32 value);

    /**
     * 设置参数编码内容
     * @param value
     */
    void setTafBuffer(const JArray<Char>& value);

    /**
     * 设置上下文
     * @param value
     */
    void setTafContext(const JMapWrapper<String, String>& value);

    /**
     * 设置特殊消息的状态值
     * @param value
     */
    void setTafStatus(const JMapWrapper<String, String>& value);

    /**
     * 获取协议版本
     * @return Short
     */
    Short getTafVersion() const;

    /**
     * 获取调用类型
     * @return Char
     */
    Char getTafPacketType() const ;

    /**
     * 获取消息类型
     * @return Int32
     */
    Int32 getTafMessageType() const ;

    /**
     * 获取超时时间
     * @return Int32
     */
    Int32 getTafTimeout() const;

    /**
     * 获取参数编码后内容
     * @return const JArray<Char>&
     */
    const JArray<Char>& getTafBuffer() const;

    /**
     * 获取上下文
     * @return const JMapWrapper<String,String>&
     */
    const JMapWrapper<String, String>& getTafContext() const;

    /**
     * 获取特殊消息的状态值
     * @return const JMapWrapper<String,String>&
     */
    const JMapWrapper<String, String>& getTafStatus() const;

    /**
     * 获取调用taf的返回值
     * @retrun taf::Int32
     */
    Int32 getTafResultCode() const;

    /**
     * 获取调用taf的返回描述
     * @retrun string
     */
    JString getTafResultDesc() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////
#endif

