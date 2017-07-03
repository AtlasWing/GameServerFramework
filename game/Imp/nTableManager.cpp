#include "game/nTableManager.h"
#include "game/nTableBase.h"

namespace Game 
{
    NumericalFormula* TableManager::numerical = NULL;
    void TableManager::RegisterTable(string name, TableBase * table, string filePath)
    {
        if(table == NULL){
            return;
        }
        if(m_tabmap.find(name) != m_tabmap.end()){
            return;
        }
        table->FilePath = filePath;
        table->Loaded(name);
        m_tabmap.insert(pair<string, TableBase*>(name, table));
    }
}
