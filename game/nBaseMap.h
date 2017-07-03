#ifndef _N_BASE_MAP_H
#define _N_BASE_MAP_H

#include "base/nObjLogger.h"
#include "base/nSTL.h"

#include "game/nTable.h"
#include "game/nTableManager.h"
#include "game/nPos.h"

namespace Game
{
    const DWORD MAX_POS = 256;
    class nBaseMap : public Tidy::nObjLogger<nBaseMap>
    {
        public:
            nBaseMap(const SceneTable *sceneData);
            virtual ~nBaseMap();

        public:
            const char* getMapFileName() const;
            const WORD getID() const;
            Tidy::nLogger* getLogger() const;
            const DWORD getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const;
            bool init();
            WORD getMapType() const;

        public:
            bool checkMapBlock(const nGridPos &pos) const;
            DWORD getGridIndex(const nGridPos &pos) const;
            DWORD getGridIndex(const nPos2D &pos) const;
            nGridPos getGridPos(const DWORD gridIndex) const;
            nGridPos getGridPos(const nPos2D &pos) const;
            bool checkPosValid(const nPos2D& pos) const;
            bool checkPosValid(const nGridPos &pos) const;
            bool checkPosValid(const DWORD gridIndex) const;

        public:
            const SceneTable *sceneTableData;

        private:
            bool initConfig();

        private:
            // 加载文件配置.比如地图NPC配置文件,只有某个服务器使用的配置放到这个函数中.
            virtual bool loadMapConfig();

        private:
            std::map<nGridPos, BYTE> _pos_data;
            WORD _x_grid_count;
            WORD _z_grid_count;
            WORD _grid_size;
            float _start_x;
            float _start_z;
            float _end_x;
            float _end_z;
    };
};
#endif
