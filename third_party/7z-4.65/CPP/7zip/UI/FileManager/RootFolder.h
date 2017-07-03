// RootFolder.h

#ifndef __ROOTFOLDER_H
#define __ROOTFOLDER_H

#include "Common/MyString.h"

#include "Windows/PropVariant.h"

#include "FSFolder.h"

const int kNumRootFolderItems = 3;

class CRootFolder:
  public IFolderFolder,
  public IFolderGetSystemIconIndex,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP1(
    IFolderGetSystemIconIndex
  )

  INTERFACE_FolderFolder(;)

  STDMETHOD(GetSystemIconIndex)(UInt32 index, INT32 *iconIndex);

  void Init();
private:
  UString _names[kNumRootFolderItems];
  int _iconIndices[kNumRootFolderItems];
};

#endif
