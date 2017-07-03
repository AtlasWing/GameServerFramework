// Test Align for updating !!!!!!!!!!!!!!!!!!

#include "StdAfx.h"

// #include <locale.h>
#include <initguid.h>

#include "Plugin.h"

#include "Common/Wildcard.h"
#include "Common/DynamicBuffer.h"
#include "Common/StringConvert.h"
#include "Common/Defs.h"

#include "Windows/FileFind.h"
#include "Windows/FileIO.h"
#include "Windows/FileDir.h"
#include "Windows/Defs.h"

#include "../../IPassword.h"
#include "../../Common/FileStreams.h"

#include "../Common/DefaultName.h"
#include "../Common/OpenArchive.h"
#include "../Agent/Agent.h"

#include "ProgressBox.h"
#include "FarUtils.h"
#include "Messages.h"

using namespace NWindows;
using namespace NFar;

static const char *kCommandPrefix = "7-zip";

static const char *kRegisrtryMainKeyName = "";

static const char *kRegisrtryValueNameEnabled = "UsedByDefault3";
static bool kPluginEnabledDefault = true;

static const char *kHelpTopicConfig =  "Config";

extern "C"
{
  void WINAPI SetStartupInfo(struct PluginStartupInfo *info);
  HANDLE WINAPI OpenFilePlugin(char *name, const unsigned char *Data,
      unsigned int DataSize);
  HANDLE WINAPI OpenPlugin(int openFrom, int item);
  void WINAPI ClosePlugin(HANDLE plugin);
  int WINAPI GetFindData(HANDLE plugin, struct PluginPanelItem **panelItems,
      int *itemsNumber, int OpMode);
  void WINAPI FreeFindData(HANDLE plugin, struct PluginPanelItem *panelItems,
    int itemsNumber);
  int WINAPI GetFiles(HANDLE plugin, struct PluginPanelItem *panelItems,
    int itemsNumber, int move, char *destPath, int opMode);
  int WINAPI SetDirectory(HANDLE plugin, char *dir, int opMode);
  void WINAPI GetPluginInfo(struct PluginInfo *info);
  int WINAPI Configure(int itemNumber);
  void WINAPI GetOpenPluginInfo(HANDLE plugin, struct OpenPluginInfo *info);
  int WINAPI PutFiles(HANDLE plugin, struct PluginPanelItem *panelItems,
      int itemsNumber, int move, int opMode);
  int WINAPI DeleteFiles(HANDLE plugin, PluginPanelItem *panelItems,
      int itemsNumber, int opMode);
  int WINAPI ProcessKey(HANDLE plugin, int key, unsigned int controlState);
};

HINSTANCE g_hInstance;
#ifndef _UNICODE
bool g_IsNT = false;
static bool IsItWindowsNT()
{
  OSVERSIONINFO versionInfo;
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (!::GetVersionEx(&versionInfo))
    return false;
  return (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT);
}
#endif

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
  if (dwReason == DLL_PROCESS_ATTACH)
  {
    g_hInstance = hInstance;
    #ifndef _UNICODE
    g_IsNT = IsItWindowsNT();
    #endif
  }
  return TRUE;
}

static struct COptions
{
  bool Enabled;
} g_Options;

static const char *kPliginNameForRegestry = "7-ZIP";

void WINAPI SetStartupInfo(struct PluginStartupInfo *info)
{
  MY_TRY_BEGIN;
  g_StartupInfo.Init(*info, kPliginNameForRegestry);
  g_Options.Enabled = g_StartupInfo.QueryRegKeyValue(
      HKEY_CURRENT_USER, kRegisrtryMainKeyName,
      kRegisrtryValueNameEnabled, kPluginEnabledDefault);
  MY_TRY_END1("SetStartupInfo");
}

class COpenArchiveCallback:
  public IArchiveOpenCallback,
  public IArchiveOpenVolumeCallback,
  public IProgress,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
  DWORD m_StartTickValue;
  bool m_MessageBoxIsShown;

  CProgressBox _progressBox;

  UInt64 _numFilesTotal;
  UInt64 _numFilesCur;
  UInt64 _numBytesTotal;
  UInt64 _numBytesCur;

  bool _numFilesTotalDefined;
  bool _numFilesCurDefined;
  bool _numBytesTotalDefined;
  bool _numBytesCurDefined;

  DWORD m_PrevTickCount;

  NWindows::NFile::NFind::CFileInfoW _fileInfo;
public:
  bool PasswordIsDefined;
  UString Password;

  UString _folderPrefix;

public:
  MY_UNKNOWN_IMP3(
     IArchiveOpenVolumeCallback,
     IProgress,
     ICryptoGetTextPassword
    )

  // IProgress
  STDMETHOD(SetTotal)(UInt64 total);
  STDMETHOD(SetCompleted)(const UInt64 *aCompleteValue);

  // IArchiveOpenCallback
  STDMETHOD(SetTotal)(const UInt64 *numFiles, const UInt64 *numBytes);
  STDMETHOD(SetCompleted)(const UInt64 *numFiles, const UInt64 *numBytes);

  // IArchiveOpenVolumeCallback
  STDMETHOD(GetProperty)(PROPID propID, PROPVARIANT *value);
  STDMETHOD(GetStream)(const wchar_t *name, IInStream **inStream);

  // ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

  void Init()
  {
    PasswordIsDefined = false;

    _numFilesTotalDefined = false;
    _numFilesCurDefined = false;
    _numBytesTotalDefined = false;
    _numBytesCurDefined = false;

    m_MessageBoxIsShown = false;
    m_PrevTickCount = GetTickCount();
  }
  void ShowMessage();

  void LoadFileInfo(const UString &folderPrefix,
      const UString &fileName)
  {
    _folderPrefix = folderPrefix;
    if (!NWindows::NFile::NFind::FindFile(_folderPrefix + fileName, _fileInfo))
      throw 1;
  }
};

void COpenArchiveCallback::ShowMessage()
{
  DWORD currentTime = GetTickCount();
  if (!m_MessageBoxIsShown)
  {
    if (currentTime - m_PrevTickCount < 100)
      return;
    _progressBox.Init(
        // g_StartupInfo.GetMsgString(NMessageID::kWaitTitle),
        g_StartupInfo.GetMsgString(NMessageID::kReading), 48);

    m_MessageBoxIsShown = true;
  }
  else
  {
    if (currentTime - m_PrevTickCount < 200)
      return;
  }
  m_PrevTickCount = currentTime;

  UInt64 total = 0, cur = 0;
  bool curIsDefined = false, totalIsDefined = false;

  char message[256] = { 0 };
  if (_numFilesCurDefined)
    ConvertUInt64ToStringAligned(_numFilesCur, message, 5);

  if (_numFilesTotalDefined)
  {
    strcat(message, " / ");
    ConvertUInt64ToStringAligned(_numFilesTotal, message + strlen(message), 5);
    total = _numFilesTotal;
    totalIsDefined = true;
    if (_numFilesCurDefined)
    {
      cur = _numFilesCur;
      curIsDefined = true;
    }
  }
  else if (_numBytesTotalDefined)
  {
    total = _numBytesTotal;
    totalIsDefined = true;
    if (_numBytesCurDefined)
    {
      cur = _numBytesCur;
      curIsDefined = true;
    }
  }
  _progressBox.Progress(
      totalIsDefined ? &total: NULL,
      curIsDefined ? &cur: NULL,
      message);
}

STDMETHODIMP COpenArchiveCallback::SetTotal(const UInt64 *numFiles, const UInt64 *numBytes)
{
  if (WasEscPressed())
    return E_ABORT;
  
  _numFilesTotalDefined = (numFiles != NULL);
  if (_numFilesTotalDefined)
    _numFilesTotal = *numFiles;

  _numBytesTotalDefined = (numBytes != NULL);
  if (_numBytesTotalDefined)
    _numBytesTotal = *numBytes;

  return S_OK;
}

STDMETHODIMP COpenArchiveCallback::SetCompleted(const UInt64 *numFiles, const UInt64 *numBytes)
{
  if (WasEscPressed())
    return E_ABORT;

  _numFilesCurDefined = (numFiles != NULL);
  if (_numFilesCurDefined)
    _numFilesCur = *numFiles;

  _numBytesCurDefined = (numBytes != NULL);
  if (_numBytesCurDefined)
    _numBytesCur = *numBytes;

  // if (*numFiles % 100 != 0)
  //   return S_OK;
  ShowMessage();
  return S_OK;
}


STDMETHODIMP COpenArchiveCallback::SetTotal(const UInt64 /* total */)
{
  if (WasEscPressed())
    return E_ABORT;
  return S_OK;
}

STDMETHODIMP COpenArchiveCallback::SetCompleted(const UInt64 *completed)
{
  if (WasEscPressed())
    return E_ABORT;
  if (completed == NULL)
    return S_OK;
  ShowMessage();
  return S_OK;
}

STDMETHODIMP COpenArchiveCallback::GetStream(const wchar_t *name,
    IInStream **inStream)
{
  if (WasEscPressed())
    return E_ABORT;
  *inStream = NULL;
  UString fullPath = _folderPrefix + name;
  if (!NWindows::NFile::NFind::FindFile(fullPath, _fileInfo))
    return S_FALSE;
  if (_fileInfo.IsDir())
    return S_FALSE;
  CInFileStream *inFile = new CInFileStream;
  CMyComPtr<IInStream> inStreamTemp = inFile;
  if (!inFile->Open(fullPath))
    return ::GetLastError();
  *inStream = inStreamTemp.Detach();
  return S_OK;
}


STDMETHODIMP COpenArchiveCallback::GetProperty(PROPID propID, PROPVARIANT *value)
{
  NWindows::NCOM::CPropVariant prop;
  switch(propID)
  {
    case kpidName:  prop = GetUnicodeString(_fileInfo.Name, CP_OEMCP); break;
    case kpidIsDir:  prop = _fileInfo.IsDir(); break;
    case kpidSize:  prop = _fileInfo.Size; break;
    case kpidAttrib:  prop = (UInt32)_fileInfo.Attrib; break;
    case kpidCTime:  prop = _fileInfo.CTime; break;
    case kpidATime:  prop = _fileInfo.ATime; break;
    case kpidMTime:  prop = _fileInfo.MTime; break;
  }
  prop.Detach(value);
  return S_OK;
}

HRESULT GetPassword(UString &password)
{
  if (WasEscPressed())
    return E_ABORT;
  password.Empty();
  CInitDialogItem initItems[]=
  {
    { DI_DOUBLEBOX, 3, 1, 72, 4, false, false, 0, false,  NMessageID::kGetPasswordTitle, NULL, NULL },
    { DI_TEXT, 5, 2, 0, 0, false, false, DIF_SHOWAMPERSAND, false, NMessageID::kEnterPasswordForFile, NULL, NULL },
    { DI_PSWEDIT, 5, 3, 70, 3, true, false, 0, true, -1, "", NULL }
  };
  
  const int kNumItems = sizeof(initItems)/sizeof(initItems[0]);
  FarDialogItem dialogItems[kNumItems];
  g_StartupInfo.InitDialogItems(initItems, dialogItems, kNumItems);
  
  // sprintf(DialogItems[1].Data,GetMsg(MGetPasswordForFile),FileName);
  if (g_StartupInfo.ShowDialog(76, 6, NULL, dialogItems, kNumItems) < 0)
    return (E_ABORT);

  AString oemPassword = dialogItems[2].Data;
  password = MultiByteToUnicodeString(oemPassword, CP_OEMCP);
  return S_OK;
}

STDMETHODIMP COpenArchiveCallback::CryptoGetTextPassword(BSTR *password)
{
  if (!PasswordIsDefined)
  {
    RINOK(GetPassword(Password));
    PasswordIsDefined = true;
  }
  return StringToBstr(Password, password);
}

/*
HRESULT OpenArchive(const CSysString &fileName,
    IInFolderArchive **archiveHandlerResult,
    CArchiverInfo &archiverInfoResult,
    UString &defaultName,
    IArchiveOpenCallback *openArchiveCallback)
{
  HRESULT OpenArchive(const CSysString &fileName,
    IInArchive **archive,
    CArchiverInfo &archiverInfoResult,
    IArchiveOpenCallback *openArchiveCallback);
}
*/

static HANDLE MyOpenFilePlugin(const char *name)
{
  UINT codePage = ::AreFileApisANSI() ? CP_ACP : CP_OEMCP;
 
  UString normalizedName = GetUnicodeString(name, codePage);
  normalizedName.Trim();
  UString fullName;
  int fileNamePartStartIndex;
  NFile::NDirectory::MyGetFullPathName(normalizedName, fullName, fileNamePartStartIndex);
  NFile::NFind::CFileInfoW fileInfo;
  if (!NFile::NFind::FindFile(fullName, fileInfo))
    return INVALID_HANDLE_VALUE;
  if (fileInfo.IsDir())
     return INVALID_HANDLE_VALUE;


  CMyComPtr<IInFolderArchive> archiveHandler;

  // CArchiverInfo archiverInfoResult;
  // ::OutputDebugString("before OpenArchive\n");
  
  CScreenRestorer screenRestorer;
  {
    screenRestorer.Save();
  }

  COpenArchiveCallback *openArchiveCallbackSpec = new COpenArchiveCallback;
  CMyComPtr<IArchiveOpenCallback> openArchiveCallback = openArchiveCallbackSpec;

  // if ((opMode & OPM_SILENT) == 0 && (opMode & OPM_FIND ) == 0)
  openArchiveCallbackSpec->Init();
  openArchiveCallbackSpec->LoadFileInfo(
      fullName.Left(fileNamePartStartIndex),
      fullName.Mid(fileNamePartStartIndex));
  
  // ::OutputDebugString("before OpenArchive\n");
  
  archiveHandler = new CAgent;
  CMyComBSTR archiveType;
  HRESULT result = archiveHandler->Open(
      GetUnicodeString(fullName, CP_OEMCP), &archiveType, openArchiveCallback);
  /*
  HRESULT result = ::OpenArchive(fullName, &archiveHandler,
      archiverInfoResult, defaultName, openArchiveCallback);
  */
  if (result != S_OK)
  {
    if (result == E_ABORT)
      return (HANDLE)-2;
    return INVALID_HANDLE_VALUE;
  }

  // ::OutputDebugString("after OpenArchive\n");

  CPlugin *plugin = new CPlugin(
      fullName,
      // defaultName,
      archiveHandler,
      (const wchar_t *)archiveType
      );
  if (plugin == NULL)
    return(INVALID_HANDLE_VALUE);
  plugin->PasswordIsDefined = openArchiveCallbackSpec->PasswordIsDefined;
  plugin->Password = openArchiveCallbackSpec->Password;

  return (HANDLE)(plugin);
}

HANDLE WINAPI OpenFilePlugin(char *name,
    const unsigned char * /* data */, unsigned int /* dataSize */)
{
  MY_TRY_BEGIN;
  if (name == NULL || (!g_Options.Enabled))
  {
    // if (!Opt.ProcessShiftF1)
      return(INVALID_HANDLE_VALUE);
  }
  return MyOpenFilePlugin(name);
  MY_TRY_END2("OpenFilePlugin", INVALID_HANDLE_VALUE);
}

HANDLE WINAPI OpenPlugin(int openFrom, int item)
{
  MY_TRY_BEGIN;
  if(openFrom == OPEN_COMMANDLINE)
  {
    CSysString fileName = (const char *)(UINT_PTR)item;
    if(fileName.IsEmpty())
      return INVALID_HANDLE_VALUE;
    if (fileName.Length() >= 2 &&
        fileName[0] == '\"' && fileName[fileName.Length() - 1] == '\"')
      fileName = fileName.Mid(1, fileName.Length() - 2);

    return MyOpenFilePlugin(fileName);
  }
  if(openFrom == OPEN_PLUGINSMENU)
  {
    switch(item)
    {
      case 0:
      {
        PluginPanelItem pluginPanelItem;
        if(!g_StartupInfo.ControlGetActivePanelCurrentItemInfo(pluginPanelItem))
          throw 142134;
        return MyOpenFilePlugin(pluginPanelItem.FindData.cFileName);
      }
      case 1:
      {
        CObjectVector<PluginPanelItem> pluginPanelItem;
        if(!g_StartupInfo.ControlGetActivePanelSelectedOrCurrentItems(pluginPanelItem))
          throw 142134;
        if (CompressFiles(pluginPanelItem) == S_OK)
        {
          /* int t = */ g_StartupInfo.ControlClearPanelSelection();
          g_StartupInfo.ControlRequestActivePanel(FCTL_UPDATEPANEL, NULL);
          g_StartupInfo.ControlRequestActivePanel(FCTL_REDRAWPANEL, NULL);
          g_StartupInfo.ControlRequestActivePanel(FCTL_UPDATEANOTHERPANEL, NULL);
          g_StartupInfo.ControlRequestActivePanel(FCTL_REDRAWANOTHERPANEL, NULL);
        }
        return INVALID_HANDLE_VALUE;
      }
      default:
        throw 4282215;
    }
  }
  return INVALID_HANDLE_VALUE;
  MY_TRY_END2("OpenPlugin", INVALID_HANDLE_VALUE);
}

void WINAPI ClosePlugin(HANDLE plugin)
{
  MY_TRY_BEGIN;
  delete (CPlugin *)plugin;
  MY_TRY_END1("ClosePlugin");
}

int WINAPI GetFindData(HANDLE plugin, struct PluginPanelItem **panelItems,
    int *itemsNumber,int opMode)
{
  MY_TRY_BEGIN;
  return(((CPlugin *)plugin)->GetFindData(panelItems, itemsNumber, opMode));
  MY_TRY_END2("GetFindData", FALSE);
}

void WINAPI FreeFindData(HANDLE plugin, struct PluginPanelItem *panelItems,
    int itemsNumber)
{
  MY_TRY_BEGIN;
  ((CPlugin *)plugin)->FreeFindData(panelItems, itemsNumber);
  MY_TRY_END1("FreeFindData");
}

int WINAPI GetFiles(HANDLE plugin, struct PluginPanelItem *panelItems,
    int itemsNumber, int move, char *destPath, int opMode)
{
  MY_TRY_BEGIN;
  return(((CPlugin *)plugin)->GetFiles(panelItems, itemsNumber, move, destPath, opMode));
  MY_TRY_END2("GetFiles", NFileOperationReturnCode::kError);
}

int WINAPI SetDirectory(HANDLE plugin, char *dir, int opMode)
{
  MY_TRY_BEGIN;
  return(((CPlugin *)plugin)->SetDirectory(dir, opMode));
  MY_TRY_END2("SetDirectory", FALSE);
}

void WINAPI GetPluginInfo(struct PluginInfo *info)
{
  MY_TRY_BEGIN;

  info->StructSize = sizeof(*info);
  info->Flags = 0;
  info->DiskMenuStrings = NULL;
  info->DiskMenuNumbers = NULL;
  info->DiskMenuStringsNumber = 0;
  static const char *pluginMenuStrings[2];
  pluginMenuStrings[0] = g_StartupInfo.GetMsgString(NMessageID::kOpenArchiveMenuString);
  pluginMenuStrings[1] = g_StartupInfo.GetMsgString(NMessageID::kCreateArchiveMenuString);
  info->PluginMenuStrings = (char **)pluginMenuStrings;
  info->PluginMenuStringsNumber = 2;
  static const char *pluginCfgStrings[1];
  pluginCfgStrings[0] = g_StartupInfo.GetMsgString(NMessageID::kOpenArchiveMenuString);
  info->PluginConfigStrings = (char **)pluginCfgStrings;
  info->PluginConfigStringsNumber = sizeof(pluginCfgStrings) / sizeof(pluginCfgStrings[0]);
  info->CommandPrefix = (char *)kCommandPrefix;
  MY_TRY_END1("GetPluginInfo");
}

int WINAPI Configure(int /* itemNumber */)
{
  MY_TRY_BEGIN;

  const int kEnabledCheckBoxIndex = 1;

  const int kYSize = 7;

  struct CInitDialogItem initItems[]=
  {
    { DI_DOUBLEBOX, 3, 1, 72, kYSize - 2, false, false, 0, false, NMessageID::kConfigTitle, NULL, NULL },
    { DI_CHECKBOX, 5, 2, 0, 0, true, g_Options.Enabled, 0, false, NMessageID::kConfigPluginEnabled, NULL, NULL },
    { DI_TEXT, 5, 3, 0, 0, false, false, DIF_BOXCOLOR | DIF_SEPARATOR, false, -1, "", NULL },
    { DI_BUTTON, 0, kYSize - 3, 0, 0, false, false, DIF_CENTERGROUP, true, NMessageID::kOk, NULL, NULL },
    { DI_BUTTON, 0, kYSize - 3, 0, 0, false, false, DIF_CENTERGROUP, false, NMessageID::kCancel, NULL, NULL },
  };

  const int kNumDialogItems = sizeof(initItems) / sizeof(initItems[0]);
  const int kOkButtonIndex = kNumDialogItems - 2;

  FarDialogItem dialogItems[kNumDialogItems];
  g_StartupInfo.InitDialogItems(initItems, dialogItems, kNumDialogItems);

  int askCode = g_StartupInfo.ShowDialog(76, kYSize,
      kHelpTopicConfig, dialogItems, kNumDialogItems);

  if (askCode != kOkButtonIndex)
    return (FALSE);

  g_Options.Enabled = BOOLToBool(dialogItems[kEnabledCheckBoxIndex].Selected);

  g_StartupInfo.SetRegKeyValue(HKEY_CURRENT_USER, kRegisrtryMainKeyName,
      kRegisrtryValueNameEnabled, g_Options.Enabled);
  return(TRUE);
  MY_TRY_END2("Configure", FALSE);
}

void WINAPI GetOpenPluginInfo(HANDLE plugin,struct OpenPluginInfo *info)
{
  MY_TRY_BEGIN;
  ((CPlugin *)plugin)->GetOpenPluginInfo(info);
  MY_TRY_END1("GetOpenPluginInfo");
}

int WINAPI PutFiles(HANDLE plugin, struct PluginPanelItem *panelItems,
                   int itemsNumber, int move, int opMode)
{
  MY_TRY_BEGIN;
  return(((CPlugin *)plugin)->PutFiles(panelItems, itemsNumber, move, opMode));
  MY_TRY_END2("PutFiles", NFileOperationReturnCode::kError);
}

int WINAPI DeleteFiles(HANDLE plugin, PluginPanelItem *panelItems,
    int itemsNumber, int opMode)
{
  MY_TRY_BEGIN;
  return(((CPlugin *)plugin)->DeleteFiles(panelItems, itemsNumber, opMode));
  MY_TRY_END2("DeleteFiles", FALSE);
}

int WINAPI ProcessKey(HANDLE plugin, int key, unsigned int controlState)
{
  MY_TRY_BEGIN;
  return (((CPlugin *)plugin)->ProcessKey(key, controlState));
  MY_TRY_END2("ProcessKey", FALSE);
}
