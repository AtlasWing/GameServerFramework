#ifndef __TAF_PATCH_H_
#define __TAF_PATCH_H_

#include "patch/Patch.h"

namespace taf
{
/**
 * patch����ͻ��˿�
 * ���øÿ�ʵ�ִ�patch����ͬ���ļ�������
 */

/////////////////////////////////////////////////////////////////////////////////////
/**
 * �����쳣
 */
struct TafPatchException : public TC_Exception
{
	TafPatchException(const string &buffer) : TC_Exception(buffer){};
    TafPatchException(const string &buffer, int err) : TC_Exception(buffer, err){};
	~TafPatchException() throw(){};
};

/**
 * ����֪ͨ�ӿ�
 */
class TafPatchNotifyInterface : public TC_HandleBase
{
public:
    /**
     * ɾ������Ŀ¼
     * @param sDir
     */
    virtual void onRemoveLocalDirectory(const string &sDir) = 0;

    /**
     * ɾ������Ŀ¼�ɹ�
     * @param sDir
     */
    virtual void onRemoveLocalDirectoryOK(const string &sDir) = 0;

    /**
     * ��ʼ��ȡ�ļ�����
     */
    virtual void onListFile() = 0;

    /**
     * ��ȡ�ļ�����ɹ�
     * @param vf
     */
    virtual void onListFileOK(const vector<FileInfo> &vf) = 0;

    /**
     * ��ʼ����
     * @param fi
     */
    virtual void onDownload(const FileInfo &fi) = 0;

    /**
     * �����ļ��ɹ�
     * @param fi
     * @param pos
     * @param dest, ���汾��·��
     */
    virtual void onDownloading(const FileInfo &fi, size_t pos, const string &dest) = 0;

    /**
     * �����ļ��ɹ�
     * @param fi
     * @param dest, ���汾��·��
     */
    virtual void onDownloadOK(const FileInfo &fi, const string &dest) = 0;

    /**
     * ���ÿ�ִ��
     * @param fi
     */
    virtual void onSetExecutable(const FileInfo &fi) = 0;

    /**
     * �����ļ��б��ļ����سɹ�
     * @param vf
     */
    virtual void onDownloadAllOK(const vector<FileInfo> &vf, time_t timeBegin, time_t timeEnd) = 0;

	/**
     * �ϱ������ļ����е�ʱ��
     * @param file, beginTime, endTime
     */
	virtual void onReportTime(const string & sFile, const time_t timeBegin, const time_t timeEnd) = 0;
};

typedef TC_AutoPtr<TafPatchNotifyInterface> TafPatchNotifyInterfacePtr;

/**
 * ���ز�����
 */
class TafPatch
{
public:

    /**
     * ���캯��
     */
    TafPatch();

    /**
     * ��ʼ��
     * @param _patchPrx, patch��������ַ
     * @param remote_dir, Զ��ͬ��Ŀ¼, ������..����
     * @param local_dir, ͬ�������ص�Ŀ¼
     */
    void init(const PatchPrx &patchPrx, const string &remote_dir, const string &local_dir, const bool bCheck = true);

    /**
     * �����Ƿ�ɾ������Ŀ¼
     * @param bRemove
     */
    void setRemove(bool bRemove);

    /**
     * ����, ʧ���׳��쳣
     *
     */
    void download(const TafPatchNotifyInterfacePtr &pPtr);


	/**
	 * ���ؾ���·�����ļ�
	 * 
	 * @author kevintian (2010-8-6)
	 * 
	 * @param pPtr 
	 */
	void downloadSubborn(const TafPatchNotifyInterfacePtr &pPtr);


protected:

    /**
     * ����ĳһ���ļ�
     * @param fi
     */
    void download(bool bDir, const FileInfo &fi, const TafPatchNotifyInterfacePtr &pPtr);


	/**
	 * ���ؾ���·���µ�һ���ļ�
	 * 
	 * @author kevintian (2010-8-6)
	 * 
	 * @param sTmpPth 
	 * @param bDir 
	 * @param fi 
	 * @param pPtr 
	 */
	void downloadSubborn(const string & sTmpPth, bool bDir, const FileInfo & fi, const TafPatchNotifyInterfacePtr & pPtr);

    /**
     * ��鱾��·��
     */
    void checkLocalDir();

protected:

    /**
     * Զ�̵�ַ
     */
    string          _remote_dir;

    /**
     * ���ص�ַ
     */
    string          _local_dir;

    /**
     * �Ƿ�ɾ������ͬ��Ŀ¼
     */
    bool            _bRemove;

    /**
     * patch������
     */
    PatchPrx        _patchPrx;
};

}

#endif


