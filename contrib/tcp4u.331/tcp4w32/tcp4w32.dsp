# Microsoft Developer Studio Project File - Name="tcp4u32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tcp4u32 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tcp4w32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tcp4w32.mak" CFG="tcp4u32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcp4u32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tcp4u32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tcp4u32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /MT /W3 /GX /O1 /Oy /Ob2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "TCP4W_DLL" /FD /c
# SUBTRACT CPP /nologo /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib /subsystem:windows /dll /pdb:none /machine:I386
# SUBTRACT LINK32 /nologo /map /nodefaultlib

!ELSEIF  "$(CFG)" == "tcp4u32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "TCP4W_DLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386

!ENDIF 

# Begin Target

# Name "tcp4u32 - Win32 Release"
# Name "tcp4u32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\src\htp4_err.c
# End Source File
# Begin Source File

SOURCE=..\src\htp4_url.c
# End Source File
# Begin Source File

SOURCE=..\src\http4u.c
# End Source File
# Begin Source File

SOURCE=..\src\skt4u.c

!IF  "$(CFG)" == "tcp4u32 - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "tcp4u32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\smtp4u.c
# End Source File
# Begin Source File

SOURCE=..\src\tcp4_err.c
# End Source File
# Begin Source File

SOURCE=..\src\tcp4_log.c
# End Source File
# Begin Source File

SOURCE=..\src\tcp4u.c
# End Source File
# Begin Source File

SOURCE=..\src\tcp4u_ex.c
# End Source File
# Begin Source File

SOURCE=.\TCP4W32.DEF
# End Source File
# Begin Source File

SOURCE=..\src\tn4u.c
# End Source File
# Begin Source File

SOURCE=..\src\tn_proto.c
# End Source File
# Begin Source File

SOURCE=..\src\udp4u.c
# End Source File
# Begin Source File

SOURCE=..\src\util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\src\build.h
# End Source File
# Begin Source File

SOURCE=..\src\dimens.h
# End Source File
# Begin Source File

SOURCE=..\src\port.h
# End Source File
# Begin Source File

SOURCE=..\src\skt4u.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
