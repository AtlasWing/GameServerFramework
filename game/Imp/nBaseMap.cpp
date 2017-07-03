#include "base/nXMLParser.h"
#include "base/nDebugger.h"
#include "base/nBaseMonitorService.h"
#include "base/nDictionary.h"

#include "game/nBaseMap.h"
#include "game/nGameServer.h"
#include "game/nAlgo.h"
#include "game/nTable.h"
#include "game/nTableManager.h"

namespace Game
{
    nBaseMap::nBaseMap(const SceneTable *sceneData):
        sceneTableData(sceneData),
        _grid_size(1), // TODO 读表
        _x_grid_count(0),
        _z_grid_count(0),
        _start_x(0.0),
        _start_z(0.0),
        _end_x(0.0),
        _end_z(0.0)
    {
    }
    
    nBaseMap::~nBaseMap()
    {
    }

    bool nBaseMap::init()
    {
        if(initConfig() == false){
            assert_fail("初始化地图数据失败");
            return false;
        }
        return true;
    }

    bool nBaseMap::initConfig()
    {
        std::string fullFileName = "";
        fullFileName = fullFileName + (const char*)Tidy::global["mapdir"] + sceneTableData->Name + ".txt";

        std::ifstream file;
        file.open(fullFileName.c_str());
        if(file.fail() == true){
            assert_fail("打开文件失败");
            return false;
        }
        char buffer[64];
        std::string posStr;
        std::vector<std::string> posBlockVec;
        std::vector<DWORD> posData;
        std::stringstream ss;
        BYTE block = 0;
        file.getline(buffer, 16);
        while(file.eof() == false && file.fail() == false){
            posStr.assign(buffer);
            posBlockVec.clear();
            if(posStr.empty() == false){
                Tidy::nStringTool::splitValue(posStr, '\t', posBlockVec);
                if(posBlockVec.size() != 2){
                    assert_fail("配置地图阻挡错误");
                    return false;
                }
                Tidy::nStringTool::splitValue(posBlockVec[0], '_', posData);
                if(posData.size() != 2){
                    assert_fail("配置地图阻挡位置错误");
                    return false;
                }
                ss.clear();
                ss.str("");
                ss << posBlockVec[1];
                ss >> block;
                nGridPos newPos(posData[0], posData[1]);
                _pos_data[newPos] = block;
                if(posData[0] > _x_grid_count){
                    _x_grid_count = posData[0];
                }
                if(posData[1] > _z_grid_count){
                    _z_grid_count = posData[1];
                }
            }
            file.getline(buffer, 64);
        }
        _x_grid_count += 1;
        _z_grid_count += 1; // 因为从0,0 点开始算起
        _end_x = _start_x + _x_grid_count * _grid_size;
        _end_z = _start_z + _z_grid_count * _grid_size;
        return true;
    }

    const char* nBaseMap::getMapFileName() const
    {
        return sceneTableData->Name.c_str();
    }

    const WORD nBaseMap::getID() const
    {
        return ASSERT_CAST(WORD, sceneTableData->ID);
    }

    Tidy::nLogger* nBaseMap::getLogger() const
    {
        return Tidy::baseServer->getLogger();
    }

    const DWORD nBaseMap::getEntityLogHead(char* headerBuffer, const DWORD bufferSize) const
    {
        return snprintf(headerBuffer, bufferSize, "[%u,%s]", getID(), getMapFileName());;
    }

    WORD nBaseMap::getMapType() const
    {
        return sceneTableData->SceneType;
    }

    bool nBaseMap::checkMapBlock(const nGridPos &pos) const
    {
        std::map<nGridPos, BYTE>::const_iterator iter = _pos_data.find(pos);
        if(iter == _pos_data.end()){
            return false;
        }
        return iter->second > 0; 
    }

    bool nBaseMap::loadMapConfig()
    {
        return true;
    }

    DWORD nBaseMap::getGridIndex(const nGridPos &pos) const
    {    
        assert_debug(pos.x < _x_grid_count && pos.z < _z_grid_count, ""); 
        return pos.x * _x_grid_count + pos.z;
    }
    
    DWORD nBaseMap::getGridIndex(const nPos2D &pos) const
    {    
        return getGridIndex(getGridPos(pos));
    }

    nGridPos nBaseMap::getGridPos(const DWORD gridIndex) const
    {
        assert_debug(checkPosValid(gridIndex), "[%u]", gridIndex);
        return nGridPos(gridIndex/_x_grid_count, gridIndex%_x_grid_count);
    }

    nGridPos nBaseMap::getGridPos(const nPos2D &pos) const
    {
        assert_debug(checkPosValid(pos), "[%f,%f]", pos.x, pos.z);
        return nGridPos(static_cast<DWORD>((pos.x-_start_x)/_grid_size), static_cast<DWORD>((pos.z-_start_z)/_grid_size));
    }

    bool nBaseMap::checkPosValid(const nPos2D& pos) const
    {   
        return (pos.x > _start_x && pos.x < _end_x && pos.z > _start_z && pos.z < _end_z);
    }

    bool nBaseMap::checkPosValid(const nGridPos &pos) const
    {   
        return pos.x < _x_grid_count && pos.z < _z_grid_count;
    }

    bool nBaseMap::checkPosValid(const DWORD gridIndex) const
    {
        return gridIndex < (_x_grid_count * _z_grid_count);
    }
};
