#ifndef	__TC_EPOLLER_H_
#define __TC_EPOLLER_H_

#include <sys/epoll.h>
#include <cassert>

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file  tc_epoller.h 
 * @brief  epoll������װ�� 
 *  
 * @author  jarodruan@tencent.com 
 */
/////////////////////////////////////////////////
 
/**
 * @brief epoller�����࣬�Ѿ�Ĭ�ϲ�����EPOLLET��ʽ������ 
 */
class TC_Epoller
{
public:

	/**
	 * @brief ���캯��. 
	 *  
     * @param bEt Ĭ����ETģʽ����״̬�����仯��ʱ��Ż��֪ͨ
	 */
	TC_Epoller(bool bEt = true);

	/**
     * @brief ��������.
	 */
	~TC_Epoller();

	/**
	 * @brief ����epoll���. 
	 *  
     * @param max_connections epoll������Ҫ֧�ֵ����������
	 */
	void create(int max_connections);

	/**
	 * @brief ��Ӽ������. 
	 *  
     * @param fd    ���
     * @param data  ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event ��Ҫ�������¼�EPOLLIN|EPOLLOUT
     *              
	 */
	void add(int fd, long long data, __uint32_t event);

	/**
	 * @brief �޸ľ���¼�. 
	 *  
     * @param fd    ���
     * @param data  ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event ��Ҫ�������¼�EPOLLIN|EPOLLOUT
	 */
	void mod(int fd, long long data, __uint32_t event);

	/**
	 * @brief ɾ������¼�. 
	 *  
     * @param fd    ���
     * @param data  ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event ��Ҫ�������¼�EPOLLIN|EPOLLOUT
	 */
	void del(int fd, long long data, __uint32_t event);

	/**
	 * @brief �ȴ�ʱ��. 
	 *  
	 * @param millsecond ���� 
     * @return int       ���¼������ľ����
	 */
	int wait(int millsecond);

	/**
     * @brief ��ȡ���������¼�.
	 *
	 * @return struct epoll_event&���������¼�
	 */
	struct epoll_event& get(int i) { assert(_pevs != 0); return _pevs[i]; }

protected:

	/**
	 * @brief ����epoll����EPOLL��Ϊ��Ե����EPOLLETģʽ 
     * @param fd    �������create����ʱ����ֵ
     * @param data  ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event ��Ҫ�������¼�
	 * @param op    EPOLL_CTL_ADD�� ע���µ�fd��epfd�У� 
	 * 			    EPOLL_CTL_MOD���޸��Ѿ�ע���fd�ļ����¼��� 
	 * 			    EPOLL_CTL_DEL����epfd��ɾ��һ��fd�� 
	 *  
	 */
	void ctrl(int fd, long long data, __uint32_t events, int op);

protected:

    /**
     * 	epoll
     */
    int _iEpollfd;

	/**
     * ���������
	 */
	int	_max_connections;

	/**
     * �¼���
	 */
	struct epoll_event *_pevs;

    /**
     * �Ƿ���ETģʽ
     */
    bool _et;
};

}
#endif

