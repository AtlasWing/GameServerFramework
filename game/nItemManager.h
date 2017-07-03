#ifndef GAME_NITEMMANAGER_H_
#define GAME_NITEMMANAGER_H_

#include "base/nManager.h"

namespace Game
{
	template<typename ItemT>
		class nItemManager : public Tidy::nManager<true, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<ItemT> > >
	{
		public:
			typedef ItemT ItemType;

		public:
			nItemManager();
			~nItemManager();

		public:
			bool addItem(ItemType *item);
			void removeItem(ItemType *item);
			ItemType *getItemByID(DWORD id) const;
			void destoryAll();

		public:
			//bool saveItemData(protocol::serialize::PackageSetSaveData &saveData);

		public:
			template<typename ExecT>
				bool execEveryItem(ExecT& exec)
				{
					return execAll(exec);
				}
			template<typename ExecT>
				bool execEveryItem(ExecT& exec) const
				{
					return execAll(exec);
				}
	};

	template<typename ItemT>
		nItemManager<ItemT>::nItemManager()
		{
		}

	template<typename ItemT>
		nItemManager<ItemT>::~nItemManager()
		{
		}

	template<typename ItemT>
		bool nItemManager<ItemT>::addItem(ItemType *item)
		{
			if(add(item) == false){
				return false;
			}
			return true;
		}

	template<typename ItemT>
		void nItemManager<ItemT>::removeItem(ItemType *item)
		{
			this->remove(item);
		}

	template<typename ItemT>
		ItemT* nItemManager<ItemT>::getItemByID(DWORD id) const{
			return this->getByID(id);
		}
};

#endif
