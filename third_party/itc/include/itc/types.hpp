#ifndef TYPES_HPP_201107181530
#define TYPES_HPP_201107181530

#include "boost/cstdint.hpp"
#include <string>

using boost::uint8_t;
using boost::int8_t;
using boost::uint32_t;
using boost::int32_t;
using std::string;


//����ͷ
struct req_hdr
{
	string	ip_forward;  /**<͸����ip*/
	uint32_t    area;

	req_hdr(void) : ip_forward(""), area(0){};
};

//��ѯ�˻����
struct req_get_balance
{
	req_hdr		hdr;
	uint32_t	uin; /**<qq��*/
	
	req_get_balance(void) : uin(0){};
};
struct res_get_balance
{
	int32_t		code; /**<������ 0��ȷ������ʧ��*/
	uint32_t	uin; /**<qq��*/
	uint32_t	balance;  /**<�˻����*/
	
	res_get_balance(void): code(0), uin(0), balance(0){};
};

//Ԥ֧��
struct req_prepay{
	req_hdr		hdr;
	uint32_t	uin; /**<qq��*/
	uint32_t	amount; /**<֧�����*/
	uint32_t	item_id; /**<��Ʒid, �����id*/
	uint32_t	timeout; /**<��ʱʱ��(��), ����û�û���ڳ�ʱ����commit��rollback����ʱ��ϵͳ�Զ�rollback*/
	string		source; /**<��Դ*/
	string		transinfo; /**<������Ϣ*/
	
	req_prepay(void) : uin(0), amount(0), item_id(0), timeout(0), source(""), transinfo(""){};
};
struct res_prepay{
	int32_t		code; /**<������ 0��ȷ������ʧ��*/
	uint32_t	uin; /**<qq��*/
	uint32_t	balance; /**<�˻����*/
	string		billno; /**Ԥ�۶�����*/

	res_prepay(void) : code(0), uin(0), balance(0), billno(""){};
};


//�ύ
struct req_commit
{
	req_hdr		hdr;
	uint32_t	uin; /**<qq��*/
	uint32_t	amount; /**<Ԥ�۽��*/
	string		billno; /**Ԥ�۶�����*/
	string		source; /**��Դ*/

	req_commit(void) : uin(0), amount(0), billno(""), source(""){};
};
struct res_commit
{
	int32_t		code; /**<������ 0��ȷ������ʧ��*/
	uint32_t	uin; /**<qq��*/

	res_commit(void) : code(0), uin(0){};
};


//�ع�
typedef req_commit req_rollback;
typedef res_commit res_rollback;

//��ֵ��ȯ
struct req_save
{
	req_hdr		hdr;
	uint32_t	uin; /**<qq��*/
	uint32_t	dstuin; /**<Ŀ��qq��**/
	uint32_t	amount; /**<��ֵ���*/
	string		portal_seq; /**<portal��ֵ��ˮ��*/
	string		source;  /**<��Դ*/
	string		remark;  /**<��ע*/

	req_save(void) : uin(0), dstuin(0), amount(0), portal_seq(""), source(""), remark(""){};
};
typedef res_commit res_save;


//�۳���ȯ
struct req_pay
{
	req_hdr		hdr;
	uint32_t	uin;   /**<qq��*/
	uint32_t	amount; /**<�۳����*/
	uint32_t	item_id; /**<��Ʒid, �����id*/
	string		source; /**<��Դ*/

	req_pay(void) : uin(0), amount(0), item_id(0), source(""){};
};
struct res_pay
{
	int32_t		code; /**<������ 0��ȷ������ʧ��*/
	uint32_t	balance;/**<���*/
	string		billno; /**<������*/
};


//��ѯ�˻���Ϣ
typedef req_get_balance req_acctinfo;
struct res_acctinfo
{
	int32_t		code;/**<������ 0��ȷ������ʧ��*/
	uint32_t	uin; /**<qq��*/
	uint8_t		stat; /**<�˻�״̬ 1���� 2����*/
	uint32_t	balance; /**<���*/
	uint32_t	expire_time; /**<����ʱ��*/

	res_acctinfo(void) : code(0), uin(0), stat(0), balance(0), expire_time(0){};
};


//ת��
struct req_transfer
{
	req_hdr		hdr;
	uint32_t	uin; /**<ת��qq��*/
	uint32_t	dstuin; /**<ת��qq��**/
	uint32_t	amount; /**<ת�˽��*/
	uint32_t	fee;   /**<������*/
	string		source; /**<��Դ��Ϣ*/
	string		remark; /**<��ע��Ϣ*/

	req_transfer(void) : uin(0), dstuin(0), amount(0), fee(0), source(""), remark(""){};
};
struct res_transfer
{
	int32_t		code;/**<������ 0��ȷ������ʧ��*/
	uint32_t	uin; /**<ת��qq��*/
	uint32_t	dstuin; /**<ת��qq��**/
	uint32_t	uin_balance; /**<ת���ʻ����*/
	uint32_t	dstuin_balance; /**<ת���ʻ����*/
	string		billno; /**<������*/

	res_transfer(void) : code(0), uin(0), dstuin(0), uin_balance(0), dstuin_balance(0){};
};


//����ת��
struct req_transfer_byorder
{
	req_hdr			hdr;
	uint32_t		uin;	/**<<qq��*/
	uint32_t		order_creator; /**<<֧����������QQ�ţ�����ǰ��Ԥ�۷��𷽺���*/
	uint32_t		amount; /**<<���۽��*/
	string			billno; /**<<���۶�����*/
	string			source; /**��Դ*/
	
	req_transfer_byorder(void) : uin(0), order_creator(0), amount(0), billno(""), source(""){};
};
struct res_transfer_byorder
{
	int32_t		code; /**<������ 0��ȷ������ʧ��*/
	uint32_t	uin; /**<qq��*/
	uint32_t	balance; /**<�˻����*/

	res_transfer_byorder(void) : code(0), uin(0), balance(0){};
};

#endif



