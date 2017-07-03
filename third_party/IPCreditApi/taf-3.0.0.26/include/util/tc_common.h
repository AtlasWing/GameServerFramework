#ifndef __TC_COMMON_H
#define __TC_COMMON_H

#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif

#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>
#include <stack>
#include <vector>
#include "util/tc_loki.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
/**
* @file tc_common.h
* @brief  ������,���Ǿ�̬����,�ṩһЩ���õĺ��� .
*
* @author  jarodruan@tencent.com
*
*/
/////////////////////////////////////////////////

 /**
 * @brief  ���������࣬�ṩ��һЩ�ǳ������ĺ���ʹ��.
 *
 * ��Щ���������Ծ�̬�����ṩ�� �������¼��ֺ���:
 *
 * Trim�ຯ��,��Сдת������,�ָ��ַ���������ֱ�ӷָ��ַ�����
 *
 * ���ֵȣ�,ʱ����غ���,�ַ���ת������,�������ַ�����ת����,
 *
 * �滻�ַ�������,Ipƥ�亯��,�ж�һ�����Ƿ���������
 */
class TC_Common
{
public:

    /**
	* @brief  ȥ��ͷ���Լ�β�����ַ����ַ���.
	*
    * @param sStr    �����ַ���
    * @param s       ��Ҫȥ�����ַ�
    * @param bChar   ���Ϊtrue, ��ȥ��s��ÿ���ַ�; ���Ϊfalse, ��ȥ��s�ַ���
    * @return string ����ȥ������ַ���
    */
    static string trim(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
	* @brief  ȥ����ߵ��ַ����ַ���.
	*
    * @param sStr    �����ַ���
    * @param s       ��Ҫȥ�����ַ�
    * @param bChar   ���Ϊtrue, ��ȥ��s��ÿ���ַ�; ���Ϊfalse, ��ȥ��s�ַ���
    * @return string ����ȥ������ַ���
    */
    static string trimleft(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
	* @brief  ȥ���ұߵ��ַ����ַ���.
	*
    * @param sStr    �����ַ���
    * @param s       ��Ҫȥ�����ַ�
    * @param bChar   ���Ϊtrue, ��ȥ��s��ÿ���ַ�; ���Ϊfalse, ��ȥ��s�ַ���
    * @return string ����ȥ������ַ���
    */
    static string trimright(const string &sStr, const string &s = " \r\n\t", bool bChar = true);

    /**
	* @brief  �ַ���ת����Сд.
	*
    * @param sString  �ַ���
    * @return string  ת������ַ���
    */
    static string lower(const string &sString);

    /**
	* @brief  �ַ���ת���ɴ�д.
	*
    * @param sString  �ַ���
    * @return string  ת����Ĵ�д���ַ���
    */
    static string upper(const string &sString);

    /**
	* @brief  �ַ����Ƿ������ֵ�.
	*
    * @param sString  �ַ���
    * @return bool    �Ƿ�������
    */
    static bool isdigit(const string &sInput);

    /**
	* @brief  �ַ���ת����ʱ��ṹ.
	*
    * @param sString  �ַ���ʱ���ʽ
    * @param sFormat  ��ʽ
    * @param stTm     ʱ��ṹ
    * @return         0: �ɹ�, -1:ʧ��
    */
    static int str2tm(const string &sString, const string &sFormat, struct tm &stTm);

	/**
	 * @brief GMT��ʽ��ʱ��ת��Ϊʱ��ṹ
	 *
	 * eg.Sat, 06 Feb 2010 09:29:29 GMT, %a, %d %b %Y %H:%M:%S GMT
	 *
	 * ������mktime����time_t, ����ע��ʱ�� ������mktime(&stTm)
	 *
	 * - timezone���ɱ��ص���(time(NULL)ֵ��ͬ) timezone��ϵͳ�� ,
	 *
	 * ��Ҫextern long timezone;
	 *
     * @param sString  GMT��ʽ��ʱ��
	 * @param stTm     ת�����ʱ��ṹ
     * @return         0: �ɹ�, -1:ʧ��
     */
    static int strgmt2tm(const string &sString, struct tm &stTm);

    /**
	* @brief  ʱ��ת�����ַ���.
	*
    * @param stTm     ʱ��ṹ
    * @param sFormat  ��Ҫת����Ŀ���ʽ��Ĭ��Ϊ���ո�ʽ
    * @return string  ת�����ʱ���ַ���
    */
    static string tm2str(const struct tm &stTm, const string &sFormat = "%Y%m%d%H%M%S");

    /**
	* @brief  ʱ��ת�����ַ���.
	*
    * @param t        ʱ��ṹ
    * @param sFormat  ��Ҫת����Ŀ���ʽ��Ĭ��Ϊ���ո�ʽ
    * @return string  ת�����ʱ���ַ���
    */
    static string tm2str(const time_t &t, const string &sFormat = "%Y%m%d%H%M%S");

    /**
	* @brief  ��ǰʱ��ת���ɽ��ո�ʽ�ַ���
    * @param sFormat ��ʽ��Ĭ��Ϊ���ո�ʽ
    * @return string ת�����ʱ���ַ���
    */
    static string now2str(const string &sFormat = "%Y%m%d%H%M%S");

    /**
	* @brief  ʱ��ת����GMT�ַ�����GMT��ʽ:Fri, 12 Jan 2001 18:18:18 GMT
    * @param stTm    ʱ��ṹ
    * @return string GMT��ʽ��ʱ���ַ���
    */
    static string tm2GMTstr(const struct tm &stTm);

    /**
	* @brief  ʱ��ת����GMT�ַ�����GMT��ʽ:Fri, 12 Jan 2001 18:18:18 GMT
    * @param stTm    ʱ��ṹ
    * @return string GMT��ʽ��ʱ���ַ���
    */
    static string tm2GMTstr(const time_t &t);

    /**
	* @brief  ��ǰʱ��ת����GMT�ַ�����GMT��ʽ:Fri, 12 Jan 2001 18:18:18 GMT
    * @return string GMT��ʽ��ʱ���ַ���
    */
    static string now2GMTstr();

    /**
	* @brief  ��ǰ������(������)ת�����ַ���(%Y%m%d).
	*
    * @return string (%Y%m%d)��ʽ��ʱ���ַ���
    */
    static string nowdate2str();

    /**
	* @brief  ��ǰ��ʱ��(ʱ����)ת�����ַ���(%H%M%S).
	*
    * @return string (%H%M%S)��ʽ��ʱ���ַ���
    */
    static string nowtime2str();

    /**
     * @brief  ��ȡ��ǰʱ��ĵĺ�����.
     *
     * @return int64_t ��ǰʱ��ĵĺ�����
     */
    static int64_t now2ms();

    /**
     * @brief  ȡ����ǰʱ���΢��.
     *
     * @return int64_t ȡ����ǰʱ���΢��
     */
    static int64_t now2us();

    /**
	* @brief  �ַ���ת����T�ͣ����T����ֵ����, ���strΪ��,��TΪ0.
	*
    * @param sStr  Ҫת�����ַ���
    * @return T    T������
    */
    template<typename T>
    static T strto(const string &sStr);

    /**
	* @brief  �ַ���ת����T��.
	*
    * @param sStr      Ҫת�����ַ���
    * @param sDefault  ȱʡֵ
    * @return T        ת�����T����
    */
    template<typename T>
    static T strto(const string &sStr, const string &sDefault);

    /**
	* @brief  �����ַ���,�÷ָ����ŷָ�,������vector��
	*
	* ����: |a|b||c|
	*
    * ���withEmpty=trueʱ, ����|�ָ�Ϊ:"","a", "b", "", "c", ""
	*
    * ���withEmpty=falseʱ, ����|�ָ�Ϊ:"a", "b", "c"
	*
    * ���T����Ϊint����ֵ����, ��ָ����ַ���Ϊ"", ��ǿ��ת��Ϊ0
    *
    * @param sStr      �����ַ���
    * @param sSep      �ָ��ַ���(ÿ���ַ�����Ϊ�ָ���)
    * @param withEmpty true����յ�Ҳ��һ��Ԫ��, falseʱ�յĹ���
    * @return          ��������ַ�vector
    */
    template<typename T>
    static vector<T> sepstr(const string &sStr, const string &sSep, bool withEmpty = false);

    /**
	* @brief T��ת�����ַ�����ֻҪT�ܹ�ʹ��ostream������<<����,�����Ա��ú���֧��
    * @param t Ҫת��������
    * @return  ת������ַ���
    */
    template<typename T>
    static string tostr(const T &t);

    /**
	 * @brief  vectorת����string.
	 *
     * @param t Ҫת����vector�͵�����
     * @return  ת������ַ���
     */
    template<typename T>
    static string tostr(const vector<T> &t);

    /**
	 * @brief  ��map���Ϊ�ַ���.
	 *
	 * @param map<K, V, D, A>  Ҫת����map����
     * @return                    string ������ַ���
     */
    template<typename K, typename V, typename D, typename A>
    static string tostr(const map<K, V, D, A> &t);

    /**
	 * @brief  map���Ϊ�ַ���.
	 *
	 * @param multimap<K, V, D, A>  map����
     * @return                      ������ַ���
     */
    template<typename K, typename V, typename D, typename A>
    static string tostr(const multimap<K, V, D, A> &t);

    /**
	* @brief  pair ת��Ϊ�ַ�������֤map�ȹ�ϵ��������ֱ����tostr�����
    * @param pair<F, S> pair����
    * @return           ������ַ���
    */
    template<typename F, typename S>
    static string tostr(const pair<F, S> &itPair);

    /**
	* @brief  container ת�����ַ���.
	*
    * @param iFirst  ������
    * @param iLast   ������
    * @param sSep    ����Ԫ��֮��ķָ���
    * @return        ת������ַ���
    */
    template <typename InputIter>
    static string tostr(InputIter iFirst, InputIter iLast, const string &sSep = "|");

    /**
	* @brief  ����������ת�����ַ���.
	*
    * @param buf     ������buffer
    * @param len     buffer����
    * @param sSep    �ָ���
    * @param lines   ���ٸ��ֽڻ�һ��, Ĭ��0��ʾ������
    * @return        ת������ַ���
    */
    static string bin2str(const void *buf, size_t len, const string &sSep = "", size_t lines = 0);

    /**
	* @brief  ����������ת�����ַ���.
	*
    * @param sBinData  ����������
    * @param sSep     �ָ���
    * @param lines    ���ٸ��ֽڻ�һ��, Ĭ��0��ʾ������
    * @return         ת������ַ���
    */
    static string bin2str(const string &sBinData, const string &sSep = "", size_t lines = 0);

    /**
	* @brief   �ַ���ת���ɶ�����.
	*
    * @param psAsciiData �ַ���
    * @param sBinData    ����������
    * @param iBinSize    ��Ҫת�����ַ�������
    * @return            ת�����ȣ�С�ڵ���0���ʾʧ��
    */
    static int str2bin(const char *psAsciiData, unsigned char *sBinData, int iBinSize);

    /**
	 * @brief  �ַ���ת���ɶ�����.
	 *
     * @param sBinData  Ҫת�����ַ���
     * @param sSep      �ָ���
     * @param lines     ���ٸ��ֽڻ�һ��, Ĭ��0��ʾ������
     * @return          ת����Ķ���������
     */
    static string str2bin(const string &sBinData, const string &sSep = "", size_t lines = 0);

    /**
	* @brief  �滻�ַ���.
	*
    * @param sString  �����ַ���
    * @param sSrc     ԭ�ַ���
    * @param sDest    Ŀ���ַ���
    * @return string  �滻����ַ���
    */
    static string replace(const string &sString, const string &sSrc, const string &sDest);

    /**
	* @brief  �����滻�ַ���.
	*
    * @param sString  �����ַ���
    * @param mSrcDest  map<ԭ�ַ���,Ŀ���ַ���>
    * @return string  �滻����ַ���
    */
	static string replace(const string &sString, const map<string,string>& mSrcDest);

    /**
	 * @brief ƥ����.�ָ����ַ�����pat��*�����ͨ���������ǿյ��κ��ַ���
	 * sΪ��, ����false ��patΪ��, ����true
     * @param s    ��ͨ�ַ���
     * @param pat  ��*��ƥ����ַ���������ƥ��ip��ַ
     * @return     �Ƿ�ƥ��ɹ�
     */
    static bool matchPeriod(const string& s, const string& pat);

	 /**
	 * @brief  ƥ����.�ָ����ַ���.
	 *
     * @param s   ��ͨ�ַ���
     * @param pat vector�е�ÿ��Ԫ�ض��Ǵ�*��ƥ����ַ���������ƥ��ip��ַ
     * @return    �Ƿ�ƥ��ɹ�
     */
    static bool matchPeriod(const string& s, const vector<string>& pat);

    /**
	 * @brief  �ж�һ�����Ƿ�Ϊ����.
	 *
	 * @param n  ��Ҫ���жϵ�����
	 * @return   true������������false��ʾ������
     */
    static bool isPrimeNumber(size_t n);

    /**
     * @brief  daemon
     */
    static void daemon();

    /**
     * @brief  ���Թܵ��쳣
     */
    static void ignorePipe();

	/**
	 * @brief  ��һ��string����ת��һ���ֽ� .
     *
	 * @param sWhat Ҫת�����ַ���
     * @return char    ת������ֽ�
     */
    static char x2c(const string &sWhat);

    /**
	 * @brief  ��С�ַ��������ֽ�����֧��K, M, G���� ����: 1K, 3M, 4G, 4.5M, 2.3G
     * @param s            Ҫת�����ַ���
     * @param iDefaultSize ��ʽ����ʱ, ȱʡ�Ĵ�С
     * @return             �ֽ���
     */
    static size_t toSize(const string &s, size_t iDefaultSize);
};

namespace p
{
    template<typename D>
    struct strto1
    {
        D operator()(const string &sStr)
        {
            string s = "0";

            if(!sStr.empty())
            {
                s = sStr;
            }

            istringstream sBuffer(s);

            D t;
            sBuffer >> t;

            return t;
        }
    };

	template<>
    struct strto1<char>
    {
        char operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return sStr[0];
			}
			return 0;
        }
    };

	template<>
    struct strto1<short>
    {
        short operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return atoi(sStr.c_str());
			}
			return 0;
        }
    };

	template<>
    struct strto1<unsigned short>
    {
        unsigned short operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return strtoul(sStr.c_str(), NULL, 10);
			}
			return 0;
        }
    };

	template<>
    struct strto1<int>
    {
        int operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return atoi(sStr.c_str());
			}
			return 0;
        }
    };

	template<>
    struct strto1<unsigned int>
    {
        unsigned int operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return strtoul(sStr.c_str(), NULL, 10);
			}
			return 0;
        }
    };

	template<>
    struct strto1<long>
    {
        long operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return atol(sStr.c_str());
			}
			return 0;
        }
    };

	template<>
    struct strto1<long long>
    {
        long long operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return atoll(sStr.c_str());
			}
			return 0;
        }
    };

	template<>
    struct strto1<unsigned long>
    {
        unsigned long operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return strtoul(sStr.c_str(), NULL, 10);
			}
			return 0;
        }
    };

	template<>
    struct strto1<float>
    {
        float operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return atof(sStr.c_str());
			}
			return 0;
        }
    };

	template<>
    struct strto1<double>
    {
        double operator()(const string &sStr)
        {
            if(!sStr.empty())
			{
				return atof(sStr.c_str());
			}
			return 0;
        }
    };

    template<typename D>
    struct strto2
    {
        D operator()(const string &sStr)
        {
            istringstream sBuffer(sStr);

            D t;
            sBuffer >> t;

            return t;
        }
    };

    template<>
    struct strto2<string>
    {
        string operator()(const string &sStr)
        {
            return sStr;
        }
    };

}

template<typename T>
T TC_Common::strto(const string &sStr)
{
    typedef typename TL::TypeSelect<TL::TypeTraits<T>::isStdArith, p::strto1<T>, p::strto2<T> >::Result strto_type;

    return strto_type()(sStr);
}

template<typename T>
T TC_Common::strto(const string &sStr, const string &sDefault)
{
    string s;

    if(!sStr.empty())
    {
        s = sStr;
    }
    else
    {
        s = sDefault;
    }

    return strto<T>(s);
}


template<typename T>
vector<T> TC_Common::sepstr(const string &sStr, const string &sSep, bool withEmpty)
{
    vector<T> vt;

    string::size_type pos = 0;
    string::size_type pos1 = 0;

    while(true)
    {
        string s;
        pos1 = sStr.find_first_of(sSep, pos);
        if(pos1 == string::npos)
        {
            if(pos + 1 <= sStr.length())
            {
                s = sStr.substr(pos);
            }
        }
        else if(pos1 == pos)
        {
            s = "";
        }
        else
        {
            s = sStr.substr(pos, pos1 - pos);
            pos = pos1;
        }

        if(withEmpty)
        {
            vt.push_back(strto<T>(s));
        }
        else
        {
            if(!s.empty())
            {
                T tmp = strto<T>(s);
                vt.push_back(tmp);
            }
        }

        if(pos1 == string::npos)
        {
            break;
        }

        pos++;
    }

    return vt;
}
template<typename T>
string TC_Common::tostr(const T &t)
{
	ostringstream sBuffer;
	sBuffer << t;
	return sBuffer.str();
}

template<typename T>
string TC_Common::tostr(const vector<T> &t)
{
	string s;
	for(size_t i = 0; i < t.size(); i++)
	{
		s += tostr(t[i]);
		s += " ";
	}
	return s;
}

template<typename K, typename V, typename D, typename A>
string TC_Common::tostr(const map<K, V, D, A> &t)
{
	string sBuffer;
	typename map<K, V, D, A>::const_iterator it = t.begin();
	while(it != t.end())
	{
		sBuffer += " [";
		sBuffer += tostr(it->first);
		sBuffer += "]=[";
		sBuffer += tostr(it->second);
		sBuffer += "] ";
		++it;
	}
	return sBuffer;
}

template<typename K, typename V, typename D, typename A>
string TC_Common::tostr(const multimap<K, V, D, A> &t)
{
	string sBuffer;
	typename multimap<K, V, D, A>::const_iterator it = t.begin();
	while(it != t.end())
	{
		sBuffer += " [";
		sBuffer += tostr(it->first);
		sBuffer += "]=[";
		sBuffer += tostr(it->second);
		sBuffer += "] ";
		++it;
	}
	return sBuffer;
}

template<typename F, typename S>
string TC_Common::tostr(const pair<F, S> &itPair)
{
	string sBuffer;
	sBuffer += "[";
	sBuffer += tostr(itPair.first);
	sBuffer += "]=[";
	sBuffer += tostr(itPair.second);
	sBuffer += "]";
	return sBuffer;
}

template <typename InputIter>
string TC_Common::tostr(InputIter iFirst, InputIter iLast, const string &sSep)
{
	string sBuffer;
	InputIter it = iFirst;

	while(it != iLast)
	{
		sBuffer += tostr(*it);
		++it;

		if(it != iLast)
		{
			sBuffer += sSep;
		}
		else
		{
			break;
		}
	}

	return sBuffer;
}

}
#endif

