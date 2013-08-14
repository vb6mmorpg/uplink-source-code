# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=tcp4u32 - Win32 Release
!MESSAGE No configuration specified.  Defaulting to tcp4u32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "tcp4u32 - Win32 Release" && "$(CFG)" !=\
 "tcp4u32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "tcp4w32.mak" CFG="tcp4u32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcp4u32 - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tcp4u32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "tcp4u32 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "tcp4u32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""
OUTDIR=.\.
INTDIR=.\.

ALL : "$(OUTDIR)\tcp4w32.dll"

CLEAN : 
	-@erase ".\tcp4w32.dll"
	-@erase ".\htp4_err.obj"
	-@erase ".\http4u.obj"
	-@erase ".\tn4u.obj"
	-@erase ".\util.obj"
	-@erase ".\skt4u.obj"
	-@erase ".\tcp4_err.obj"
	-@erase ".\htp4_url.obj"
	-@erase ".\tcp4u_ex.obj"
	-@erase ".\tcp4u.obj"
	-@erase ".\udp4u.obj"
	-@erase ".\tcp4w32.lib"
	-@erase ".\tcp4w32.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /MT /W3 /GX /O1 /Oy /Ob2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "TCP4W_DLL" /c
# SUBTRACT CPP /nologo /YX
CPP_PROJ=/MT /W3 /GX /O1 /Oy /Ob2 /I "..\Include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "TCP4W_DLL" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\./
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/tcp4w32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib /subsystem:windows /dll /pdb:none /machine:I386
# SUBTRACT LINK32 /nologo /map /nodefaultlib
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib /subsystem:windows /dll\
 /pdb:none /machine:I386 /def:".\TCP4W32.DEF" /out:"$(OUTDIR)/tcp4w32.dll"\
 /implib:"$(OUTDIR)/tcp4w32.lib" 
DEF_FILE= \
	".\TCP4W32.DEF"
LINK32_OBJS= \
	"$(INTDIR)/htp4_err.obj" \
	"$(INTDIR)/http4u.obj" \
	"$(INTDIR)/tn4u.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/skt4u.obj" \
	"$(INTDIR)/tcp4_err.obj" \
	"$(INTDIR)/htp4_url.obj" \
	"$(INTDIR)/tcp4u_ex.obj" \
	"$(INTDIR)/tcp4u.obj" \
	"$(INTDIR)/udp4u.obj"

"$(OUTDIR)\tcp4w32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tcp4u32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "."
# PROP Target_Dir ""
OUTDIR=.\.
INTDIR=.\.

ALL : "$(OUTDIR)\tcp4w32.dll"

CLEAN : 
	-@erase ".\vc40.pdb"
	-@erase ".\vc40.idb"
	-@erase ".\tcp4w32.dll"
	-@erase ".\htp4_err.obj"
	-@erase ".\http4u.obj"
	-@erase ".\tn4u.obj"
	-@erase ".\util.obj"
	-@erase ".\skt4u.obj"
	-@erase ".\tcp4_err.obj"
	-@erase ".\htp4_url.obj"
	-@erase ".\tcp4u_ex.obj"
	-@erase ".\tcp4u.obj"
	-@erase ".\udp4u.obj"
	-@erase ".\tcp4w32.lib"
	-@erase ".\tcp4w32.exp"
	-@erase ".\tcp4w32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "TCP4W_DLL" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\Include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /D "TCP4W_DLL" /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\./
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/tcp4w32.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib wsock32.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib wsock32.lib /nologo /subsystem:windows\
 /dll /incremental:no /pdb:"$(OUTDIR)/tcp4w32.pdb" /debug /machine:I386\
 /def:".\TCP4W32.DEF" /out:"$(OUTDIR)/tcp4w32.dll"\
 /implib:"$(OUTDIR)/tcp4w32.lib" 
DEF_FILE= \
	".\TCP4W32.DEF"
LINK32_OBJS= \
	"$(INTDIR)/htp4_err.obj" \
	"$(INTDIR)/http4u.obj" \
	"$(INTDIR)/tn4u.obj" \
	"$(INTDIR)/util.obj" \
	"$(INTDIR)/skt4u.obj" \
	"$(INTDIR)/tcp4_err.obj" \
	"$(INTDIR)/htp4_url.obj" \
	"$(INTDIR)/tcp4u_ex.obj" \
	"$(INTDIR)/tcp4u.obj" \
	"$(INTDIR)/udp4u.obj"

"$(OUTDIR)\tcp4w32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "tcp4u32 - Win32 Release"
# Name "tcp4u32 - Win32 Debug"

!IF  "$(CFG)" == "tcp4u32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tcp4u32 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\TCP4W32.DEF

!IF  "$(CFG)" == "tcp4u32 - Win32 Release"

!ELSEIF  "$(CFG)" == "tcp4u32 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\tn4u.c
DEP_CPP_TN4U_=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\tn4u.obj" : $(SOURCE) $(DEP_CPP_TN4U_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\tcp4u_ex.c
DEP_CPP_TCP4U=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\tcp4u_ex.obj" : $(SOURCE) $(DEP_CPP_TCP4U) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\tcp4u.c
DEP_CPP_TCP4U_=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\tcp4u.obj" : $(SOURCE) $(DEP_CPP_TCP4U_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\tcp4_err.c
DEP_CPP_TCP4_=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\tcp4_err.obj" : $(SOURCE) $(DEP_CPP_TCP4_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\skt4u.c
DEP_CPP_SKT4U=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\skt4u.obj" : $(SOURCE) $(DEP_CPP_SKT4U) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\http4u.c
DEP_CPP_HTTP4=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\http4u.obj" : $(SOURCE) $(DEP_CPP_HTTP4) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\htp4_err.c
DEP_CPP_HTP4_=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\htp4_err.obj" : $(SOURCE) $(DEP_CPP_HTP4_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=..\src\htp4_url.c
DEP_CPP_HTP4_U=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\htp4_url.obj" : $(SOURCE) $(DEP_CPP_HTP4_U) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\users\phjounin\PJ\SHARED\tcp4u.31\src\util.c
DEP_CPP_UTIL_=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\util.obj" : $(SOURCE) $(DEP_CPP_UTIL_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\users\phjounin\PJ\SHARED\tcp4u.31\src\udp4u.c
DEP_CPP_UDP4U=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\udp4u.obj" : $(SOURCE) $(DEP_CPP_UDP4U) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
