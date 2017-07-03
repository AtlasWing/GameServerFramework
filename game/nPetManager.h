#ifndef GAME_NPETMANAGER_H_
#define GAME_NPETMANAGER_H_

#include "base/nManager.h"

namespace Game
{
	template<typename PetT>
		class nPetManager : public Tidy::nManager<true, Tidy::nIndexByID< Tidy::CommonGetIDPolicy<PetT> > >
	{
		public:
			typedef PetT PetType;

		public:
			nPetManager();
			~nPetManager();

		public:
			bool addPet(PetType *pet);
			void removePet(PetType *pet);
			PetType *getPetByID(DWORD id) const;
			void destoryAll();

		public:
			template<typename ExecT>
				bool execEveryPet(ExecT& exec)
				{
					return execAll(exec);
				}
			template<typename ExecT>
				bool execEveryPet(ExecT& exec) const
				{
					return execAll(exec);
				}
	};

	template<typename PetT>
		nPetManager<PetT>::nPetManager()
		{
		}

	template<typename PetT>
		nPetManager<PetT>::~nPetManager()
		{
		}

	template<typename PetT>
		bool nPetManager<PetT>::addPet(PetType *pet)
		{
			if(add(pet) == false){
				return false;
			}
			return true;
		}

	template<typename PetT>
		void nPetManager<PetT>::removePet(PetType *pet)
		{
			this->remove(pet);
		}

	template<typename PetT>
		PetT* nPetManager<PetT>::getPetByID(DWORD id) const{
			return this->getByID(id);
		}
};

#endif
