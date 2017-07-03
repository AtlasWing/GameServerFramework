# Microsoft Developer Studio Project File - Name="hgehelp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=hgehelp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hgehelp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hgehelp.mak" CFG="hgehelp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hgehelp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "hgehelp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hgehelp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../lib/vc"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /Op /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /QIfist /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "hgehelp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../lib/vc"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /QIfist /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "hgehelp - Win32 Release"
# Name "hgehelp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\hgeanim.cpp
# End Source File
# Begin Source File

SOURCE=.\hgecolor.cpp
# End Source File
# Begin Source File

SOURCE=.\hgedistort.cpp
# End Source File
# Begin Source File

SOURCE=.\hgefont.cpp
# End Source File
# Begin Source File

SOURCE=.\hgegui.cpp
# End Source File
# Begin Source File

SOURCE=.\hgeguictrls.cpp
# End Source File
# Begin Source File

SOURCE=.\hgeparticle.cpp
# End Source File
# Begin Source File

SOURCE=.\hgepmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\hgerect.cpp
# End Source File
# Begin Source File

SOURCE=.\hgeresource.cpp
# End Source File
# Begin Source File

SOURCE=.\hgesprite.cpp
# End Source File
# Begin Source File

SOURCE=.\hgestrings.cpp
# End Source File
# Begin Source File

SOURCE=.\hgevector.cpp
# End Source File
# Begin Source File

SOURCE=.\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\resources.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\hgeanim.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgecolor.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgedistort.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgefont.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgegui.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgeguictrls.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgeparticle.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgerect.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgeresource.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgesprite.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgestrings.h
# End Source File
# Begin Source File

SOURCE=..\..\include\hgevector.h
# End Source File
# Begin Source File

SOURCE=.\parser.h
# End Source File
# Begin Source File

SOURCE=.\resources.h
# End Source File
# End Group
# End Target
# End Project
