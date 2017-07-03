#ifndef __TAF_JCE_CURRENT_H_
#define __TAF_JCE_CURRENT_H_

#include "util/tc_epoll_server.h"
#include "jce/RequestF.h"
#include "servant/BaseF.h"

namespace taf
{
class ServantHandle;

/**
 * 当前请求的上下文
 */
class JceCurrent : public TC_HandleBase
{
public:
    typedef std::map<string, string>    TAF_STATUS;

    typedef std::vector<char>           TAF_BUFFER;

    /**
     * 构造函数
     * @param pServantHandle
     */
    JceCurrent(ServantHandle *pServantHandle);

    /**
     * 析构
     */
    ~JceCurrent();

    /**
     * 获取IP
     * @return string
     */
    string getIp() const;

    /**
     * 获取端口
     * @return int
     */
    int getPort() const;


    /**
     * 获取uid
     * @return uint32
     */
	uint32_t getUId() const;

	/**
     * 获取fd
     * @return int
     */
	int getFd() const { return _fd; }

    /**
     * 是否函数返回时发送响应包给客户端
     * @return bool
     */
    bool isResponse() const;

    /**
     * 是否需要负载值
     * @return bool
     */
	bool isNeedSetLoaded() const;

    /**
     * 设置服务端负载值，该负载值反应目前服务端的负载情况
     * 如果客户端要求通过负载值路由，但是业务没有设置，则框架默认取当前队列大小
     * @param load 负载值
     */
	void setLoaded(size_t load);

	/**
	 * 设置连接的关闭类型，详情参看TC_EpollServer::EM_CLOSE_T
	 */
	void setCloseType(int type);
    /**
     * 获取连接关闭类型，详情请参考TC_EpollServer::EM_CLOSE_T类型
     */
	int getCloseType() const;

    /**
     * 设置是否自动回响应包
     */
    void setResponse(bool value) { _bResponse = value; }


    /**
     * 设置返回的context
     */
    void setResponseContext(const map<std::string, std::string> & context){_responseContext = context;}

    const map<std::string, std::string> & getResponseContext() const {return _responseContext;}

    /**
     * 关闭当前连接
     */
    void close();

    /**
     * 获取所属的ServantHandle
     */
    ServantHandle* getServantHandle();

    /**
     * 获取来源的Adapter
     * @return TC_EpollServer::BindAdapter*
     */
    TC_EpollServer::BindAdapter* getBindAdapter();

    /**
     * 获取请求buffer
     * @return string
     */
    const vector<char> &getRequestBuffer() const;

    /**
     * 获取服务Servant名称
     * @return string
     */
    string getServantName() const;

    /**
     * 请求的协议的版本号(仅TAF协议有效)
     *
     * @return short
     */
    short getRequestVersion() const;

    /**
     * 扩展map(仅TAF协议有效)
     * @return map<string,string>&
     */
    map<string, string>& getContext();

    /**
     * 保存状态信息，比如灰度、染色等(仅TAF协议有效)
     * @return map<string,string>&
     */
    const map<string, string>& getRequestStatus() const;

    /**
     * 函数名称(仅TAF协议有效)
     * @return string
     */
    string getFuncName() const;

    /**
     * 请求ID(仅TAF协议有效)
     * @return int
     */
    uint32_t getRequestId() const;


    /**
     * 获取包类型(仅TAF协议有效)
     * @return char
     */
    char getPacketType() const;

    /**
     * 获取消息类型(仅TAF协议有效)
     * @return taf::Int32
     */
    taf::Int32 getMessageType() const;

	/**
	 * 获取接收到请求的时间
	 */
	struct timeval getRecvTime() const;

	/**
	 * 获取按set规则调用的set名称
	 * @returun string
	 */
	string getSetName() const;
	/**
	 * 设置是否上报状态报告
	 */
	void setReportStat(bool bReport);


    /**
     * taf协议的发送响应数据(仅TAF协议有效)
     * @param iRet
     * @param status
     * @param buffer
     */
    void sendResponse(int iRet, const vector<char>& buffer = TAF_BUFFER(),
                      const map<string, string>& status = TAF_STATUS(),
                      const string & sResultDesc = "");

    /**
     * 普通协议的发送响应数据(非TAF协议有效)
     * @param buff
     * @param len
     */
    void sendResponse(const char* buff, uint32_t len);

protected:

    friend class ServantHandle;

    friend class Application;

    /**
     * 初始化
     * @param stRecvData
     */
    void initialize(const TC_EpollServer::tagRecvData &stRecvData);

	void initialize(const TC_EpollServer::tagRecvData &stRecvData, int64_t beginTime);

	/**
     * 初始化
     * @param stRecvData
     */
    void initializeClose(const TC_EpollServer::tagRecvData &stRecvData);

    /**
     * 初始化
     * @param sRecvBuffer
     */
    void initialize(const string &sRecvBuffer);

    /**
     * 服务端上报状态，针对单向调用及WUP调用(仅对TAF协议有效)
     */
    void reportToStat(const string & sObj);

    /**
    * 保存采样信息(仅对TAF协议有效)
    */
    void saveSampleKey();

protected:
    /**
     * 操作类指针
     */
    ServantHandle*			_pServantHandle;

    /**
     * 消息_pBindAdapter
     */
    TC_EpollServer::BindAdapter* _pBindAdapter;

    /**
     * 唯一标识
     */
    uint32_t                _uid;

    /**
     * ip地址
     */
    string                  _ip;

    /**
     * 端口
     */
    int                     _port;

	/**
     * 用于回包时选择网络线程
     */
	int						_fd;

    /**
     * 客户端请求包
     */
    RequestPacket			_request;

    /**
     * 响应
     */
    bool                    _bResponse;

    /**
     * 收到请求时间
     */
    int64_t                _begintime;

    /**
     * 接口处理的返回值
     */
    int                     _iRet;

	/**
     * 是否上报stat
     */
	bool					_bReportStat;

	//业务是否自己设置了负载值，不设置，如果客户端要求返回，则采用服务端队列大小
	bool 	                _bSetLoaded;
	size_t                  _iLoaded;
	//连接关闭的类型，初始值是-1，非法值
	int              _closeType;

    map<std::string, std::string> _responseContext;
};
//////////////////////////////////////////////////////////////
}
#endif
