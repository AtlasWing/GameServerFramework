// ������������,��Ҫ�ϸ����ع���,��Ҫ���κ���������

/// @brief ���߻�������
#ifdef ITEM_BASE_ATTRIBUTE

	ITEM_BASE_ATTRIBUTE( WORD,	Num,	    num,	    0,      "����" )

#endif

/// @brief ������������
#ifdef ITEM_RARE_ATTRIBUTE

	ITEM_RARE_ATTRIBUTE( bool,	Bind,	    bind,	    false,  "��" )
	ITEM_RARE_ATTRIBUTE( DWORD,	Quality,	quality,    0,	"Ʒ��" )
	ITEM_RARE_ATTRIBUTE( DWORD,	Grade,	    grade,      0,	"Ʒ��" )
	ITEM_RARE_ATTRIBUTE( DWORD,	Identify,	identify,   0,	"����״̬" )
	ITEM_RARE_ATTRIBUTE( bool,	Received,	received,   false,"�Ƿ�ջ��" )
#endif

#ifdef ITEM_LOG_ATTRIBUTE
    ITEM_LOG_ATTRIBUTE(DWORD, useiteminfo, usenum, 0, "��Ʒʹ������")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, shopid, 0, "�̵�id")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, shoppage1, 0, "�̵�ҳǩһ")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, shoppage2, 0,"�̵�ҳǩ��")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, pos, 0,"λ��")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, moneytype, 0,"��Ǯ����")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, moneyvalue, 0,"��Ǯ����")
    ITEM_LOG_ATTRIBUTE(DWORD, buytypeinfo, num, 0,"��������")
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, spacketid, 0,"�ƶ�Դ����")
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, spos, 0,"Դλ��")
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, dpacketid, 0, "Ŀ�����") 
    ITEM_LOG_ATTRIBUTE(DWORD, movetypeinfo, dpos, 0, "Ŀ��λ��")
#endif
