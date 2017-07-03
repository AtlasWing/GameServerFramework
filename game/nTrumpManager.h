#ifndef GAME_NTRUMPMANAGER_H_
#define GAME_NTRUMPMANAGER_H_

#include "base/nManager.h"

namespace Game
{
	template<typename TrumpT>
		class nTrumpManager : public Tidy::nManager<true, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<TrumpT> > >
	{
		public:
			typedef TrumpT TrumpType;

		public:
			nTrumpManager();
			~nTrumpManager();

		public:
			bool addTrump(TrumpType *dupl);
			void removeTrump(TrumpType *dupl);
			TrumpType *getTrumpByID(DWORD id) const;
			void destoryAll();

		public:
			template<typename ExecT>
				bool execEveryTrump(ExecT& exec)
				{
					return execAll(exec);
				}
			template<typename ExecT>
				bool execEveryTrump(ExecT& exec) const
				{
					return execAll(exec);
				}
	};

	template<typename TrumpT>
		nTrumpManager<TrumpT>::nTrumpManager()
		{
		}

	template<typename TrumpT>
		nTrumpManager<TrumpT>::~nTrumpManager()
		{
		}

	template<typename TrumpT>
		bool nTrumpManager<TrumpT>::addTrump(TrumpType *dupl)
		{
			if(add(dupl) == false){
				return false;
			}
			return true;
		}

	template<typename TrumpT>
		void nTrumpManager<TrumpT>::removeTrump(TrumpType *dupl)
		{
			this->remove(dupl);
		}

	template<typename TrumpT>
		TrumpT* nTrumpManager<TrumpT>::getTrumpByID(DWORD id) const{
			return this->getByID(id);
		}
};

#endif
