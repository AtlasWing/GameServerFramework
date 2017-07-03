#ifndef _GAME_NOBJ_H_
#define _GAME_NOBJ_H_

#include "protocol/roomCommand.h"

#include "base/nMisc.h"
#include "base/nServerTickTimer.h"
#include "base/nSig.h"

#include "game/nAlgo.h"
#include "game/nGameServer.h"
#include "game/nGameMessageService.h"

namespace Game
{
	struct nDir{
		nDir() : dirX(0.0), dirZ(0.0){
		}
		float dirX;
		float dirZ;
	};

	template<typename RoomT>
		class nRoomObj : public Tidy::nObj
	{
		public:
			typedef RoomT Room;

		protected:
			nRoomObj(const eRoomEntityType entityType, const DWORD id, const char* name); 
			virtual ~nRoomObj();

		public:
			Tidy::nLogger* getLogger() const;
			const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;

		public:
			bool hasRoom() const;
			BYTE getType() const;
			const nPos2D& getPos() const;
			const nPos3D& getPos3D() const;
			float getHeight() const;
			void setHeight(const float height);
			bool setPos(const nPos2D& pos);
			void setRoom(RoomT* room);
			RoomT* getRoom() const;
			DWORD getRoomID() const;
			bool checkInRoom() const;
			void setDir(const BYTE dir);
			BYTE getDirect() const;
			float getDistance(const nRoomObj<RoomT> *obj) const;
			float getDistance(const nPos2D &pos) const;
			bool checkInRange(const float posX, const float posZ, const float distance) const;
			bool checkInRange(const nPos2D &pos, const float distance) const;
			nGridPos getGridPos(const nPos2D& pos) const;
			nGridPos getGridPos() const;
			bool checkCommonRegion(const BYTE commonRegionType) const;
			const bool checkInAround(const nPos2D &pos) const;

		public:
			bool isUser() const;
			bool isNpc() const;
			bool isPet() const;

		public:
			virtual const DWORD fillRemoveFromRoomCommand(char* buffer, const DWORD maxLen, const BYTE removeType) = 0;

		public:
			void setBlockType(WORD type);
			void clearBlockType(WORD type);
			const WORD getBlockType() const;

		public:
			bool checkMove(const nPos2D &destPos, const BYTE campType) const;
			void setInSpace(DWORD maxTimeInSec, const float height);
			void clearInSpace();
			bool inInSpace() const;
			bool getValidPosInLine(const nPos2D &fromPos, const nPos2D &destPos, nPos2D *validPos, const BYTE campType) const;

		private:
			bool checkMove(const nPos2D &fromPos, const nPos2D &destPos, const BYTE campType) const;
			void checkSpaceHeight() const;

		private:
			RoomT* _room;
			nPos3D _pos;
			BYTE _type;
			BYTE _dir;
			WORD _block_type;
			Tidy::nServerTickTimer *_in_space_timer;
	};

	template<typename RoomT>
		nRoomObj<RoomT>::nRoomObj(const eRoomEntityType objType, const DWORD id, const char* name):
			Tidy::nObj(id, name),
			_room(NULL),
			_pos(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT),
			_type(objType),
			_dir(0),
			_in_space_timer(NULL)
	{
	}

	template<typename RoomT>
		nRoomObj<RoomT>::~nRoomObj()
		{
			SAFE_DELETE(_in_space_timer);
		}

	template<typename RoomT>
		Tidy::nLogger* nRoomObj<RoomT>::getLogger() const
		{
			return gameServer->getLogger();
		}

	template<typename RoomT>
		const DWORD nRoomObj<RoomT>::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
		{
			return Tidy::nStringTool::copyStr(headerBuffer, "Obj", bufferSize);
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::hasRoom() const
		{
			return _room != NULL; 
		}

	template<typename RoomT>
		BYTE nRoomObj<RoomT>::getType() const
		{
			return _type;
		}

	template<typename RoomT>
		const nPos2D& nRoomObj<RoomT>::getPos() const
		{
			//只是返回二维坐标,所以不需要checkSpaceHeight
			return _pos;
		}

	template<typename RoomT>
		const nPos3D& nRoomObj<RoomT>::getPos3D() const
		{
			checkSpaceHeight();
			return _pos;
		}

	template<typename RoomT>
		float nRoomObj<RoomT>::getHeight() const
		{
			checkSpaceHeight();
			return _pos.y;
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::setHeight(const float height)
		{
			_pos.y = height;
		}

	template<typename RoomT>
		nGridPos nRoomObj<RoomT>::getGridPos(const nPos2D& pos) const
		{
			return this->getRoom()->getMap()->getGridPos(pos);
		}

	template<typename RoomT>
		nGridPos nRoomObj<RoomT>::getGridPos() const
		{
			return this->getGridPos(getPos());
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::checkCommonRegion(const BYTE commonRegionType) const
		{
			return this->getRoom()->getMap()->checkCommonRegion(this->getPos(), commonRegionType);
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::setPos(const nPos2D& pos)
		{
			_pos.x = pos.x;
			_pos.z = pos.z;
			return true;
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::setRoom(RoomT* room)
		{
			_room = room;
		}

	template<typename RoomT>
		RoomT* nRoomObj<RoomT>::getRoom() const
		{
			return _room;
		}

	template<typename RoomT>
		DWORD nRoomObj<RoomT>::getRoomID() const
		{
			return _room->getID();
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::setDir(const BYTE dir)
		{
			_dir = dir;
		}

	template<typename RoomT>
		BYTE nRoomObj<RoomT>::getDirect() const
		{
			return _dir;
		}

	template<typename RoomT>
		float nRoomObj<RoomT>::getDistance(const nRoomObj<RoomT> *obj) const
		{
			return getDistance(obj->getPos());
		}

	template<typename RoomT>
		float nRoomObj<RoomT>::getDistance(const nPos2D &pos) const
		{
			return nGraphAlgo::getDistance(getPos(), pos);
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::checkInRange(const float posX, const float posZ, const float distance) const
		{
			return getDistance(nPos2D(posX, posZ)) <= distance;
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::checkInRange(const nPos2D &pos, const float distance) const
		{
			return getDistance(pos) <= distance;
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::isUser() const
		{
			return _type == eRoomEntityType_Player;
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::isNpc() const
		{
			return _type == eRoomEntityType_NPC;
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::isPet() const
		{
			return _type == eRoomEntityType_Pet;
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::checkInRoom() const
		{
			return _room != NULL;
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::setBlockType(WORD type)
		{
			_block_type |= type;
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::clearBlockType(WORD type)
		{
			_block_type &= ~type;
		}

	template<typename RoomT>
		const WORD nRoomObj<RoomT>::getBlockType() const
		{
			return _block_type;
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::checkMove(const nPos2D &destPos, const BYTE campType) const
		{
			return checkMove(_pos, destPos, campType);
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::checkMove(const nPos2D &fromPos, const nPos2D &destPos, const BYTE campType) const
		{
			if(this->getRoom()->checkPosValid(destPos) == false){
				return false;
			}

			const DWORD curGridIndex = this->getRoom()->getMap()->getGridIndex(fromPos);
			const DWORD destGridIndex = this->getRoom()->getMap()->getGridIndex(destPos);
			if(curGridIndex == destGridIndex){
				return true;
			}

			return true;
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::getValidPosInLine(const nPos2D &fromPos, const nPos2D &destPos, nPos2D *validPos, const BYTE campType) const
		{
			assert_debug(&destPos != validPos, "");
			float lineLength = nGraphAlgo::getDistance(fromPos, destPos);
			const float gridSize = getRoom()->getMap()->getGridSize();
			nPos2D unitVec((gridSize * (destPos.x - fromPos.x)/lineLength), (gridSize * (destPos.z - fromPos.z)/lineLength));
			*validPos = fromPos;
			nPos2D nextCheckPos = fromPos + unitVec;
			do{
				if(nGraphAlgo::getDistance(fromPos, nextCheckPos) >= lineLength){
					return true;
				}
				if(checkMove(*validPos, nextCheckPos, campType) == false){
					return false;
				}
				*validPos = nextCheckPos;
				nextCheckPos = nextCheckPos + unitVec;
			}while(true);

			return false;
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::setInSpace(DWORD maxTimeInMsec, const float  height)
		{
			if(_in_space_timer == NULL){
				_in_space_timer = new Tidy::nServerTickTimer(maxTimeInMsec);
			}
			else{
				_in_space_timer->reset(maxTimeInMsec, gameServer->time());
			}
			_pos.y = this->getRoom()->getHeight(_pos) + height;
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::clearInSpace()
		{
			if(_in_space_timer == NULL){
				return;
			}
			SAFE_DELETE(_in_space_timer);
			_pos.y = this->getRoom()->getHeight(_pos);
		}

	template<typename RoomT>
		bool nRoomObj<RoomT>::inInSpace() const
		{
			checkSpaceHeight();
			return _in_space_timer != NULL;
		}

	template<typename RoomT>
		void nRoomObj<RoomT>::checkSpaceHeight() const
		{
			if(_in_space_timer == NULL){
				*const_cast<float*>(&this->_pos.y) = this->getRoom()->getHeight(_pos, eCampType_None);
			}
			else if(_in_space_timer->isReady() == true){
				const_cast<nRoomObj<RoomT>*>(this)->clearInSpace();
			}
		}
};
#endif
