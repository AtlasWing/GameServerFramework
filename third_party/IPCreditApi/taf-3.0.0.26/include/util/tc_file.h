#ifndef __TC_FILE_H_
#define __TC_FILE_H_

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include "util/tc_ex.h"
#include "util/tc_common.h"

namespace taf
{
/////////////////////////////////////////////////
/** 
 * @file tc_file.h 
 * @brief  �ļ�������. 
 *  
 * @author  jarodruan@tencent.com,skingfan@tencent.com          
 */
/////////////////////////////////////////////////


/**
* @brief �ļ��쳣��. 
*  
*/
struct TC_File_Exception : public TC_Exception
{
    TC_File_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_File_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_File_Exception() throw(){};
};

/**
* @brief �����ļ���������. 
*  
*/
class TC_File
{
public:

    /**
	* @brief ��ȡ�ļ���С, ����ļ�������, �򷵻�0. 
	*  
    * @param  sFullFileName �ļ�ȫ·��(����Ŀ¼���ļ���)
    * @return               ofstream::pos_type�����ļ���С
    */
    static ifstream::pos_type getFileSize(const string &sFullFileName);

    /**
	 * @brief �ж��Ƿ�Ϊ����·��, ���Կո���'/'��ͷ. 
	 *  
	 * @param sFullFileName �ļ�ȫ·��(����Ŀ¼���ļ���)
	 * @return              ture�Ǿ���·����false����Ǿ���·�� 
     */
    static bool isAbsolute(const string &sFullFileName);

    /**
    * @brief �жϸ���·�����ļ��Ƿ����. 
	* ����ļ��Ƿ�������,���Է��������ж϶������Է�������ָ����ļ��ж� 
    * @param sFullFileName �ļ�ȫ·��
    * @param iFileType     �ļ�����, ȱʡS_IFREG
	* @return           true������ڣ�fals��������
    */
    static bool isFileExist(const string &sFullFileName, mode_t iFileType = S_IFREG);

    /**
    * @brief �жϸ���·�����ļ��Ƿ����.  
    * ע��: ����ļ��Ƿ�������,���Է�������ָ����ļ��ж�
    * @param sFullFileName  �ļ�ȫ·��
    * @param iFileType      �ļ�����, ȱʡS_IFREG
    * @return               true-���ڣ�fals-������
	*/ 
    static bool isFileExistEx(const string &sFullFileName, mode_t iFileType = S_IFREG);

    /**
	 * @brief ����Ŀ¼����, ��һЩ���õ�ȥ��, ����./��. 
	 *  
	 * @param path Ŀ¼����
     * @return        �淶���Ŀ¼����
     */
    static string simplifyDirectory(const string& path);

    /**
	* @brief ����Ŀ¼, ���Ŀ¼�Ѿ�����, ��Ҳ���سɹ�. 
	*  
    * @param sFullPath Ҫ������Ŀ¼����
    * @param iFlag     Ȩ��, Ĭ�� S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP| S_IXGRP | S_IROTH |  S_IXOTH
	* @return bool  true-�����ɹ� ��false-����ʧ�� 
    */
    static bool makeDir(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);

    /**
	 *@brief ѭ������Ŀ¼, ���Ŀ¼�Ѿ�����, ��Ҳ���سɹ�. 
	 * 
     * @param sFullPath Ҫ������Ŀ¼����
	 * @param iFlag   Ȩ��, Ĭ�� S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH
     * @return           true-�����ɹ���false-����ʧ�� 
     */

    static bool makeDirRecursive(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);

    /**
	 * @brief �����ļ��Ƿ��ִ��. 
	 *  
     * @param sFullFileName �ļ�ȫ·��
	 * @param canExecutable true��ʾ��ִ��, false������֮�� 
     * @return                 �ɹ�����0, ����ʧ��
     */
    static int setExecutable(const string &sFullFileName, bool canExecutable);

    /**
	 * @brief �ж��ļ��Ƿ��ִ��. 
	 *  
	 * @param sFullFileName �ļ�ȫ·��
     * @return                 true-��ִ��, false-����ִ�� 
     */
    static bool canExecutable(const string &sFullFileName);

    /**
	 * @brief ɾ��һ���ļ���Ŀ¼. 
	 *  
     * @param sFullFileName �ļ�����Ŀ¼��ȫ·��
     * @param bRecursive    �����Ŀ¼�Ƿ�ݹ�ɾ��
     * @return              0-�ɹ���ʧ�ܿ���ͨ��errno�鿴ʧ�ܵ�ԭ��
     */
    static int removeFile(const string &sFullFileName, bool bRecursive);

    /**
	* @brief ��ȡ�ļ���string  
	* �ļ������򷵻��ļ����ݣ������ڻ��߶�ȡ�ļ������ʱ��, ����Ϊ��
    * @param sFullFileName �ļ�����
    * @return              �ļ�����
    */
    static string load2str(const string &sFullFileName);

    /**
	* @brief д�ļ�. 
	*  
    * @param sFullFileName �ļ�����
    * @param sFileData     �ļ�����
    * @return 
    */
    static void save2file(const string &sFullFileName, const string &sFileData);

    /**
	 * @brief д�ļ�. 
	 *  
     * @param sFullFileName  �ļ���
     * @param sFileData      ����ָ��
	 * @param length      д�볤�� 
     * @return               0-�ɹ�,-1-ʧ��
     */
    static int save2file(const string &sFullFileName, const char *sFileData, size_t length);

    /**
     * @brief ��ȡǰ����ִ���ļ�·��.
     *
     * @return string ��ִ���ļ���·��ȫ����
     */
    static string getExePath();

    /**
	* @brief ��ȡ�ļ�����
       *��һ����ȫ�ļ�����ȥ��·��������:/usr/local/temp.gif��ȡtemp.gif
    *@param sFullFileName  �ļ�����ȫ����
    *@return string        ��ȡ����ļ�����
    */
    static string extractFileName(const string &sFullFileName);

    /**
    * @brief ��һ����ȫ�ļ�������ȡ�ļ���·��.
	*  
    * ����1: "/usr/local/temp.gif" ��ȡ"/usr/local/"
    * ����2: "temp.gif" ��ȡ "./"
    * @param sFullFileName �ļ�����ȫ����
    * @return              ��ȡ����ļ�·��
    */
    static string extractFilePath(const string &sFullFileName);

    /**
    * @brief ��ȡ�ļ���չ��.
	* 
    * ����1: "/usr/local/temp.gif" ��ȡ"gif"
    * ����2: "temp.gif" ��ȡ"gif"
    *@param sFullFileName �ļ�����
    *@return              �ļ���չ��
    */
    static string extractFileExt(const string &sFullFileName);

    /**
    * @brief ��ȡ�ļ�����,ȥ����չ��.  
    * ����1: "/usr/local/temp.gif" ��ȡ"/usr/local/temp"
    * ����2: "temp.gif" ��ȡ"temp"
    * @param sFullFileName �ļ�����
    * @return              ȥ����չ�����ļ�����
    */
    static string excludeFileExt(const string &sFullFileName);

    /**
	* @brief �滻�ļ���չ�� 
	*  
	* �ı��ļ����ͣ��������չ��,�������չ�� =?1: 
	* ����1��"/usr/temp.gif" �� �� "jpg" �õ�"/usr/temp.jpg" 
	* ����2: "/usr/local/temp" �� �� "jpg" �õ�"/usr/local/temp.jpg"
    * @param sFullFileName �ļ�����
    * @param sExt          ��չ��
    * @return              �滻��չ������ļ���
    */
    static string replaceFileExt(const string &sFullFileName, const string &sExt);

    /**
    * @brief ��һ��url�л�ȡ��ȫ�ļ���.
	*  
    * ��ȡ��http://��,��һ��'/'����������ַ�
    * ����1:http://www.qq.com/tmp/temp.gif ��ȡtmp/temp.gif
    * ����2:www.qq.com/tmp/temp.gif ��ȡtmp/temp.gif
    * ����3:/tmp/temp.gif ��ȡtmp/temp.gif
    * @param sUrl url�ַ���
    * @return     �ļ�����
    */
    static string extractUrlFilePath(const string &sUrl);

    /**
    * @brief �����ļ�ʱȷ���Ƿ�ѡ��.
	*  
    * @return 1-ѡ��, 0-��ѡ��
    */
    typedef int (*FILE_SELECT)(const dirent *);

    /**
	* @brief ɨ��һ��Ŀ¼. 
	*  
    * @param sFilePath     ��Ҫɨ���·��
    * @param vtMatchFiles  ���ص��ļ���ʸ����
    * @param f             ƥ�亯��,ΪNULL��ʾ�����ļ�����ȡ
    * @param iMaxSize      ����ļ�����,iMaxSize <=0ʱ,��������ƥ���ļ�
    * @return              �ļ�����
    */
    static size_t scanDir(const string &sFilePath, vector<string> &vtMatchFiles, FILE_SELECT f = NULL, int iMaxSize = 0);

    /**
	 * @brief ����Ŀ¼, ��ȡĿ¼����������ļ�����Ŀ¼. 
	 *  
     * @param path       ��Ҫ������·��
     * @param files      Ŀ��·�����������ļ�
     * @param bRecursive �Ƿ�ݹ���Ŀ¼
     *
     **/
    static void listDirectory(const string &path, vector<string> &files, bool bRecursive);
    
     /**
	 * @brief �����ļ���Ŀ¼.   
	 * ���ļ�����Ŀ¼��sExistFile���Ƶ�sNewFile  
     * @param sExistFile ���Ƶ��ļ�����Ŀ¼Դ·��
     * @param sNewFile   ���Ƶ��ļ�����Ŀ¼Ŀ��·��
     * @param bRemove    �Ƿ���ɾ��sNewFile��copy ����ֹTextfile busy���¸���ʧ�� 
     * @return 
     */
    static void copyFile(const string &sExistFile, const string &sNewFile,bool bRemove = false);
};

}
#endif // TC_FILE_H
