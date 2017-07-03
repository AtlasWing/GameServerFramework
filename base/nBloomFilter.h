#ifndef BASE_NBLOOMFILTER_H
#define BASE_NBLOOMFILTER_H

#include "base/nType.h"

namespace Tidy
{
	class bloom_filter;
	class nBloomFilter
	{
		public:
			nBloomFilter(const DWORD assumeKeyNum, const double assumeFalsePositiveProbability);
			~nBloomFilter();

		public:
			bool init(const DWORD assumeKeyNum, const double assumeFalsePositiveProbability);
			DWORD getReservedSpaceSize() const;
			double getFalsePositiveProbability() const;

		public:
			bool test(const char *key, const DWORD len) const;
			void set(const char *key, const DWORD len);

		private:
			bloom_filter *_bloom_filter_imp;
	};
};
#endif
