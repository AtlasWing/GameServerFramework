#ifndef __TC_CONFIG_H_
#define __TC_CONFIG_H_

#include <map>
#include <list>
#include <stack>
#include <vector>
#include "util/tc_ex.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file  tc_config.h 
 * @brief  �����ļ���ȡ��. 
 *  
 * @author  jarodruan@tencent.com 
 */
/////////////////////////////////////////////////

/**
* ��ָ���
*/
const char TC_CONFIG_DOMAIN_SEP = '/';

/**
* ������ʼ��
*/
const char TC_CONFIG_PARAM_BEGIN = '<';

/**
* ����������
*/
const char TC_CONFIG_PARAM_END = '>';

/**
* @brief �����ļ��쳣��.
*/
struct TC_Config_Exception : public TC_Exception
{
	TC_Config_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Config_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
	~TC_Config_Exception() throw(){};
};

/**
 * @brief ����û���ļ�����
 */
struct TC_ConfigNoParam_Exception : public TC_Exception
{
	TC_ConfigNoParam_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ConfigNoParam_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
	~TC_ConfigNoParam_Exception() throw(){};
};


/** 
* @brief ���������ļ��е������. 
*/
class TC_ConfigDomain
{
public:
    friend class TC_Config;

    /**
	* @brief ���캯��. 
	*  
    * @param sLine  ָ�����ļ��е�һ�У����ж�ȡ
    */
    TC_ConfigDomain(const string &sLine);

    /**
    *  @brief ��������.
    */
    ~TC_ConfigDomain();

    /**
	 * @brief ��������.
	 *  
     * @param tcd һ��TC_ConfigDomain����ָ�����ļ���һ����
     */
    TC_ConfigDomain(const TC_ConfigDomain &tcd);

    /**
	 * @brief ��ֵ. 
	 * 
	 * @param tcd һ��TC_ConfigDomain����ָ�����ļ���һ����
     */
    TC_ConfigDomain& operator=(const TC_ConfigDomain &tcd);


    struct DomainPath
    {
        vector<string>  _domains;
        string          _param;
    };

    /**
	 * @brief ����һ��domain 
	 * ������"/Main/Domain<Name>"��path���н����������Ľ��Ϊһ�� 
	 * DomainPath ���ͣ�����·��_domains��·���ж�Ӧ��������_param.
	 *  
	 * @param path       һ������������ַ������������һ����Ҫ��
	 * @param bWithParam "/Main/Domain<Name>"ʱbWithParamΪture 
	 *  				 "/Main/Domain"ʱbWithParamΪfalse
	 * @return DomainPath һ��DomainPath���󣬽��������е������Ͳ���
     */
    static DomainPath parseDomainName(const string& path, bool bWithParam);

    /**
	* @brief ������������
	* ����һ������name����������֣�����"/Main/Domain" 
	* ������һ��nameΪsubDomain������,���"/Main/Domain/subDomain" 
	*  
	* @param name               ��������� 
	* @return TC_ConfigDomain*  ָ�������ָ�� 
	* @throws TC_Config_Exception 
    */
    TC_ConfigDomain* addSubDomain(const string& name);

    /**
	* @brief �ݹ��������� Sub Domain. 
	*  
	* @param itBegin ������ ��ָ��_domains�Ķ���
	* @param itEnd  ������ ��ָ��_domains�ĵײ�
	* @return TC_ConfigDomain* ָ��Ŀ�������ָ��
    */
    TC_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd);
    const TC_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd) const;

    /**
	* @brief Get Param Value ��ȡ����/ֵ��. 
	*  
    * @param sName ����������
    * @return      �������Ӧ��ֵ
    */
    string getParamValue(const string &sName) const;

    /**
	* @brief Get Param Map ��ȡmap 
	* map�ǲ���/ֵ�ԣ�����������������ֵ��Ӧ�ļ�ֵ��, 
	* eg. SyncThreadNum = 2
	* @return һ�����еĲ��������Ӧ��ֵ��map 
    */
    const map<string, string>& getParamMap() const { return _param; }

    /**
	* @brief Get Domain Map ��ȡmap. 
	*  
    * @return map
    */
    const map<string, TC_ConfigDomain*>& getDomainMap() const { return _subdomain; }

    /**
	* @brief Set Param Value ���ò���/ֵ��. 
	* 
    * @param sLine ��
    * @return 
    */
    void setParamValue(const string &sLine);

    /**
	 * @brief �����������key��û�е���������ӵ���� 
	 *  
     * @param ��������Ͷ�Ӧ������ֵ��map
     */
    void insertParamValue(const map<string, string> &m);

    /**
	* @brief Destroy �ͷ�. 
	*  
    * @return ��
    */
    void destroy();

    /**
    * @brief Get Name ��ȡ������
    * @return ������
    */
    string getName() const;

    /**
	 * @brief ���������� 
	 * 
     * @param ������
     */
    void setName(const string& name);

    /**
     * @brief �����ļ��е�˳���ȡkey
     * 
     * @return vector<string>���key��vector
     */
    vector<string> getKey() const;

    /**
     * @brief �����ļ��е�˳���ȡline
     * 
     * @return vector<string>���key��vector
     */
    vector<string> getLine() const;

    /**
     * @brief �����ļ��е�˳���ȡ��Domain
     * 
     * @return vector<string>��������ֵ�vector
     */
    vector<string> getSubDomain() const;

    /**
	* @brief ת���������ļ����ַ�����ʽ. 
	*  
    * @param i  tab�Ĳ���
	* @return   string���������ַ���
    */
    string tostr(int i) const;

    /**
     * @brief ��¡.
     * 
     * @return TC_ConfigDomain*
     */
    TC_ConfigDomain* clone() const 
    { 
        return new TC_ConfigDomain(*this); 
    }

protected:
    /**
	 * @brief ת��. 
	 *  
	 * @param name 
     * @return string
     */
    static string parse(const string& s);

    /**
	 * @brief ����ת��. 
	 *  
	 * @param s 
     * @return string
     */
    static string reverse_parse(const string &s);

    /**
	* @brief ���ò���/ֵ�� 
	*  
    * @param sName  ����������
    * @param sValue �������ֵ
    * @return ��
    */
    void setParamValue(const string &sName, const string &sValue);

protected:

    /**
    * ������
    */
    string                  _name;

    /**
    * name/value�ԣ�����������ƺ��������ֵ
    */
    map<string, string>     _param;

    /**
     * keyҲ����������Ĳ���˳��
     */
    vector<string>          _key;

    /**
    * ����
    */
    map<string, TC_ConfigDomain*>	    _subdomain;

    /**
     * ��Ĳ���˳��
     */
    vector<string>          _domain;
 
    /**
     * ���е������б�
     */
    vector<string>          _line;
};

/**
 * @brief ���������ļ����ࣨ����wblģʽ��.
 
 * ֧�ִ�string�н��������ļ���
 
 * ֧�����������ļ���
 
 * ���������׳��쳣��
 
 * ����[]��ȡ���ã�������������׳��쳣��
 
 * ����get��ȡ���ã��������򷵻ؿգ�
 
 * ��ȡ�����ļ����̰߳�ȫ�ģ�insert��Ⱥ������̰߳�ȫ��
 
 * ���磺
 
 *  <Main>
 
 *  <Domain>
 
 *      Name = Value
 
 *  </Domain>
 
 *   <Domain1>
 
 *   	Name = Value
 
 *   </Domain1>
 
 *  </Main>
 
 *  ��ȡ����:conf["/Main/Domain<Name>"] ��ȡ��Map:
 
 *   getDomainMap("/Main/Domain", m); m�õ�Name/Value��
 
 * ��ȡ��Vector: getDomainVector("/Main", v); v�õ�Domain�б�
 
   ������������������ֵ��
 
 */
class TC_Config
{
public:

    /**
    * @brief ���캯��
    */
    TC_Config();

    /**
	 * @brief ��������.
	 *  
     * @param tcΪTC_Config����
     */
    TC_Config(const TC_Config &tc);

    /**
	 * @brief ��ֵ. 
	 *  
	 * @param tcd 
     * @return TC_Config&
     */
    TC_Config& operator=(const TC_Config &tc);

    /**
	* @brief �����ļ�. 
	*  
	* ��fileName��Ӧ���ļ�ת��Ϊ�����������parse������н��� 
	* @param sFileName : �ļ����� 
	* @return �� 
	* @throws TC_Config_Exception 
    */
    void parseFile(const string& sFileName);

    /**
	* @brief �����ַ���. 
	*  
	* ��string ������ת��Ϊ�����������parse������н��� 
	* @return void 
	* @throws TC_Config_Exception 
    */
    void parseString(const string& buffer);

    /**
	* @brief ��ȡֵ, ���û�����׳��쳣�� 
	* ��������/Main/Domain<Param>���ַ�������ȡ��/Main/Domain�µ�����������ΪParam��ֵ
    * @param sName ��������,����: /Main/Domain<Param>
	* @return      �������Ӧ��ֵ 
	*  @throws     TC_Config_Exception 
    */
    string operator[](const string &sName);

    /**
	 * @brief ��ȡֵ, ע�����û�в��׳��쳣,���ؿ��ַ���. 
	 * ��������/Main/Domain<Param>���ַ�������ȡ��/Main/Domain�µ�����������ΪParam��ֵ
	 * @param sName ��������,  ����: /Main/Domain<Param>
     * @return     �������Ӧ��ֵ
     */
    string get(const string &sName, const string &sDefault="") const;

    /**
	* @brief GetDomainParamMap��ȡ������Ĳ���ֵ��. 
	*  
    * @param path ������, ���ʶ, ����: /Main/Domain
    * @param m    map<string, string>���ͣ����ڵ�map�б�
    * @return     bool, ����������Ĳ���ֵ��
    */
    bool getDomainMap(const string &path, map<string, string> &m) const;

    /**
	 * @brief ��ȡ������Ĳ���ֵ��,�����򷵻ؿ�map. 
	 *  
	 * @param path ������, ���ʶ, ����: /Main/Domain 
     * @return     map<string,string>,��Ӧ����������в���ֵ��
     */
    map<string, string> getDomainMap(const string &path) const;

    /**
	 * @brief ��ȡ�����������key, �����ļ�����˳�򷵻� 
	 * @param path ������, ���ʶ, ����: /Main/Domain 
     * @return     vector<string>��ĳ����ʶ�µ�����key
     */
    vector<string> getDomainKey(const string &path) const;

    /**
     * @brief ��ȡ����������з�����, �����ļ�����˳�򷵻� 
     *        ������getDomainKeyֻ�ܻ�ȡ��key,�ýӿڶԴ�"="������Ҳ���з���
     * @param path ������, ���ʶ, ����: /Main/Domain 
     * @return     vector<string>��ĳ����ʶ�µ����з�����
     */
    vector<string> getDomainLine(const string &path) const;

    /**
	* @brief getDomainMap ��ȡ�����������. 
	*  
    * @param path  ������, ���ʶ, ����: /Main/Domain
    * @param v     Ҫ��ȡ����������
    * @param       vector<string>�������������
    * @return      �ɹ���ȡ����true�����򷵻�false
    */
    bool getDomainVector(const string &path, vector<string> &v) const;

    /**
	 * @brief ��ȡ�����������, 
	 *  	  �������򷵻ؿ�vector�������ļ��е�˳�򷵻�
	 * @param path ������, ���ʶ, ����: /Main/Domain 
     * @return     vector<string>Ŀ�������������
     */
    vector<string> getDomainVector(const string &path) const;

    /**
	 * @brief �Ƿ������. 
	 *  
	 * @param path ������, ���ʶ, ����: /Main/Domain 
     * @return     ���ڷ���true�����򷵻�false
     */
    bool hasDomainVector(const string &path) const;

	/** 
	 * @brief �������ڵ�ǰ������������, ����Ѿ�����sAddDomain��, 
	 *  	  ����Ϊ�ɹ�
     * @param sCurDomain ���ʶ��, ����:/Main/Domain
     * @param sAddDomain ��Ҫ���ӵ�������: ����: sCurDomain
     * @param bCreate    sCurDomain�򲻴��ڵ������, �Ƿ��Զ�����
	 * @return           0-�ɹ�����, 1-sCurDomain������ 
     */
    int insertDomain(const string &sCurDomain, const string &sAddDomain, bool bCreate);


	/** 
	 * @brief ���Ӳ��������������ǰ�������������������, 
	 *  	  ����Ѿ�����ز���, �����(���滻)
     * @param sCurDomain  ���ʶ��, ����:/Main/Domain
     * @param m           map���ͣ������ֵ��
     * @param bCreate     sCurDomain�򲻴��ڵ������, �Ƿ��Զ�����
	 * @return            0: �ɹ�, 1:sCurDomain������ 
     */
    int insertDomainParam(const string &sCurDomain, const map<string, string> &m, bool bCreate);

    /**
	 * @brief �ϲ������ļ�����ǰ�����ļ�. 
	 *  
     * @param cf
     * @param bUpdate true-��ͻ����±�����, false-��ͻ�����
     */
    void joinConfig(const TC_Config &cf, bool bUpdate);

    /**
	* @brief ת���������ļ����ַ�����ʽ. 
	*  
    * @return �����ַ���
    */
	string tostr() const;

protected:
    /**
	* @brief Parse�������� 
	* ���հ�������������Ϊһ��TC_ConfigDomainװ��stack�� 
	* @param  Ҫ�����������������н��� 
    * @throws TC_Config_Exception
    * @return
    */
    void parse(istream &is);

    /**
	* @brief create New Domain ��������. 
	*  
    * @param sName ������
	* @return      ָ�������ɵ������ָ�� 
	* @throws TC_Config_Exception 
    */
    TC_ConfigDomain *newTcConfigDomain(const string& sName);

    /**
	* @brief Search Domain ������. 
	*  
    * @param sDomainName ��������,֧����������
    * @return value
    */
	TC_ConfigDomain *searchTcConfigDomain(const vector<string>& domains);
	const TC_ConfigDomain *searchTcConfigDomain(const vector<string>& domains) const;

protected:

    /**
    * ��domain
    */
	TC_ConfigDomain _root;
};

}
#endif //_TC_CONFIG_H_
