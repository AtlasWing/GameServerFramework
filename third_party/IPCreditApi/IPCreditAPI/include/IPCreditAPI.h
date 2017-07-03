#ifndef _IP_CREDIT_API_H
#define _IP_CREDIT_API_H

#include <ext/hash_map>
#include <iostream>
#include <string>
#include <map>
#include "servant/Communicator.h"
#include "IPCredit.h"
#include "FIPCredit.h"
#include "Toolkit.h" 
#include "log/taf_logger.h"
#include "util/tc_common.h"
#include "util/tc_mmap.h"
#include "util/tc_shm.h"
#include "util/tc_thread.h"
#include "memVector.h"
#include "ipcm_comm.h"
#include "ConfigDataWrapper.h"
#include "ClientReport.h"

using namespace std;
using namespace taf;
using namespace newIPPro;

template<typename T>
class MemVector;

typedef taf::TC_Logger<TC_DefaultWriteT, TC_RollBySize> MyRollLogger;

#if __WORDSIZE == 32
namespace __gnu_cxx {
template<>
struct hash<uint64_t> {
    size_t operator()(const uint64_t& key) const {
        if (sizeof(key) == sizeof(size_t)) {
            return key;
        } else {
            return (key >> 32) ^ (key & 0xFFFFFFFF);
        }
    }
}; // struct hash<uint64_t>
} // namespace __gnu_cxx
#endif // __WORDSIZE == 32

struct MemNode {
    MemNode() {
        memset(m_buffer, 0, sizeof(m_buffer));
    }
    
    MemNode(const MemNode& mem_node) {
        memcpy(m_buffer, mem_node.m_buffer, sizeof(m_buffer));
    }
    
    MemNode(const newIPPro::IPAttribute& message) {
        CopyFrom(message);
    }
    MemNode(const newIPPro::CodeInfo& message) {
        CopyFrom(message);
    }
    void CopyFrom(const newIPPro::IPAttribute& message) {
        taf::JceOutputStream<taf::BufferWriter> taf_buffer;
        message.writeTo(taf_buffer);
        InnerCopyFrom(taf_buffer);
    }
    void CopyFrom(const newIPPro::CodeInfo& message) {
        taf::JceOutputStream<taf::BufferWriter> taf_buffer;
        message.writeTo(taf_buffer);
        InnerCopyFrom(taf_buffer);
    }
    void InnerCopyFrom(const taf::JceOutputStream<taf::BufferWriter>& taf_buffer) {
        const char* data = taf_buffer.getBuffer();
        size_t size = taf_buffer.getLength();
        assert(size < 128u);
        memset(m_buffer, 0, sizeof(m_buffer));
        memcpy(m_buffer, &size, sizeof(size_t));
        memcpy(m_buffer + sizeof(size_t), data, size);
    }
    void ParseTo(newIPPro::IPAttribute* message) const {
        taf::JceInputStream<taf::BufferReader> taf_buffer;
        InnerParseTo(&taf_buffer);
        message->readFrom(taf_buffer);
    }
    void ParseTo(newIPPro::CodeInfo* message) const {
        taf::JceInputStream<taf::BufferReader> taf_buffer;
        InnerParseTo(&taf_buffer);
        message->readFrom(taf_buffer);
    }
    void InnerParseTo(taf::JceInputStream<taf::BufferReader>* taf_buffer) const {
        size_t size = *((size_t*)m_buffer);
        taf_buffer->setBuffer(m_buffer + sizeof(size_t), size);
    }
    char m_buffer[128];
}; // struct MemNode

struct MemNode_CodeInfo : public MemNode {
    bool operator < (const MemNode_CodeInfo& other) const {
        newIPPro::CodeInfo lhs, rhs;
        this->ParseTo(&lhs);
        other.ParseTo(&rhs);
        return lhs < rhs;
    }
}; // struct MemNode_CodeInfo

struct MemNode_IPAttribute : public MemNode {
    bool operator < (const MemNode_IPAttribute& other) const {
        newIPPro::IPAttribute lhs, rhs;
        this->ParseTo(&lhs);
        other.ParseTo(&rhs);
        return lhs < rhs;
    }
}; // struct MemNode_IPAttribute

typedef MemVector<MemNode_IPAttribute> IPVector;
typedef MemVector<MemNode_CodeInfo> CodeVector;

enum MemoryStatus
{
    kMemoryStatusReady = 0x01, ///< memory is usable
    kMemoryStatusPending = 0x02, ///< memory is pending
    kMemoryStatusUnknown = 0x03, ///< unknown status
};

struct MemoryHead
{
	MemoryStatus ip_usable;
	MemoryStatus code_usable;
	unsigned char pad0;
	unsigned char pad1;
	unsigned char pad2;
	unsigned char pad3;	
	unsigned char pad4;
	unsigned char pad5;	
}; // struct MemoryHead

class ProlatePointer
{
public:
    ProlatePointer() : m_ptr(NULL), m_size(0), m_data(NULL)
    {}
    explicit ProlatePointer(void* ptr)
    {
        ReSet(ptr);
    }
    ProlatePointer(void* data, size_t size)
    {
        ReSet(data, size);
    }
    virtual ~ProlatePointer()
    {
        m_ptr = NULL;
        m_size = 0;
        m_data = NULL;
    }
public:
    void ReSet(void* ptr)
    {
        assert(ptr != NULL);
        m_ptr = ptr;
        size_t* temp = reinterpret_cast<size_t*>(m_ptr);
        m_size = *temp;
        m_data = reinterpret_cast<void*>(temp + 1);
    }
    void ReSet(void* data, size_t size)
    {
        assert(data != NULL);
        m_data = data;
        m_size = size;
        size_t* temp = reinterpret_cast<size_t*>(m_data);
        --temp;
        *temp = m_size;
        m_ptr = reinterpret_cast<void*>(temp);
    }
    void* GetPtr() const
    {
        return m_ptr;
    }
    size_t Size() const
    {
        return m_size;
    }
    void* Data() const
    {
        return m_data;
    }
private:
    void* m_ptr;
    size_t m_size;
    void* m_data;
}; // class ProlatePointer

struct ShmSize
{
	size_t m_usable[16];
	size_t m_size;
	ShmSize()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct IPCreditAPIOptions {
    Communicator*  comm;
    std::string    ip_servant;
    std::string    fast_ip_servant;
    key_t          shm_key_1;
    key_t          shm_key_2;
    std::string    log_path;
    key_t          shm_key_3;
    IPCreditAPIOptions() {
        comm = NULL;
        log_path = "./";
        shm_key_3 = 600014;
    }
    IPCreditAPIOptions(
        Communicator* _comm, const std::string& _ip_servant, const std::string& _fast_ip_servant,
        key_t _shm_key_1, key_t _shm_key_2, const std::string& _log_path, key_t _shm_key_3, bool _report_stat)
        : comm(_comm), ip_servant(_ip_servant), fast_ip_servant(_fast_ip_servant),
          shm_key_1(_shm_key_1), shm_key_2(_shm_key_2), log_path(_log_path),
          shm_key_3(_shm_key_3){
    }
    IPCreditAPIOptions(const IPCreditAPIOptions& options) 
        : comm(options.comm), ip_servant(options.ip_servant), fast_ip_servant(options.fast_ip_servant),
          shm_key_1(options.shm_key_1), shm_key_2(options.shm_key_2), log_path(options.log_path),
          shm_key_3(options.shm_key_3) {
    }
    std::string ToString() const {
        std::ostringstream ostrs;
        ostrs << "[IPCreditAPIOptions]ip_servant=" << ip_servant
            << ", fast_ip_servant=" << fast_ip_servant
            << ", shm_key_1=" << shm_key_1
            << ", shm_key_2=" << shm_key_2
            << ", shm_key_3=" << shm_key_3
            << ", log_path=" << log_path;
        return ostrs.str();
    }
}; // struct IPCreditAPIOptions

struct ClientConfWrapper {
	newIPPro::ClientConf conf;
	set<string> citys;
};

class IPCreditAPI : public taf::TC_Thread, public taf::TC_ThreadLock
{
	enum
	{ 
	    STX = 0x02, 
	    ETX = 0x03, 
	    IPPROTOCOL_VERSION = 1, 
	    DOWNLOAD_CHUNK_LEN = 50000,
	    CHECK_CHANGE_INTERVAL = 600,
	    DISTRICTCODE_CHECK_CHANGE_INTERVAL = 604800,
	};

	enum {ADD=0,DELETE=1,MODIFY=2};

public:
	IPCreditAPI();
	
	virtual ~IPCreditAPI();

	virtual void run();

    taf::TC_ThreadControl start();
	
    bool initialize(const IPCreditAPIOptions& options_value);
	
	bool initialize(Communicator* comm,string ServantObj1,string ServantObj2,key_t shm_key_1, key_t shm_key_2, string logPath = "", key_t shm_key_3 = 600014);
	
	void InitializeDbInfo();

	void reload(const string& serverIP,const string& serverPort);
	
	bool getIPInfo(uint32_t IP, IPHumanMessage* message);
   
	bool getIPInfoFromStr(const std::string& strIP, IPHumanMessage* message);
	
	bool getIPInfo(uint32_t IP, IPInfo* info);
	
	bool getIPInfoFromStr(const string& strIP, IPInfo* info);
	
	string getCountryByID(uint32_t countryid);
	
	string getProvinceByID(uint32_t provid);
	
	string getCityByID(uint32_t cityid);
	
	string getDistrictByID(uint64_t districtid);
	
	string getExtendByID(uint32_t extendid);
	
	string getOperByID(uint32_t operid);
	
	string getNettypeByID(uint32_t nettypeid);
	
	string getApnByID(uint32_t apnid);
	
	string getFlableByID(uint32_t flableid);

	void getCountryByID(uint32_t id, std::string* result);
	
	void getProvinceByID(uint32_t id, std::string* result);
	
	void getCityByID(uint32_t id, std::string* result);
	
	void getDistrictByID(uint64_t id, std::string* result);
	
	void getExtendByID(uint32_t id, std::string* result);
	
	void getOperByID(uint32_t id, std::string* result);
	
	void getNettypeByID(uint32_t id, std::string* result);
	
	void getApnByID(uint32_t id, std::string* result);
	
	void getFlableByID(uint32_t id, std::string* result);

	bool getIPInfo4Shm(uint32_t IP,struct VALUE* value);

	void terminate();

    void set_destructor_wait(bool destructor_wait) {
    	m_destructor_wait = destructor_wait;
    }

private:
	MyRollLogger  _logger;
	TC_LoggerThreadGroup    _local;

    void InitializeShareMemory(key_t shm_key_1, key_t shm_key_2, key_t shm_key_3);
   
	void CalculateMd5FromShareMemory();
   
	void ReadMemoryHead(int shm_index, MemoryHead* memory_head);
   
	void WriteMemoryHead(int shm_index, const MemoryHead& memory_head);
    
	void ResizeCodeBuffer(int shm_index, int code_index, size_t new_size);
    
	void ResizeIpBuffer(int shm_index, size_t new_size);
    
	void CopyShareMemory(int src_shm_index, int dst_shm_index);

	bool ReadShareMemorySize(size_t* size);
	
	void WriteShareMemorySize(size_t size);
	
	void ResizeShareMemory(int shm_index, size_t new_size);
	
	//计算使用2的n次方放下size的值
    size_t CalculateNextSize(size_t size)
    {
        if ((size & (size - 1)) == 0)
        {
            return size;
        }
        size_t origin = size;
        size_t result = 1;
        while (size > 0)
        {
            size = size >> 1;
            result = result << 1;
        }
        assert(result >= origin);
        return result;
    }
  
    string GetCodeInfoById(int code_type, uint64_t code_id);

    void GetCodeInfoById(int code_type, uint64_t code_id, std::string* result);

	bool checkChange();

	bool totalDownloadRequest(); 

	bool checkCodeChangeRequest();

	void totalWriteData2Shm();

	bool binarySearch(uint32_t IP,struct VALUE* value);
	
	bool simple();

	void reportLog(const string& log);

private:
	// concurrency safty
	int m_has_initialized; ///< 0(NONE) 1(load IP) 2(Code info) 3(load IP and Code info)
    taf::TC_ThreadMutex m_has_initialized_mutex;
    taf::TC_ThreadCond m_has_initialized_cond;

    ConfigDataWrapper<ClientConfWrapper> m_client_conf; //客户端配置

    ClientReport m_report;
    int64_t	m_simple_key; //采样用的key
public:
    IPCreditAPIOptions m_init_options;
    int64_t       m_proxy_hash_key;
	//int32_t       logOpen;
	IPCreditPrx  _pIPCreditPrx;
	FIPCreditPrx  _pFIPCreditPrx;

	MemoryHead m_memory_head[2];
    IPVector m_ip_vector_impl[2];
	IPVector* m_vectorIP;
		
	enum CodeType
    {

        kCodeTypeCountry = 0,
		kCodeTypeProvince,
		kCodeTypeCity,
		kCodeTypeDistrict,
		kCodeTypeExtend,
        kCodeTypeOperator,
		kCodeTypeNettype,
        kCodeTypeApn,
        kCodeTypeCount,
    }; // enum CodeType
	
	struct MemoryHolder
    {
        ProlatePointer ip_ptr;
        ProlatePointer code_ptr[kCodeTypeCount];
    }; // struct MemoryHolder

    CodeVector m_code_vector_impl[kCodeTypeCount][2];
    CodeVector* m_code_vector[kCodeTypeCount];

    MemoryHolder m_holder[2];
	key_t m_shm_key[3];
    taf::TC_Shm m_shm[2];

	string m_serverHost;
    string m_serverPort;
	int  m_size;
	string m_recvBuffer;
	string m_sendBuffer;

    
    bool m_log_initialized;
    ostringstream _osLog;
	bool m_terminated;
	uint32_t m_protocolSeqNo;
	bool m_isCheckingCode;
	bool m_isTotalDownLoading;
	bool m_isIncrementDownloading;	
	char m_isReset;	
	unsigned long long m_currDownloadPos;
	string m_cltMd5;
	time_t m_lastCheckDistrictCodeChanged;
	time_t m_lastCheckFileChanged;
	vIPProperty m_vIPProperty;

    taf::TC_Mmap m_posFile;
	timeval m_tBegin;
	timeval m_tEnd;

    vIPProperty  m_ip_query_vec;
    typedef __gnu_cxx::hash_map<uint64_t, newIPPro::CodeInfo> CodeInfoMap;
    CodeInfoMap  m_code_query_map[kCodeTypeCount];

private:
    //析构时，是否等待线程退出，在java里调用，总是退不出
    bool m_destructor_wait;
};

#endif
