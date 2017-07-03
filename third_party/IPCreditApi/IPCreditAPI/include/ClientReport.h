#ifndef __CLIENT_REPORT_H_
#define __CLIENT_REPORT_H_

#include "util/tc_thread.h"
#include "IPCredit.h"
#include "util/tc_monitor.h"

using namespace newIPPro;
/**
 * 统计和上报客户端的调用信息
 */

class ClientReport : public TC_Thread
{
public:
	ClientReport():
		m_terminated(false),
		_tTime(0),
		_iReportInterval(60000),
		m_destructor_wait(true) {
	}

	virtual ~ClientReport();

public:
	void setIPCreditPrx(const IPCreditPrx& prx) {
		_pIPCreditPrx = prx;
	}

	void report(ReportType eType, const string& name);

	//上报采样日志
	void reportSimpleLog(const string& log_content, int limit_count);

    virtual void run();

    void terminate();

    void set_destructor_wait(bool destructor_wait) {
    	m_destructor_wait = destructor_wait;
    }

private:
	IPCreditPrx  _pIPCreditPrx;
	bool m_terminated;
    time_t  _tTime;
    int    _iReportInterval;
    TC_ThreadLock _reportLock;

    map<ReportInfo, taf::Int64> _local_data;
    vector<string> _local_logs;

    bool m_destructor_wait;
};
#endif
