#ifndef _NRANGE_H_
#define _NRANGE_H_

#include "base/Tidy.h"
#include "base/nSTL.h"
#include "base/nRandom.h"
#include "base/nStringTool.h"

namespace Tidy
{
	/// @brief 区间
	template <typename T>
		struct nRange
		{
			public:
				nRange()
				{
					_min = T();
					_max = T();
				}
				nRange(const T min, const T max)
				{
					this->_min = min;
					this->_max = max;
					if(_min > _max)
						swap();
				}

			public:
				/// @brief	设置区间
				void set(const T& _min, const T& _max);
				/// @brief	解析区间
				void parse(const std::string &str, const char c);
				/// @brief	内容清零
				void clear(void);
				void swap(void);
				const bool isValid() const;
				/// @brief 取区间中的一个随机值
				const T getValueByRand() const;
			public:
				/// @brief 赋值操作符号
				nRange<T> & operator= (const nRange<T> &r);
				/// @brief 重载+运算符号
				const nRange<T> & operator+ (const nRange<T> &r);
				/// @brief 重载+=运算符号
				const nRange<T> & operator+= (const nRange<T> &r);
				/// @brief 重载-运算符号
				const nRange<T> & operator- (const nRange<T> &r);
				/// @brief 重载-=运算符号
				const nRange<T> & operator-= (const nRange<T> &r);
				/// @brief 重载==逻辑运算符号
				const bool operator== (const nRange<T> &r) const;
				/// @brief 重载>逻辑运算符号
				const bool operator> (const nRange<T> &r) const;
				/// @brief 重载>=逻辑运算符号
				const bool operator>= (const nRange<T> &r) const;
				/// @brief 重载<逻辑运算符号
				const bool operator< (const nRange<T> &r) const;
				/// @brief 重载<=逻辑运算符号
				const bool operator<= (const nRange<T> &r) const;
				/// @brief 重载!=逻辑运算符号
				const bool operator!= (const nRange<T> &r) const;
				/// @brief 重载()逻辑运算符号
				const T operator()() const;
			public :
				T _min;
				T _max;
		};

	template <typename T>
		nRange<T>& nRange<T>::operator= (const nRange<T> &r)
		{
			_min = r._min;
			_max = r._max;
			return *this;
		}

	template <typename T>
		const nRange<T>& nRange<T>::operator+ (const nRange<T> &r)
		{
			_min += r._min;
			_max += r._max;
			return *this;
		}

	template <typename T>
		const nRange<T>& nRange<T>::operator+= (const nRange<T> &r)
		{
			_min += r._min;
			_max += r._max;
			return *this;
		}

	template <typename T>
		const nRange<T>& nRange<T>::operator- (const nRange<T> &r)
		{
			_min -= r._min;
			_max -= r._max;
			return *this;
		}

	template <typename T>
		const nRange<T>& nRange<T>::operator-= (const nRange<T> &r)
		{
			_min -= r._min;
			_max -= r._max;
			return *this;
		}

	template <typename T>
		const bool nRange<T>::operator== (const nRange<T> &r) const
		{
			return (_min == r._min) && (_max == r._max);
		}

	template <typename T>
		const bool nRange<T>::operator> (const nRange<T> &r) const
		{
			return (_max > r._max) && (_min < r._min);
		}

	template <typename T>
		const bool nRange<T>::operator>= (const nRange<T> &r) const
		{
			return (_max >= r._max) && (_min <= r._min);
		}

	template <typename T>
		const bool nRange<T>::operator< (const nRange<T> &r) const
		{
			return (_max < r._max) && (_min > r._min);
		}

	template <typename T>
		const bool nRange<T>::operator<= (const nRange<T> &r) const
		{
			return (_max <= r._max) && (_min >= r._min);
		}

	template <typename T>
		const bool nRange<T>::operator!= (const nRange<T> &r) const
		{
			return (_max != r._max) || (_min != r._min);
		}

	template <typename T>
		void nRange<T>::set(const T& _min, const T& _max)
		{
			this->_min = _min;
			this->_max = _max;
		}

	template <typename T>
		void nRange<T>::parse(const std::string &str, const char c)
		{
			std::string trimStr = str;
			nStringTool::trim(trimStr);
			std::vector<std::string> valueStrVec;
			nStringTool::split(trimStr, c, valueStrVec);
			if(valueStrVec.size() > 0)
			{
				_min = _max = (T)(nAny(valueStrVec[0]));
				if(valueStrVec.size() > 1)
					_max = (T)(nAny(valueStrVec[1]));
			}
		}

	template <typename T>
		void nRange<T>::clear(void)
		{
			_min = 0;
			_max = 0;
		}

	template <typename T>
		void nRange<T>::swap(void)
		{
			T temp = _min;
			_max = _min;
			_min = temp;
		}

	template <typename T>
		const bool nRange<T>::isValid() const
		{
			return _max >= _min;
		}

	template <typename T>
		const T nRange<T>::getValueByRand() const
		{
			return (T)Tidy::randBetween(_min,_max);
		}

	template <typename T>
		const T nRange<T>::operator()() const
		{
			return getValueByRand();
		}
};

#endif
