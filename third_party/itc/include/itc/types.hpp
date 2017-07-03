#ifndef TYPES_HPP_201107181530
#define TYPES_HPP_201107181530

#include "boost/cstdint.hpp"
#include <string>

using boost::uint8_t;
using boost::int8_t;
using boost::uint32_t;
using boost::int32_t;
using std::string;


//请求头
struct req_hdr
{
	string	ip_forward;  /**<透传的ip*/
	uint32_t    area;

	req_hdr(void) : ip_forward(""), area(0){};
};

//查询账户余额
struct req_get_balance
{
	req_hdr		hdr;
	uint32_t	uin; /**<qq号*/
	
	req_get_balance(void) : uin(0){};
};
struct res_get_balance
{
	int32_t		code; /**<返回码 0正确，其它失败*/
	uint32_t	uin; /**<qq号*/
	uint32_t	balance;  /**<账户余额*/
	
	res_get_balance(void): code(0), uin(0), balance(0){};
};

//预支付
struct req_prepay{
	req_hdr		hdr;
	uint32_t	uin; /**<qq号*/
	uint32_t	amount; /**<支付金额*/
	uint32_t	item_id; /**<物品id, 如道具id*/
	uint32_t	timeout; /**<超时时间(秒), 如果用户没有在超时间内commit或rollback，超时后系统自动rollback*/
	string		source; /**<来源*/
	string		transinfo; /**<交易信息*/
	
	req_prepay(void) : uin(0), amount(0), item_id(0), timeout(0), source(""), transinfo(""){};
};
struct res_prepay{
	int32_t		code; /**<返回码 0正确，其它失败*/
	uint32_t	uin; /**<qq号*/
	uint32_t	balance; /**<账户余额*/
	string		billno; /**预扣订单号*/

	res_prepay(void) : code(0), uin(0), balance(0), billno(""){};
};


//提交
struct req_commit
{
	req_hdr		hdr;
	uint32_t	uin; /**<qq号*/
	uint32_t	amount; /**<预扣金额*/
	string		billno; /**预扣订单号*/
	string		source; /**来源*/

	req_commit(void) : uin(0), amount(0), billno(""), source(""){};
};
struct res_commit
{
	int32_t		code; /**<返回码 0正确，其它失败*/
	uint32_t	uin; /**<qq号*/

	res_commit(void) : code(0), uin(0){};
};


//回滚
typedef req_commit req_rollback;
typedef res_commit res_rollback;

//充值点券
struct req_save
{
	req_hdr		hdr;
	uint32_t	uin; /**<qq号*/
	uint32_t	dstuin; /**<目标qq号**/
	uint32_t	amount; /**<充值金额*/
	string		portal_seq; /**<portal充值流水号*/
	string		source;  /**<来源*/
	string		remark;  /**<备注*/

	req_save(void) : uin(0), dstuin(0), amount(0), portal_seq(""), source(""), remark(""){};
};
typedef res_commit res_save;


//扣除点券
struct req_pay
{
	req_hdr		hdr;
	uint32_t	uin;   /**<qq号*/
	uint32_t	amount; /**<扣除金额*/
	uint32_t	item_id; /**<物品id, 如道具id*/
	string		source; /**<来源*/

	req_pay(void) : uin(0), amount(0), item_id(0), source(""){};
};
struct res_pay
{
	int32_t		code; /**<返回码 0正确，其它失败*/
	uint32_t	balance;/**<余额*/
	string		billno; /**<订单号*/
};


//查询账户信息
typedef req_get_balance req_acctinfo;
struct res_acctinfo
{
	int32_t		code;/**<返回码 0正确，其它失败*/
	uint32_t	uin; /**<qq号*/
	uint8_t		stat; /**<账户状态 1正常 2冻结*/
	uint32_t	balance; /**<余额*/
	uint32_t	expire_time; /**<过期时间*/

	res_acctinfo(void) : code(0), uin(0), stat(0), balance(0), expire_time(0){};
};


//转账
struct req_transfer
{
	req_hdr		hdr;
	uint32_t	uin; /**<转出qq号*/
	uint32_t	dstuin; /**<转入qq号**/
	uint32_t	amount; /**<转账金额*/
	uint32_t	fee;   /**<手续费*/
	string		source; /**<来源信息*/
	string		remark; /**<备注信息*/

	req_transfer(void) : uin(0), dstuin(0), amount(0), fee(0), source(""), remark(""){};
};
struct res_transfer
{
	int32_t		code;/**<返回码 0正确，其它失败*/
	uint32_t	uin; /**<转出qq号*/
	uint32_t	dstuin; /**<转入qq号**/
	uint32_t	uin_balance; /**<转出帐户余额*/
	uint32_t	dstuin_balance; /**<转入帐户余额*/
	string		billno; /**<订单号*/

	res_transfer(void) : code(0), uin(0), dstuin(0), uin_balance(0), dstuin_balance(0){};
};


//订单转账
struct req_transfer_byorder
{
	req_hdr			hdr;
	uint32_t		uin;	/**<<qq号*/
	uint32_t		order_creator; /**<<支付订单发起方QQ号，即先前的预扣发起方号码*/
	uint32_t		amount; /**<<寄售金额*/
	string			billno; /**<<寄售订单号*/
	string			source; /**来源*/
	
	req_transfer_byorder(void) : uin(0), order_creator(0), amount(0), billno(""), source(""){};
};
struct res_transfer_byorder
{
	int32_t		code; /**<返回码 0正确，其它失败*/
	uint32_t	uin; /**<qq号*/
	uint32_t	balance; /**<账户余额*/

	res_transfer_byorder(void) : code(0), uin(0), balance(0){};
};

#endif



