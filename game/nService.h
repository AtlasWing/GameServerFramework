#ifndef _N_SERVICE_H_
#define _N_SERVICE_H_

namespace Game
{
	class nService
	{
		protected:
			nService(const char* serviceName);
			~nService();

		public:
			virtual bool initService() = 0;
			virtual void finalService() = 0;
	};
};
#endif
