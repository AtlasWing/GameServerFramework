#ifndef	_TC_HASH_FUN_H_
#define _TC_HASH_FUN_H_

#include <iostream>
#include <string>

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file tc_hash_fun.h 
 * @brief hash�㷨.
 *���Զ�������ֽ�������hash�õ��൱���ȵ�hashֵ 
 * @author jarodruan@tencent.com 
 *  
 *
 */             
/////////////////////////////////////////////////

template <class _Key> struct hash { };
template <class _Key> struct hash_new { };

inline size_t hash_string(const char* s)
{
    unsigned long h = 0; 
    for ( ; *s; ++s)
    h = 5*h + *s;

    return size_t(h);
}

//////////////////////////////////////////////////////////
/**
 * @brief ��������hash_new, ������һЩ.
 *  
 *���Զ�������ֽ�������hash�õ��൱���ȵ�hashֵ
 */
//////////////////////////////////////////////////////////
template <>
struct hash<string>
{
    size_t operator()(const string &s) const
    {
        size_t h = 0, g;
        const char *arKey = s.c_str();
        size_t nKeyLength = s.length();
        const char *arEnd = arKey + nKeyLength;
        while (arKey < arEnd)
        {
            h = (h << 4) + *arKey++;
            if ((g = (h & 0xF0000000)))
            {
                h = h ^ (g >> 24);
                h = h ^ g;
            }
        }
        return h;
    }
};

template <>
struct hash_new<string>
{
    size_t operator()(const string &s) const
    {
    	const char *ptr= s.c_str();
        size_t key_length = s.length();
    	uint32_t value= 0;
    	
    	while (key_length--) 
    	{
    		value += *ptr++;
    		value += (value << 10);
    		value ^= (value >> 6);
    	}
    	value += (value << 3);
    	value ^= (value >> 11);
    	value += (value << 15); 
    	
    	return value == 0 ? 1 : value;
    }
};

template <>
struct hash<char*>
{ 
    size_t operator()(const char* s) const { return hash_string(s); }
};

template <>
struct hash<const char*>
{ 
    size_t operator()(const char* s) const { return hash_string(s); }
};

template <>
struct hash<char> 
{ 
    size_t operator()(char x) const { return x; }
};

template <>
struct hash<unsigned char> 
{ 
    size_t operator()(unsigned char x) const { return x; }
};

template <>
struct hash<signed char> 
{ 
    size_t operator()(unsigned char x) const { return x; }
};

template <>
struct hash<short> 
{ 
    size_t operator()(short x) const { return x; }
};

template <>
struct hash<unsigned short> 
{ 
    size_t operator()(unsigned short x) const { return x; }
};

template <>
struct hash<int> 
{
    size_t operator()(int x) const { return x; }
};

template <>
struct hash<unsigned int> 
{ 
    size_t operator()(unsigned int x) const { return x; }
};

template <>
struct hash<long> 
{ 
    size_t operator()(long x) const { return x; }
};

template <>
struct hash<unsigned long> 
{ 
    size_t operator()(unsigned long x) const { return x; }
};

/**
* @brief һ�������hash�㷨.
*  
*���Զ�������ֽ�������hash�õ��൱���ȵ�hashֵ
*/
struct magic_string_hash
{
    size_t operator()(const string &s) const
    {
		const char *ptr= s.c_str();
		size_t key_length = s.length();
		uint32_t value= 0;
		
		while (key_length--) 
		{
			value += *ptr++;
			value += (value << 10);
			value ^= (value >> 6);
		}
		value += (value << 3);
		value ^= (value >> 11);
		value += (value << 15); 
		
		return value == 0 ? 1 : value;
    }
};

////////////////////////////////////////////////////////////////////
}

#endif

