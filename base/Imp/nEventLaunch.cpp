#include "base/nEventLaunch.h"
#include "base/nEventCheck.h"
#include "base/nEventObserver.h"

namespace Tidy
{
	EventRegValue::EventRegValue(nEventObserver *observerObj, nEventCheck *eventCheckObj):
		observer(observerObj),
		eventCheck(eventCheckObj),
		isValid(true)
	{
	}

	bool EventRegItem::reg(nEventObserver *observer, nEventCheck *eventCheck)
	{
		for(Tidy::list<EventRegValue>::iterator iter = eventRegValueList.begin(); iter != eventRegValueList.end(); ++ iter){
			EventRegValue &regValue = *iter;
			if(regValue.observer == observer && regValue.eventCheck == eventCheck){
				regValue.isValid = true;
				return true;
			}
		}
		eventRegValueList.push_back(EventRegValue(observer, eventCheck));
		return true;
	}

	bool EventRegItem::unreg(const nEventObserver *observer, const nEventCheck *eventCheck)
	{
		for(Tidy::list<EventRegValue>::iterator iter = eventRegValueList.begin(); iter != eventRegValueList.end(); ++ iter){
			EventRegValue &regValue = *iter;
			if(regValue.observer == observer && regValue.eventCheck == eventCheck){
				if(regValue.isValid == false){
					return false;
				}
				regValue.isValid = false;
				return true;
			}
		}
		return false;
	}

	nEventLaunch::nEventLaunch() : _erased_num(0)
	{
	}

	nEventLaunch::~nEventLaunch()
	{
		for(EventRegContainer::iterator conIter= _event_container.begin(); conIter!= _event_container.end(); ++conIter) {
			EventRegItem &eventItem = conIter->second;	
			for(EventRegItem::iterator iter = eventItem.eventRegValueList.begin(); iter != eventItem.eventRegValueList.end(); ++ iter) {
				EventRegValue &eventRegValue = *iter;
				if(eventRegValue.isValid == true) {
					eventRegValue.observer->removeEvent(conIter->first, this, eventRegValue.eventCheck);
				}
			}
		}
		_event_container.clear();
	}

	bool nEventLaunch::regEvent(DWORD eventID, nEventObserver *observer, const nEventCheck *eventCheck)
	{
		if(observer->addEvent(eventID, this, const_cast<nEventCheck*>(eventCheck))) {
			EventRegItem *eventRegItem = _event_container.fetch(eventID);
			return eventRegItem->reg(observer, const_cast<nEventCheck*>(eventCheck));
		}

		assert_fail("注册事件失败");
		return false;
	}

	bool nEventLaunch::fireEvent(DWORD eventID, const EventParam* eventParam, const bool initResult) const
	{
		bool result = initResult;
		EventRegContainer::const_iterator conIter= _event_container.find(eventID);
		if(conIter != _event_container.end()) {
			const EventRegItem &eventItem = conIter->second;	
			if(eventItem.eventRegValueList.empty() == true){
				return result;
			}
			EventRegItem::const_iterator endIter = eventItem.eventRegValueList.end();
			for(EventRegItem::const_iterator iter = eventItem.eventRegValueList.begin(); iter != endIter; ++ iter) {
				EventRegValue &eventRegValue = *const_cast<EventRegValue*>(&(*iter));
				if(eventRegValue.isValid == true) {
					if(eventRegValue.eventCheck->notifyEvent(const_cast<nEventLaunch*>(this), eventRegValue.observer, eventID, eventParam) != initResult){
						result = !initResult;
					}
				}
			}
		}

		return result;
	}

	bool nEventLaunch::fireEvent(nEventLaunch *luanch, DWORD eventID, const EventParam *eventParam, const bool initResult) const
	{
		bool result = initResult;
		EventRegContainer::const_iterator conIter= _event_container.find(eventID);
		if(conIter != _event_container.end()) {
			const EventRegItem &eventItem = conIter->second;	
			if(eventItem.eventRegValueList.empty() == true){
				return result;
			}
			EventRegItem::const_iterator endIter = eventItem.eventRegValueList.end();
			for(EventRegItem::const_iterator iter = eventItem.eventRegValueList.begin(); iter != endIter; ++iter) {
				EventRegValue &eventRegValue = *const_cast<EventRegValue*>(&(*iter));
				if(eventRegValue.isValid == true) {
					if(eventRegValue.eventCheck->notifyEvent(luanch, eventRegValue.observer, eventID, eventParam) != initResult){
						result = !initResult;
					}
				}
			}
		}

		return result;
	}

	bool nEventLaunch::unregEvent(DWORD eventID, nEventObserver* observer, const nEventCheck *eventCheck)
	{
		if(observer->removeEvent(eventID, this, (nEventCheck*)eventCheck)) {
			EventRegContainer::iterator conIter= _event_container.find(eventID);
			if(conIter != _event_container.end()) {
				EventRegItem &eventItem = conIter->second;	
				if(eventItem.unreg(observer, eventCheck) == true) {
					++ _erased_num; 
				}
				return true;
			}
		}
		return false;
	}

	DWORD nEventLaunch::getEventCheckListByObserver(const nEventObserver* observer, EventCheckList &eventCheckList) const
	{
		DWORD num = 0;
		for(EventRegContainer::const_iterator conIter = _event_container.begin(); conIter != _event_container.end(); ++conIter) {
			const EventRegItem &eventItem = conIter->second;	
			for(EventRegItem::const_iterator iter = eventItem.eventRegValueList.begin(); iter != eventItem.eventRegValueList.end(); ++iter) {
				const EventRegValue &eventRegValue = *iter;
				if(eventRegValue.isValid == true) {
					if(eventRegValue.observer == observer) {
						eventCheckList.push_back(EventCheckListValue(conIter->first, const_cast<nEventCheck*>(eventRegValue.eventCheck)));
						++ num;
					}
				}
			}
		}

		return num;
	}

	DWORD nEventLaunch::clearErasedEventRegItem()
	{
		if(_erased_num < ERASED_EVENT_ITEM_MAX) {
			return 0;
		}

#if 0
		baseServer->debug("_erased_num=[%u]", _erased_num);
#endif

		EventRegItem::iterator tmpIter;
		for(EventRegContainer::iterator conIter=_event_container.begin(); conIter != _event_container.end(); ++conIter) {
			EventRegItem &eventItem = conIter->second;
			for(EventRegItem::iterator iter = eventItem.eventRegValueList.begin(); iter != eventItem.eventRegValueList.end();) {
				tmpIter = iter;
				++ iter;
				if((*tmpIter).isValid == false) {
					eventItem.eventRegValueList.erase(tmpIter);
				}
			}
		}

		DWORD tmpNum = _erased_num;
		_erased_num = 0;
		return tmpNum;
	}

	bool nEventLaunch::deleteEvent(const DWORD &eventID, nEventObserver* observer, nEventCheck *eventCheck)
	{
		EventRegContainer::iterator conIter = _event_container.find(eventID);
		if(conIter != _event_container.end()) {
			EventRegItem &eventItem = conIter->second;
			if(eventItem.unreg(observer, eventCheck) == true) {
				++ _erased_num;
			}
			return true;
		}
		return false;
	}

	const bool nEventLaunch::hasEventReged(const DWORD eventID) const
	{
		EventRegContainer::const_iterator eventRegIter = _event_container.find(eventID);
		if(eventRegIter == _event_container.end()) {
			return false;
		}
		const EventRegItem &eventRegItem = eventRegIter->second;
		const EventRegItem::const_iterator endIter = eventRegItem.eventRegValueList.end();
		for(EventRegItem::const_iterator iter = eventRegItem.eventRegValueList.begin(); iter != endIter; ++iter) {
			if((*iter).isValid == true) {
				return true;
			}
		}

		return false;
	}
};
