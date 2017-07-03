#ifndef GAME_NDUPLMANAGER_H_
#define GAME_NDUPLMANAGER_H_

#include "base/nManager.h"

namespace Game
{
	template<typename DuplT>
		class nDuplManager : public Tidy::nManager<true, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<DuplT> > >
	{
		public:
			typedef DuplT DuplType;

		public:
			nDuplManager();
			~nDuplManager();

		public:
			bool addDupl(DuplType *dupl);
			void removeDupl(DuplType *dupl);
			DuplType *getDuplByID(DWORD id) const;
			void destoryAll();

		public:
			template<typename ExecT>
				bool execEveryDupl(ExecT& exec)
				{
					return execAll(exec);
				}
			template<typename ExecT>
				bool execEveryDupl(ExecT& exec) const
				{
					return execAll(exec);
				}
	};

	template<typename DuplT>
		nDuplManager<DuplT>::nDuplManager()
		{
		}

	template<typename DuplT>
		nDuplManager<DuplT>::~nDuplManager()
		{
		}

	template<typename DuplT>
		bool nDuplManager<DuplT>::addDupl(DuplType *dupl)
		{
			if(add(dupl) == false){
				return false;
			}
			return true;
		}

	template<typename DuplT>
		void nDuplManager<DuplT>::removeDupl(DuplType *dupl)
		{
			this->remove(dupl);
		}

	template<typename DuplT>
		DuplT* nDuplManager<DuplT>::getDuplByID(DWORD id) const{
			return this->getByID(id);
		}
};

#endif
