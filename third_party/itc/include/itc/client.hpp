#ifndef CLIENT_HPP_201107191559
#define CLIENT_HPP_201107191559

#include "types.hpp"
#include <string>

#ifdef ITC_CLIENT_DLL

#ifdef CLIENT_EXPORTS
#define CLIENT_EXPORT __declspec(dllexport)
#else
#define CLIENT_EXPORT __declspec(dllimport)
#endif

#else
#define	CLIENT_EXPORT
#endif


namespace itc
{

	namespace system{
		CLIENT_EXPORT void sleep(const uint32_t mills);
	}

	class client_impl;

	/**
	req 请求数据
	res 返回数据
	timeout 超时时间(毫秒)
	返回：0 正确，<0错误
	*/
	class CLIENT_EXPORT client
	{
	public:
		enum{
			DEF_TIMEOUT	= 3000 /**<默认超时时间(毫秒)*/
		};

	public:
		client(void);
		~client(void);

		void add_server_addr(const std::string &ip, const unsigned short port);
		void set_userid(const std::string userid);
		void set_secretkey(const std::string &key);

		/**查询余额*/
		int get_balance(const req_get_balance &req, res_get_balance &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_get_balance(const req_get_balance &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_get_balance&));

		/**预支付*/
		int prepay(const req_prepay &req, res_prepay &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_prepay(const req_prepay &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_prepay&));

		/**提交/预支付确认*/
		int commit(const req_commit &req, res_commit &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_commit(const req_commit &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_commit&));

		/**回滚/预支付取消*/
		int rollback(const req_rollback &req, res_rollback &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_rollback(const req_rollback &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_rollback&));
		
		/**充值*/
		int save(const req_save &req, res_save &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_save(const req_save &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_save&));

		/**支付/扣费*/
		int pay(const req_pay &req, res_pay &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_pay(const req_pay &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_pay&));

		/**查询账户信息*/
		int get_acctinfo(const req_acctinfo &req, res_acctinfo &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_get_acctinfo(const req_acctinfo &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_acctinfo&));

		/**转账*/
		int transfer(const req_transfer &req, res_transfer &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_transfer(const req_transfer &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_transfer&));

		/**订单转账*/
		int transfer_byorder(const req_transfer_byorder &req, res_transfer_byorder &res, const unsigned int timeout = DEF_TIMEOUT);
		int asyn_transfer_byorder(const req_transfer_byorder &req, uint32_t &reqid, void (*hdl)(const uint32_t&, const res_transfer_byorder&));

		/**得到错误信息*/
		std::string get_error_message(const int errcode) const;

	private:
		client_impl	*impl_;
	};
}

#endif



