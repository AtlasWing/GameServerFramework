#ifndef GAME_NSTATICINDEX_H_
#define GAME_NSTATICINDEX_H_

#include "protocol/forwardCommand.h"
#include "protocol/roomCommand.h"

#include "base/nHashListTable.h"
#include "base/nManager.h"

#include "game/nAlgo.h"
#include "game/nBaseRoom.h"

#include "platform/platform_shared/frame/Event.h"

namespace protocol{
	namespace base{
		struct NullProtoCommand;
	};
};

namespace Game
{
	template <typename MapT, typename GatewayConnT, typename RoomObjT, typename UserT>
		class nStaticIndex : public nBaseRoom<MapT>
        {
		public:
			typedef MapT Map;
			typedef RoomObjT RoomObj;
			typedef UserT User;

		protected:
			nStaticIndex(const MapT* map, const DWORD roomID):
                nBaseRoom<MapT>(map, roomID)
            {
            }
			~nStaticIndex()
			{
#ifdef _DEBUG
				for(DWORD i = 0; i < eRoomEntityType_Max; ++ i){
					assert_debug(_all_obj_manager[i].empty() == true, "");
				}
#endif
			}

		public:
			bool addObjToIndex( RoomObjT *obj, const nPos2D &newPos );
			bool removeObjFromIndex( RoomObjT *obj );
			bool freshObjToIndex( RoomObjT *obj, const nPos2D &newPos, const BYTE freshPosType);

		public:
			bool sendUserCmdToAround(const nPos2D& pos, IEvent* event) const;
			bool sendUserCmdToAroundExceptOne(const DWORD exceptCharID, const nPos2D& pos, IEvent* event) const;
			bool sendCmdToGatewayInRoom(protocol::NullProtoCommand* cmd, const DWORD len) const;

		public:
			RoomObjT* getRoomObjByTypeAndID(const BYTE type, const DWORD id) const;
			DWORD getRoomObjNumByType(const BYTE type) const;
			User* getUserByID(const DWORD charID) const; 

		public:
			template<typename ExecT>
				bool execAllUser(ExecT &exe);
			template<typename ExecT>
				bool execAllObjByPos(const nPos2D& pos, const float distance, const BYTE direct, const WORD angle, ExecT &cb );
			template<typename ExecT>
				bool execAllObjByPos(const BYTE roomEntityType, const nPos2D& pos, const float distance, const BYTE direct, const WORD angle, ExecT &cb );
			template<typename ExecT>
				bool execAllObjByPos(const nPos2D& pos, const float distance, ExecT &cb );
			template<typename ExecT>
				bool execAllObjByPos(const BYTE roomEntityType, const nPos2D& pos, const float distance, ExecT &cb );
			template<typename ExecT>
				bool execAllOfRoom( const BYTE type, ExecT &cb );
			template<typename ExecT>
				bool execAllOfRoom( const BYTE type, const nPos2D &pos, const float range, ExecT &cb );
			template<typename ExecT>
				bool execAllOfRoom( const nPos2D &pos, const float range, ExecT &cb );
			template<typename ExecT>
				bool execAllOfRoom( ExecT &cb );
			template<typename ExecT>
				bool execAllAroundObj(const nPos2D &pos, ExecT &exec);
			template<typename ExecT>
				bool execAllAroundObjInRange(const nPos2D &pos, const float range, ExecT &exec);
			template<typename ExecT>
				bool execAllAroundObjByType(const eRoomEntityType type, const nPos2D& pos, ExecT &exec);
			template<typename ExecT>
				bool execAllAroundObjByTypeInRange(const eRoomEntityType type, const nPos2D& pos, const float range, ExecT &exec);

		public:
			bool checkInAround(const nPos2D& pos, const nPos2D& otherPos);

		public:
			//网关关闭要删除,否则野指针了
			void removeGatewayConn( GatewayConnT *conn );

		private:
			bool addGatewayConn( GatewayConnT *conn );
			bool subGatewayConn( GatewayConnT *conn );
			void freshEffectPosI( const nPosI oldPosI, const nPosI newPosI );

		protected:
			typedef Tidy::map< GatewayConnT*, DWORD > GatewayContainerType;
			typedef typename GatewayContainerType::const_iterator GatewayContainer_constIter;
			GatewayContainerType _gateway_conn_container;

		protected:
			typedef Tidy::nManager<false, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<RoomObjT>, Tidy::nHashListTable<DWORD, RoomObjT*, Tidy::CommonGetIDPolicy<RoomObjT> > > > RoomObjManager;
			RoomObjManager _all_obj_manager[eRoomEntityType_Max];
	};

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::freshObjToIndex( RoomObjT *obj, const nPos2D &newPos, const BYTE freshPosType)
		{
			if(obj->hasRoom() == false){
				assert_fail("不在场景索引");
				return false;
			}

			if(this->checkPosValid(newPos) == false){
				assert_fail("位置不对[%s][%f,%f]", obj->getName(), newPos.x, newPos.z);
				return false;
			}

			nPos2D oldPos = obj->getPos();
			if(obj->setPos(newPos) == false){
				assert_fail("设置位置失败[%f,%f]", newPos.x, newPos.z);
				return false;
			}

			//必须把changePos放在changePosI之后，客户端显示移动才正常
			obj->onChangePos(oldPos, freshPosType);
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::addObjToIndex(RoomObjT *obj, const nPos2D &newPos )
		{
			assert_debug(obj != NULL, "");

			if(this->checkPosValid(newPos) == false){
				assert_fail("位置不对");
				return false;
			}

			BYTE type = obj->getType();
			if(obj->getType() != eRoomEntityType_Player && obj->getType() != eRoomEntityType_Pet && obj->hasRoom() == true){
				assert_fail("已经加入场景索引");
				return false;
			}

			if(obj->setPos(newPos) == false) {
				assert_fail("设置位置失败");
				return false;
			}

			if(obj->getType() == eRoomEntityType_Player) {
				UserT* user = static_cast<UserT*>(obj);
				addGatewayConn(user->getGatewayConn());
			}

			if(_all_obj_manager[type].add(obj) == false){
				assert_fail("添加物品失败");
				return false;
			}

			obj->onAddToRoomIndex();

			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::removeObjFromIndex( RoomObjT *obj )
		{
			assert_debug(obj != NULL, "");
			if(obj->hasRoom() == false){
				assert_fail("obj不在场景索引");
				return false;
			}

			BYTE type = obj->getType();

			//在全局索引中删除
			assert_debug(_all_obj_manager[type].getByID(obj->getID()) != NULL, "");
			_all_obj_manager[type].remove(obj);

			if(obj->getType() == eRoomEntityType_Player){
				UserT* user = static_cast<UserT*>(obj);
				subGatewayConn(user->getGatewayConn());
			}

			obj->onRemoveFromRoomIndex();

			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::sendUserCmdToAround(const nPos2D& pos, IEvent* event) const
		{
			BUFFER_PROTO_CMD(protocol::forward::ForwardRoomUserCommand, send);
			assert_debug(PROTO_CMD_CAPACITY(protocol::forward::ForwardRoomUserCommand) > event->GetSize(), "");
			send->roomID = this->getID();
            CDynamicStreamBuf buff;
            COStream to(buff);
            event->SaveStreamNoThrow(to);
            send->num = to.GetBuf()->Size();
            send->clsID = event->GetCLSID();
            bcopy(to.GetBuf()->GetBytes(), send->data, send->num);
			return sendCmdToGatewayInRoom(send, send->size());
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::sendUserCmdToAroundExceptOne(const DWORD exceptCharID, const nPos2D& pos, IEvent* event) const
		{
			BUFFER_PROTO_CMD(protocol::forward::ForwardRoomUserExceptOneCommand, send);
			assert_debug(PROTO_CMD_CAPACITY(protocol::forward::ForwardRoomUserExceptOneCommand) > event->GetSize(), "");
			send->exceptCharID = exceptCharID;
			send->roomID = this->getID();
            CDynamicStreamBuf buff;
            COStream to(buff);
            event->SaveStreamNoThrow(to);
            send->num = to.GetBuf()->Size();
            send->clsID = event->GetCLSID();
            bcopy(to.GetBuf()->GetBytes(), send->data, send->num);
			return sendCmdToGatewayInRoom(send, send->size());
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::checkInAround(const nPos2D& pos, const nPos2D& otherPos)
		{
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		RoomObjT* nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::getRoomObjByTypeAndID(const BYTE type, const DWORD id) const
		{
			if(type >= eRoomEntityType_Max)
			{
				assert_fail("无法处理的场景对象类型");
				return NULL;
			}
			return _all_obj_manager[type].getByID(id);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		DWORD nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::getRoomObjNumByType(const BYTE type) const
		{
			if(type >= eRoomEntityType_Max)
			{
				assert_fail("无法处理的场景对象类型");
				return 0;
			}
			return _all_obj_manager[type].size();
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		UserT* nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::getUserByID(const DWORD charID) const
		{
			return static_cast<UserT*>(this->getRoomObjByTypeAndID(eRoomEntityType_Player, charID));
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllUser(ExecT &exe)
		{
			return execAllOfRoom(eRoomEntityType_Player, exe);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllObjByPos(const nPos2D& pos, const float distance, const BYTE direct, const WORD angle, ExecT &cb )
		{
            // TODO
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllObjByPos(const BYTE roomEntityType, const nPos2D& pos, const float distance, const BYTE direct, const WORD angle, ExecT &cb )
		{
			assert_fail("TODO");
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllObjByPos(const nPos2D& pos, const float distance, ExecT &cb )
		{
			assert_fail("TODO");
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllObjByPos(const BYTE roomEntityType, const nPos2D& pos, const float distance, ExecT &cb )
		{
			return execAllOfRoom(roomEntityType, pos, distance, cb);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllOfRoom( const BYTE type, ExecT &cb)
		{
			return _all_obj_manager[type].execAll(cb);
		}

	namespace{
		template<typename RoomObjT, typename ExecT>
			struct RangeObjExec{
				const nPos2D &_pos;
				const float _range;
				ExecT &_exe;
				RangeObjExec(const nPos2D &pos, const float range, ExecT &cb):
					_pos(pos),
					_range(range),
					_exe(cb)
				{
				}
				bool exec(RoomObjT *roomObj){
					if(roomObj->checkInRange(_pos, _range) == true){
						return _exe.exec(roomObj);
					}
					return true;
				}
			};
	};
	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllOfRoom( const BYTE type, const nPos2D &pos, const float range, ExecT &cb)
		{
			RangeObjExec<RoomObjT, ExecT> exe(pos, range, cb);
			return _all_obj_manager[type].execAll(exe);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllOfRoom( const nPos2D &pos, const float range, ExecT &cb)
		{
			RangeObjExec<RoomObjT, ExecT> exe(pos, range, cb);
			for(DWORD i = eRoomEntityType_None; i < eRoomEntityType_Max; ++ i){
				_all_obj_manager[i].execAll(exe);
			}
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllOfRoom(ExecT &cb)
		{
			for(DWORD i = 0; i < eRoomEntityType_Max; ++ i)
			{
				_all_obj_manager[i].execAll(cb);
			}
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllAroundObj(const nPos2D& pos, ExecT &exe)
		{
			return this->execAllOfRoom(exe);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllAroundObjInRange(const nPos2D& pos, const float range, ExecT &exe)
		{
			return this->execAllOfRoom(pos, range, exe);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllAroundObjByType(const eRoomEntityType type, const nPos2D& pos, ExecT &exec)
		{
			return this->execAllOfRoom(type, exec);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		template<typename ExecT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::execAllAroundObjByTypeInRange(const eRoomEntityType type, const nPos2D& pos, const float range, ExecT &exec)
		{
			return this->execAllOfRoom(type, pos, range, exec);
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::addGatewayConn( GatewayConnT *conn )
		{
			++ _gateway_conn_container[conn];
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::subGatewayConn( GatewayConnT *conn )
		{
			assert_debug(_gateway_conn_container[conn] > 0, "");
			typename GatewayContainerType::iterator iter = _gateway_conn_container.find(conn);
			-- (iter->second);
			if((iter->second) == 0)
			{
				_gateway_conn_container.erase(iter);
			}
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		bool nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::sendCmdToGatewayInRoom(protocol::NullProtoCommand* cmd, const DWORD len) const
		{
			for(GatewayContainer_constIter iter = _gateway_conn_container.begin(); iter != _gateway_conn_container.end(); ++ iter)
			{
				assert_debug((iter->second) > 0, "");
				(iter->first)->sendCmd(cmd, len);
			}
			return true;
		}

	template<class MapT, class GatewayConnT, class RoomObjT, class UserT>
		void nStaticIndex< MapT, GatewayConnT, RoomObjT, UserT >::removeGatewayConn( GatewayConnT *conn )
		{
			_gateway_conn_container.erase(conn);
		}

};

#endif //GAME_NSTATICINDEX_H_
