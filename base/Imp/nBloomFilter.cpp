#include "base/nBloomFilter.h"
#include "base/Imp/nBloomFilterImp.h"
#include "base/nRandom.h"

namespace Tidy
{
	nBloomFilter::nBloomFilter(const DWORD assumeKeyNum, const double falsePositiveProbability):
		_bloom_filter_imp(NULL)
	{
		init(assumeKeyNum, falsePositiveProbability);
	}

	nBloomFilter::~nBloomFilter()
	{
		SAFE_DELETE(_bloom_filter_imp);
	}

	bool nBloomFilter::init(const DWORD assumeKeyNum, const double falsePositiveProbability)
	{
		bloom_parameters parameters;
		parameters.projected_element_count    = assumeKeyNum;
		parameters.false_positive_probability = falsePositiveProbability;
		parameters.random_seed                = Tidy::randBetween(1, 100000);
		parameters.maximum_number_of_hashes   = 7;
		parameters.compute_optimal_parameters();

		_bloom_filter_imp = new bloom_filter(parameters);
		return true;
	}

	DWORD nBloomFilter::getReservedSpaceSize() const
	{
		return _bloom_filter_imp->size()/bits_per_char;
	}

	double nBloomFilter::getFalsePositiveProbability() const
	{
		return _bloom_filter_imp->effective_fpp();
	}

	bool nBloomFilter::test(const char *key, const DWORD len) const
	{
		return _bloom_filter_imp->contains(key, len);
	}

	void nBloomFilter::set(const char *key, const DWORD len)
	{
		_bloom_filter_imp->insert(key, len);
	}
};
