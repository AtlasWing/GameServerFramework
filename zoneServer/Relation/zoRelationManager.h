#ifndef _RELATION_MANAGER_H_
#define _RELATION_MANAGER_H_

#include "base/nManager.h"
#include "base/nMisc.h"

template<typename Relation>
class zoRelationManager : 
	public Tidy::nManager<false, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<Relation> >, Tidy::nIndexByName< Tidy::CommonGetNamePolicy<Relation> > >
{
	public:
		//bool sendCmdToAll(const command::NullUserCommand *cmd, const DWORD len) const;
		//bool sendCmdToAllExceptMe(Relation *relation,const command::NullUserCommand *cmd, const DWORD len) const;

		//bool sendSysMessageToAll(const char *info,...);
		//bool sendSysMessageExceptMe(Relation *relation, const char *info,...);

		template<typename ExecT>
			bool execEveryRelation(ExecT &exec)
			{
				return execAll(exec);
			}
		template<typename ExecT>
			bool execEveryRelation(ExecT &exec)const
			{
				return execAll(exec);
			}
};

///========================template function======================

/*
namespace
{
	template<typename Relation>
		struct AllSendCmd
		{
			const command::NullUserCommand *_cmd;
			const DWORD _len;
			AllSendCmd(const command::NullUserCommand *cmd, const DWORD len):
				_cmd(cmd),_len(len)
			{}
			bool exec(Relation *entity)
			{
				return entity->sendCmdToMe(_cmd, _len);
			}
		};
}
template<typename Relation>
inline bool zoRelationManager<Relation>::sendCmdToAll(const command::NullUserCommand *cmd, const DWORD len)const
{
	AllSendCmd<Relation> sendExec(cmd,len);
	return execEveryRelation(sendExec);
}

namespace
{
	template<typename Relation>
		struct ExceptSendCmd
		{
			Relation *_except;
			const command::NullUserCommand *_cmd;
			const DWORD _len;
			ExceptSendCmd(Relation *except,const command::NullUserCommand *cmd, const DWORD len):
				_except(except), _cmd(cmd), _len(len)
			{}
			bool exec(Relation *entity)
			{
				if(entity == _except)
					return true;

				return entity->sendCmdToMe(_cmd, _len);
			}
		};
}
template<typename Relation>
inline bool zoRelationManager<Relation>::sendCmdToAllExceptMe(Relation *relation,const command::NullUserCommand *cmd, const DWORD len) const
{
	ExceptSendCmd<Relation> sendExec(relation,cmd,len);
	return execEveryRelation(sendExec);
}

namespace
{
	template<typename Relation>
	struct ExceptSendSysMessageExec
	{
		const char* _message;
		Relation *_except;
		ExceptSendSysMessageExec(Relation *except,char* message):
            _message(message),
			_except(except)
		{}
		bool exec(Relation* entity)
		{
			if(_except == entity)
				return true;

			return entity->sendSysMessage(_message);
		}
	};
};
	template<typename Relation>
inline bool zoRelationManager<Relation>::sendSysMessageExceptMe(Relation *relation, const char *info,...)
{
	char buff[command::chat::MAX_SYS_CHATINFO];
	FMTString(buff, command::chat::MAX_SYS_CHATINFO, info);

	ExceptSendSysMessageExec<Relation> sendExec(relation,buff);
	return execEveryRelation(sendExec);
}


namespace
{
	template<typename Relation>
	struct AllSendSysMessageExec
	{
		const char* _message;
		AllSendSysMessageExec(char* message): _message(message)
		{}
		bool exec(Relation* entity)
		{
			return entity->sendSysMessage(_message);
		}
	};
}
	template<typename Relation>
inline bool zoRelationManager<Relation>::sendSysMessageToAll(const char *info,...)
{
	char buff[command::chat::MAX_SYS_CHATINFO];
	FMTString(buff, command::chat::MAX_SYS_CHATINFO, info);

	AllSendSysMessageExec<Relation> sendExec(buff);
	return execEveryRelation(sendExec);
}
*/

#endif

