#include "base/nEventObserver.h"
#include "base/nEventCheck.h"
#include "base/nBaseServer.h"

namespace Tidy
{
	void nEventObserver::printAllEventCheck(const char *ownerLog) const
	{
		for(RegEventCheckContainer::const_iterator iter = _reg_event_check_con.begin(); iter != _reg_event_check_con.end(); ++iter) {
			if(ownerLog != NULL) {
				baseServer->debug("[%s %s %lu]", ownerLog, iter->first->name, iter->second.size());
			}
			else {
				baseServer->debug("[%s %lu]", iter->first->name, iter->second.size());
			}
		}
	}

	bool nEventObserver::addEvent(DWORD &eventID, nEventLaunch *launch, nEventCheck *eventCheck)
	{
		EventCheckRegItem *eventCheckRegItem = _reg_event_check_con.fetch(eventCheck);
		eventCheckRegItem->insert(EventCheckRegValueType(eventID, launch));
		return true;
	}

	bool nEventObserver::removeEvent(const DWORD &eventID, nEventLaunch *launch, nEventCheck *eventCheck)
	{
		if(_reg_event_check_con[eventCheck].erase(EventCheckRegValueType(eventID,launch)) > 0) {
			eventCheck->dealRemoveEvent(eventID, launch);
			return true;
		}
		return false;
	}

	bool nEventObserver::unregEventByEventCheck(nEventCheck *eventCheck)
	{
		RegEventCheckContainer::iterator conIter = _reg_event_check_con.find(eventCheck);
		if(conIter != _reg_event_check_con.end()) {
			EventCheckRegItem &regItem = conIter->second;
			for(EventCheckRegItem::iterator iter = regItem.begin(); iter != regItem.end();) {
				const EventCheckRegValueType &value = *iter;
				++ iter;
				value.second->deleteEvent(value.first, (nEventObserver*)this, eventCheck);
				eventCheck->dealRemoveEvent(value.first, value.second);
			}

			//regItem已经为空,但是必须删除才能清除这一项
			_reg_event_check_con.erase(conIter);
			return true;
		}
		return false;
	}

	void nEventObserver::unregAllEvent()
	{
		for(RegEventCheckContainer::iterator conIter = _reg_event_check_con.begin(); conIter != _reg_event_check_con.end(); ++conIter) {
			EventCheckRegItem &regItem = conIter->second;
			for(EventCheckRegItem::iterator iter = regItem.begin(); iter != regItem.end(); ++ iter) {
				iter->second->deleteEvent(iter->first, (nEventObserver*)this, conIter->first);
				conIter->first->dealRemoveEvent(iter->first, iter->second);
			}
		}

		_reg_event_check_con.clear();
	}
};
