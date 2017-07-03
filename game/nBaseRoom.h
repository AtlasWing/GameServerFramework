#ifndef GAME_N_BASE_SCENE_H_
#define GAME_N_BASE_SCENE_H_

#include "base/nObj.h"

#include "game/nRegion.h"

#include "game/nGameServer.h"

namespace Game
{
	template<typename MapT>
		class nBaseRoom : public Tidy::nObjLogger<nBaseRoom<MapT> >
	{
		public:
			typedef MapT Map;

		protected:
			nBaseRoom(const Map* map, const DWORD roomID) : 
				_map(map), 
				_room_id(roomID)
		{
			snprintf(name, sizeof(name), "%s", map->getMapFileName());
		}

			~nBaseRoom()
			{
			}

		public:
			const char* getName() const;
			const DWORD getID() const;
			Tidy::nLogger* getLogger() const;
			const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;

		public:
			const MapT* getMap() const;
			const WORD getMapID() const;
			const WORD getMapType() const;
			const bool checkPosValid(const nPos2D& pos) const;
			nRegion getMapRegion() const;
			DWORD getCreateTimeInSec() const;

		private:
			char name[MAX_NAMESIZE];

		private:
			const MapT *_map;
			DWORD _room_id;
	};


	///< ---------------------------------------------------

	template<typename MapT>
		inline const char* nBaseRoom<MapT>::getName() const
		{
			return this->name;
		}

	template<typename MapT>
		inline const DWORD nBaseRoom<MapT>::getID() const
		{
			return this->_room_id;
		}

	template<typename MapT>
		inline Tidy::nLogger* nBaseRoom<MapT>::getLogger() const
		{
			return Tidy::baseServer->getLogger();
		}

	template<typename MapT>
		inline const DWORD nBaseRoom<MapT>::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
		{
			return snprintf(headerBuffer, bufferSize, "[%u,%s]", getID(), getName());;
		}

	template<typename MapT>
		inline const MapT* nBaseRoom<MapT>::getMap() const
		{
			return this->_map;
		}

	template<typename MapT>
		const WORD nBaseRoom<MapT>::getMapID() const
		{
			assert_debug(_map != NULL, "");
			return _map->getID();
		}

	template<typename MapT>
		const WORD nBaseRoom<MapT>::getMapType() const
		{
			assert_debug(_map != NULL, "");
			return _map->getMapType();
		}

	template<typename MapT>
		inline const bool nBaseRoom<MapT>::checkPosValid(const nPos2D& pos) const
		{
			return _map->checkPosValid(pos);
		}

};
#endif
