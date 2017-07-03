#ifndef BASE_NCMDQUEUE_H_
#define BASE_NCMDQUEUE_H_

#include "base/nMisc.h"

namespace Tidy
{
	struct nCmdData{
		DWORD len;
		char  data[0];
	};
#define QueueSize	102400
	class nCmdQueue
	{
		public:
			nCmdQueue(): _queue_write(0), _queue_read(0)
		{
			memset(_cmd_queue_data_buffer, 0, sizeof(_cmd_queue_data_buffer));
			_cmd_queue = reinterpret_cast<nCmdData**>(_cmd_queue_data_buffer);
		}
			~nCmdQueue()
			{
				clear();
			}
			const nCmdData *getCmd()
			{
				const nCmdData *ret = NULL;
				if(_cmd_queue[_queue_read] != NULL){
					ret = _cmd_queue[_queue_read];
					// 之所以在这里++ _queue_read,是为了避免在消息处理宕掉后,新起的tick线程继续处理老消息,继续宕机.
					++ _queue_read;
					_queue_read = _queue_read % QueueSize;
				}
				return ret;
			}
			void popCmd()
			{
				DWORD queueRead = (_queue_read +  QueueSize - 1)%QueueSize;
				::free(_cmd_queue[queueRead]);
				_cmd_queue[queueRead] = NULL;
			}
			bool pushCmd(const void *command, const DWORD len)
			{
				nCmdData *cmdData = reinterpret_cast<nCmdData*>(::malloc(sizeof(nCmdData) + len));
				cmdData->len = len;
				memcpy(cmdData->data, command, len);
				if(!putQueueToArray() && _cmd_queue[_queue_write] == NULL){
					_cmd_queue[_queue_write] = cmdData;
					_queue_write = (++_queue_write)%QueueSize;
					return true;
				}
				else{
					_queue_cmd.push_back(cmdData);
				}
				return true;
			}

		private:
			void clear()
			{
				while(putQueueToArray()) {
					while(getCmd()) {
						popCmd();
					}
				}
				while(getCmd()) {
					popCmd();
				}
			}
			bool putQueueToArray()
			{
				while(!_queue_cmd.empty()){
					if(_cmd_queue[_queue_write] == NULL){
						_cmd_queue[_queue_write] = _queue_cmd.front();;
						_queue_write = (++_queue_write)%QueueSize;
						_queue_cmd.pop_front();
					}
					else{
						return true;
					}
				}

				return false;
			}

		private:
			char _cmd_queue_data_buffer[sizeof(nCmdData*) * QueueSize];
			nCmdData **_cmd_queue;
			std::deque<nCmdData*> _queue_cmd;
			DWORD _queue_write;
			DWORD _queue_read;
	};
};

#endif
