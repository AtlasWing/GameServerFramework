#include "itc/client.hpp"
#include <deque>
#include <string>
#include <iostream>
#include <map>

using std::cout;
using std::cin;
using std::endl;
using std::string;

typedef std::map<uint32_t, res_prepay*>		_map_res_prepay;
_map_res_prepay wait_resprepay;

void get_balance_hdl(const uint32_t &reqid, const res_get_balance &res){
	cout << "get_balance: reqid=" << reqid << " code=" << res.code << " uin=" << res.uin 
		<< " balance=" << res.balance << endl;
}

/**查询余额*/
void get_balance(itc::client &c, const string &isasyn){
	uint32_t uin;
	cout << "uin:";
	cin >> uin;

	uint32_t area;
	cout << "area:";
	cin >> area;

	req_get_balance req;
	res_get_balance res;
	req.hdr.ip_forward = "127.0.0.1";
	req.uin = uin;
	req.hdr.area = area;
	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_get_balance(req, reqid, get_balance_hdl);
		cout << "get_balance: reqid=" << reqid << endl;
	}else{
		int ret = c.get_balance(req, res);
		if(0 != ret){
			cout << "get_balance error. ret=" << ret << endl;
			return;
		}
		if(0 != res.code){
			cout << "get_balance error. code=" << res.code << endl;
			return ;
		}
		cout << "get_balance: code=" << res.code << " uin=" << res.uin 
			<< " balance=" << res.balance << endl;
	}
}

void save_hdl(const uint32_t &reqid, const res_save &res){
	cout << "save: reqid=" << reqid << " code=" << res.code << " uin=" << res.uin << endl;
}
/**充值*/
void save(itc::client &c, const string isasyn){
	req_save req;
	res_save res;

        uint32_t area;
	cout << "area:";
	cin >> area;

	req.hdr.ip_forward = "127.0.0.2";
	req.hdr.area = area;
	cout << "uin:";
	cin >> req.uin;
	cout << "dstuin:";
	cin >> req.dstuin;
	cout << "amount:";
	cin >> req.amount;
	cout << "protal_seq:";
	cin >> req.portal_seq;
	cout << "source:";
	cin >> req.source;
	cout << "remark:";
	cin >> req.remark;

	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_save(req, reqid, save_hdl);
		cout << "save: reqid=" << reqid << endl;
	}else{
		int ret = c.save(req, res);
		if(0 != ret){
			cout << "save error. ret=" << ret << endl;
			return;
		}
		if(0 != res.code){
			cout << "save error. code=" << res.code << endl;
			return;
		}
		cout << "save: code=" << res.code << " uin=" << res.uin << endl;
	}
}

void prepay_hdl(const uint32_t &reqid, const res_prepay &res){
	cout << "prepay: reqid=" << reqid << " code=" << res.code 
		<< " uin=" << res.uin 
		<< " balance=" << res.balance
		<< " billno=" << res.billno
		<< endl;

	res_prepay *pres = new res_prepay();
	pres->code	= res.code;
	pres->uin	= res.uin;
	pres->balance	= res.balance;
	pres->billno	= res.billno;
	wait_resprepay[reqid] = pres;
}

void commit_hdl(const uint32_t &reqid, const res_commit &res){
	cout << "commit: rqid=" << reqid << " code=" << res.code << " uin=" << res.uin << endl;
}

/**预支付&提交*/
void prepay_commit(itc::client &c, const string &isasyn){
	req_prepay req_pp;
	res_prepay res_pp;

	uint32_t area;
	cout << "area:";
	cin >> area;	

	req_pp.hdr.ip_forward = "127.0.0.3";
	req_pp.hdr.area = area;
	cout << "uin:";
	cin >> req_pp.uin;
	cout << "amount:";
	cin >> req_pp.amount;
	cout << "item_it:";
	cin >> req_pp.item_id;
	cout << "timeout:";
	cin >> req_pp.timeout;
	cout << "source:";
	cin >> req_pp.source;
	cout << "transinfo:";
	cin >> req_pp.transinfo;

	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_prepay(req_pp, reqid, prepay_hdl);
		if(0 != ret){
			cout << "prepay error. ret=" << ret << endl;
			return;
		}
		res_prepay *pres = NULL;

		while(true){
			_map_res_prepay::iterator i = wait_resprepay.find(reqid);
			if(wait_resprepay.end() == i){
				itc::system::sleep(100);
				continue;
			}
			pres = i->second;
			wait_resprepay.erase(i);
			break;
		}
		
		req_commit	req_cmt;
		res_commit	res_cmt;

		req_cmt.hdr.ip_forward = "127.0.0.4";
		req_cmt.uin		= pres->uin;
		req_cmt.amount	= req_pp.amount;
		req_cmt.billno	= pres->billno;
		req_cmt.source	= req_pp.source;

		delete pres;

		ret = c.asyn_commit(req_cmt, reqid, commit_hdl);
		if(0 != ret){
			cout << "commit error. ret=" << ret << endl;
			return;
		}
		cout << "commit reqid=" << reqid << endl;

	}else{
		int ret = c.prepay(req_pp, res_pp);
		if(0 != ret){
			cout << "prepay error. ret=" << ret << endl;
			return;
		}
		if(0 != res_pp.code){
			cout << "prepay error. code=" << res_pp.code << endl;
			return;
		}
		cout << "prepay: code=" << res_pp.code 
			<< " uin=" << res_pp.uin 
			<< " balance=" << res_pp.balance
			<< " billno=" << res_pp.billno
			<< endl;

		req_commit	req_cmt;
		res_commit	res_cmt;

		req_cmt.hdr.ip_forward = "127.0.0.4";
		req_cmt.uin		= res_pp.uin;
		req_cmt.amount	= req_pp.amount;
		req_cmt.billno	= res_pp.billno;
		req_cmt.source	= req_pp.source;
		ret = c.commit(req_cmt, res_cmt);
		if(0 != ret){
			cout << "commit error. ret=" << ret << endl;
			return;
		}
		if(0 != res_cmt.code){
			cout << "commit error. code=" << res_cmt.code << endl;
			return;
		}
		cout << "commit: code=" << res_cmt.code << " uin=" << res_cmt.uin << endl;
	}
}

void rollback_hdl(const uint32_t &reqid, const res_rollback &res)
{
	cout << "rollback: reqid=" << reqid << " code=" << res.code 
		<< " uin=" << res.uin << endl;
}

/**预支付&回滚*/
void prepay_rollback(itc::client &c, const string isasyn){
	req_prepay	req_pp;
	res_prepay	res_pp;

	uint32_t area;
	cout << "area:";
	cin >> area;	

	req_pp.hdr.ip_forward = "127.0.0.5";
	req_pp.hdr.area = area;
	cout << "uin:";
	cin >> req_pp.uin;
	cout << "amount:";
	cin >> req_pp.amount;
	cout << "item_it:";
	cin >> req_pp.item_id;
	cout << "timeout:";
	cin >> req_pp.timeout;
	cout << "source:";
	cin >> req_pp.source;
	cout << "transinfo:";
	cin >> req_pp.transinfo;

	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_prepay(req_pp, reqid, prepay_hdl);
		if(0 != ret){
			cout << "prepay error. ret=" << ret << endl;
			return;
		}
		res_prepay *pres = NULL;

		while(true){
			_map_res_prepay::iterator i = wait_resprepay.find(reqid);
			if(wait_resprepay.end() == i){
				itc::system::sleep(100);
				continue;
			}
			pres = i->second;
			wait_resprepay.erase(i);
			break;
		}
		
		req_rollback req_rb;

		req_rb.hdr.ip_forward = "127.0.0.6";
		req_rb.uin	= pres->uin;
		req_rb.amount = req_pp.amount;
		req_rb.billno = pres->billno;
		req_rb.source = req_pp.source;

		delete pres;

		ret = c.asyn_rollback(req_rb, reqid, rollback_hdl);
		if(0 != ret){
			cout << "rollback error. ret=" << ret << endl;
			return;
		}
		cout << "rollback reqid=" << reqid << endl;
		
	}else{
		int ret = c.prepay(req_pp, res_pp);
		if(0 != ret){
			cout << "prepay error. ret=" << ret << endl;
			return;
		}
		if(0 != res_pp.code){
			cout << "prepay error. code=" << res_pp.code << endl;
			return;
		}
		cout << "prepay: code=" << res_pp.code 
			<< " uin=" << res_pp.uin << " balance=" << res_pp.balance
			<< " billno=" << res_pp.billno << endl;

		req_rollback req_rb;
		res_rollback res_rb;

		req_rb.hdr.ip_forward = "127.0.0.6";
		req_rb.uin	= res_pp.uin;
		req_rb.amount = req_pp.amount;
		req_rb.billno = res_pp.billno;
		req_rb.source = req_pp.source;
		ret = c.rollback(req_rb, res_rb);
		if(0 != ret){
			cout << "rollback error. ret=" << ret << endl;
			return;
		}
		if(0 != res_rb.code){
			cout << "rollback error. code=" << res_rb.code << endl;
			return;
		}
		cout << "rollback: code=" << res_rb.code 
			<< " uin=" << res_rb.uin << endl;
	}
}

void pay_hdl(const uint32_t &reqid, const res_pay &res){
	cout << "pay: reqid=" << reqid 
		<< " code=" << res.code 
		<< " balance=" << res.balance
		<< " billno=" << res.billno 
		<< endl;
}

/**扣费*/
void pay(itc::client &c, const string isasyn){
	req_pay	req;
	res_pay	res;

	uint32_t area;
	cout << "area:";
	cin >> area;	
	
	req.hdr.ip_forward="127.0.0.7";
	req.hdr.area = area;
	cout << "uin:";
	cin >> req.uin;
	cout << "amount:";
	cin >> req.amount;
	cout << "item_id:";
	cin >> req.item_id;
	cout << "source:";
	cin >> req.source;

	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_pay(req, reqid, pay_hdl);
		if(0 != ret){
			cout << "pay error. ret=" << ret << endl;
			return;
		}
		cout << "pay: reqid=" << reqid << endl;
	}else{
		int ret = c.pay(req, res);
		if(0 != ret){
			cout << "pay error. ret=" << ret << endl;
		}
		if(0 != res.code){
			cout << "pay error. code=" << res.code << endl;
			return;
		}
		cout << "pay: code=" << res.code 
			<< " balance=" << res.balance
			<< " billno=" << res.billno 
			<< endl;
	}
}

void acctinfo_hdl(const uint32_t &reqid, const res_acctinfo &res){
	cout << "acctinfo: reqid=" << reqid
		<< " code=" << (int)res.code << " uin=" << res.uin
		<< " balance=" << res.balance << " stat=" << (int)res.stat
		<< " expire_time=" << res.expire_time << endl;
}

/**查询账户信息*/
void acctinfo(itc::client &c, const string isasyn)
{
	req_acctinfo	req;
	res_acctinfo	res;
	
	uint32_t area;
	cout << "area:";
	cin >> area;	
	
	req.hdr.ip_forward="127.0.0.8";
	req.hdr.area = area;
	cout << "uin:";
	cin >> req.uin;

	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_get_acctinfo(req, reqid, acctinfo_hdl);
		if(0 != ret){
			cout << "get acctinfo error. ret=" << ret << endl;
			return;
		}
		cout << "acctinfo: reqid=" << reqid << endl;
	}else{
		int ret = c.get_acctinfo(req, res);
		if(0 != ret){
			cout << "get acctinfo error. ret=" << ret << endl;
			return;
		}
		if(0 != res.code){
			cout << "acctinfo error. code=" << res.code << endl;
			return;
		}
		cout << "acctinfo: code=" << (int)res.code << " uin=" << res.uin
			<< " balance=" << res.balance << " stat=" << (int)res.stat
			<< " expire_time=" << res.expire_time << endl;
	}
}

void transfer_hdl(const uint32_t &reqid, const res_transfer &res){
	cout << "transfer: reqid=" << reqid
		<< " code=" << (int)res.code 
		<< " uin=" << res.uin << " uin_balance=" << res.uin_balance 
		<< " dstuin=" << res.dstuin << " dstuin_balance=" << res.dstuin_balance
		<< " billno=" << res.billno << endl;
}
/**转账*/
void transfer(itc::client &c, const string isasyn){
	req_transfer req;
	res_transfer res;

	uint32_t area;
	cout << "area:";
	cin >> area;	

	req.hdr.ip_forward="127.0.0.9";
	req.hdr.area = area;
	cout << "uin:";
	cin >> req.uin;
	cout << "dstuin:";
	cin >> req.dstuin;
	cout << "amount:";
	cin >> req.amount;
	cout << "fee:";
	cin >> req.fee;
	cout << "source:";
	cin >> req.source;
	cout << "remark:";
	cin >> req.remark;
	
	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_transfer(req, reqid, transfer_hdl);
		if(0 != ret){
			cout << "transfer error. ret=" << ret << endl;
			return ;
		}
		cout << "transfer: reqid=" << reqid << endl;
	}else{
		int ret = c.transfer(req, res);
		if(0 != ret){
			cout << "transfer error. ret=" << ret << endl;
			return ;
		}
		if(0 != res.code){
			cout << "transfer error. code=" << res.code << endl;
			return;
		}
		cout << "transfer: code=" << (int)res.code 
			<< " uin=" << res.uin << " uin_balance=" << res.uin_balance 
			<< " dstuin=" << res.dstuin << " dstuin_balance=" << res.dstuin_balance
			<< " billno=" << res.billno << endl;
	}
}

void transfer_byorder_hdl(const uint32_t &reqid, const res_transfer_byorder &res){
	cout << "transfer_byorder: reqid=" << reqid
		<< " code=" << res.code
		<< " uin=" << res.uin << " balance=" << res.balance 
		<< endl;
}

/**订单转账*/
void transfer_byorder(itc::client &c, const string isasyn){
	req_prepay	req_pp;
	res_prepay	res_pp;

	uint32_t area;
	cout << "area:";
	cin >> area;	
	
	req_pp.hdr.ip_forward="127.0.0.10";
	req_pp.hdr.area = area;
	cout << "uin:";
	cin >> req_pp.uin;
	cout << "amount:";
	cin >> req_pp.amount;
	cout << "item_it:";
	cin >> req_pp.item_id;
	cout << "timeout:";
	cin >> req_pp.timeout;
	cout << "source:";
	cin >> req_pp.source;
	cout << "transinfo:";
	cin >> req_pp.transinfo;
	
	if(isasyn.compare("y") == 0){
		uint32_t reqid = 0;
		int ret = c.asyn_prepay(req_pp, reqid, prepay_hdl);
		if(0 != ret){
			cout << "prepay error. ret=" << ret << endl;
			return;
		}
		res_prepay *pres = NULL;

		while(true){
			_map_res_prepay::iterator i = wait_resprepay.find(reqid);
			if(wait_resprepay.end() == i){
				itc::system::sleep(100);
				continue;
			}
			pres = i->second;
			wait_resprepay.erase(i);
			break;
		}

		req_transfer_byorder req;

		req.hdr.ip_forward="127.0.0.11";
		cout << "transfer to uin:";
		cin >> req.uin;
		req.order_creator = req_pp.uin;
		req.amount	= req_pp.amount;
		req.billno	= pres->billno;
		req.source	= req_pp.source;

		ret = c.asyn_transfer_byorder(req, reqid, transfer_byorder_hdl);
		if(0 != ret){
			cout << "transfer_byorder error. ret=" << ret << endl;
			return;
		}
		cout << "transfer_byorder reqid = " << reqid << endl;

	}else{
		int ret = c.prepay(req_pp, res_pp);
		if(0 != ret){
			cout << "prepay error. ret=" << ret << endl;
			return;
		}
		if(0 != res_pp.code){
			cout << "prepay error. code=" << res_pp.code << endl;
			return;
		}
		cout << "prepay: code=" << (int)res_pp.code 
			<< " uin=" << res_pp.uin << " balance=" << res_pp.balance
			<< " billno=" << res_pp.billno << endl;

		req_transfer_byorder req;
		res_transfer_byorder res;

		req.hdr.ip_forward="127.0.0.11";
		cout << "transfer to uin:";
		cin >> req.uin;
		req.order_creator = req_pp.uin;
		req.amount	= req_pp.amount;
		req.billno	= res_pp.billno;
		req.source	= req_pp.source;
		ret = c.transfer_byorder(req, res);
		if(0 != ret){
			cout << "transfer_byorder error. ret=" << ret << endl;
			return;
		}
		if(0 != res.code){
			cout << "transfer_byorder error. code=" << res.code << endl;
			return;
		}
		cout << "transfer_byorder: code=" << res.code
			<< " uin=" << res.uin << " balance=" << res.balance 
			<< endl;
	}
}

int main(int argc, char **args)
{
	itc::client *c = new itc::client();

	string ip;
	unsigned short port;
	cout << "server address:";
	cin >> ip >> port;

	cout << "ip=" << ip << " port=" << port << endl;

	string userid;
	cout << "userid:";
	cin >> userid;
	cout << "userid=" << userid << endl;

	string secretkey;
	cout << "secretkey:";
	cin >> secretkey;
	cout << "secretkey=" << secretkey << endl;

	string isasyn = "n";
	cout << "use asynch api?[y/n] defaule n:";
	cin >> isasyn;
	if(isasyn.compare("y") != 0) isasyn = "n";
	cout << "isasyn=" << isasyn << endl;

	c->add_server_addr(ip, port);
	c->set_userid(userid);
	c->set_secretkey(secretkey);

	string ops = 
"help show help infomation\n\
quit	exit program\n\
save	save userticket into dstuin account\n\
get_blanace  get the account balance\n\
prepay-commit prepay and commit\n\
prepay-rollback prepay and rollback\n\
pay		reduce userticket from uin account\n\
acctinfo	get the account infomation\n\
transfer	transfer\n\
transfer_byorder	use order id to transfer\n";


	while(true){
		string cmd;
		cout << ">";
		cin >> cmd;
		if(cmd.compare("help") == 0){
			cout << ops;
		}else if(cmd.compare("quit") == 0){
			return 0;
		}else if(cmd.compare("get_balance") == 0){
			get_balance(*c, isasyn);
		}else if(cmd.compare("save") == 0){
			save(*c, isasyn);
		}else if(cmd.compare("prepay-commit") == 0){
			prepay_commit(*c, isasyn);
		}else if(cmd.compare("prepay-rollback") == 0){
			prepay_rollback(*c, isasyn);
		}else if(cmd.compare("pay") == 0){
			pay(*c, isasyn);
		}else if(cmd.compare("acctinfo") == 0){
			acctinfo(*c, isasyn);
		}else if(cmd.compare("transfer") == 0){
			transfer(*c, isasyn);
		}else if(cmd.compare("transfer_byorder") == 0){
			transfer_byorder(*c, isasyn);
		}
	}

	return 0;
}