# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=tcp4w32l - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to tcp4w32l - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tcp4w32l - Win32 Release" && "$(CFG)" !=\
 "tcp4w32l - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "tcp4w32l.mak" CFG="tcp4w32l - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcp4w32l - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tcp4w32l - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "tcp4w32l - Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "tcp4w32l - Win32 Release"

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

ALL : "$(OUTDIR)\tcp4w32l.lib"

CLEAN : 
	-@erase ".\tcp4w32l.lib"
	-@erase ".\tcp4u.obj"
	-@erase ".\http4u.obj"
	-@erase ".\udp4u.obj"
	-@erase ".\tn4u.obj"
	-@erase ".\tcp4_err.obj"
	-@erase ".\util.obj"
	-@erase ".\htp4_url.obj"
	-@erase ".\tcp4u_ex.obj"
	-@erase ".\skt4u.obj"
	-@erase ".\htp4_err.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O1 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MT /W3 /GX /O1 /I "..\Include" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\./
CPP_SBRS=
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/tcp4w32l.bsc" 
BSC32_SBRS=
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/tcp4w32l.lib" 
LIB32_OBJS= \
	".\tcp4u.obj" \
	".\http4u.obj" \
	".\udp4u.obj" \
	".\tn4u.obj" \
	".\tcp4_err.obj" \
	".\util.obj" \
	".\htp4_url.obj" \
	".\tcp4u_ex.obj" \
	".\skt4u.obj" \
	".\htp4_err.obj"

"$(OUTDIR)\tcp4w32l.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tcp4w32l - Win32 Debug"

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

ALL : "$(OUTDIR)\tcp4w32l.lib"

CLEAN : 
	-@erase ".\tcp4w32l.lib"
	-@erase ".\tcp4u.obj"
	-@erase ".\http4u.obj"
	-@erase ".\udp4u.obj"
	-@erase ".\tn4u.obj"
	-@erase ".\tcp4_err.obj"
	-@erase ".\util.obj"
	-@erase ".\htp4_url.obj"
	-@erase ".\tcp4u_ex.obj"
	-@erase ".\skt4u.obj"
	-@erase ".\htp4_err.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /YX
CPP_PROJ=/nologo /MTd /W3 /GX /Z7 /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\./
CPP_SBRS=
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/tcp4w32l.bsc" 
BSC32_SBRS=
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/tcp4w32l.lib" 
LIB32_OBJS= \
	".\tcp4u.obj" \
	".\http4u.obj" \
	".\udp4u.obj" \
	".\tn4u.obj" \
	".\tcp4_err.obj" \
	".\util.obj" \
	".\htp4_url.obj" \
	".\tcp4u_ex.obj" \
	".\skt4u.obj" \
	".\htp4_err.obj"

"$(OUTDIR)\tcp4w32l.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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

# Name "tcp4w32l - Win32 Release"
# Name "tcp4w32l - Win32 Debug"

!IF  "$(CFG)" == "tcp4w32l - Win32 Release"

!ELSEIF  "$(CFG)" == "tcp4w32l - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=...31\src\util.c
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

SOURCE=..\tcp4u.c
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

SOURCE=...31\src\tcp4_err.c
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

SOURCE=...31\src\skt4u.c
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

SOURCE=...31\src\http4u.c
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

SOURCE=...31\src\htp4_url.c
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
	

"$(INTDIR)\htp4_url.obj" : $(SOURCE) $(DEP_CPP_HTP4_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=...31\src\htp4_err.c
DEP_CPP_HTP4_E=\
	".\..\src\build.h"\
	{$(INCLUDE)}"\SYS\TYPES.H"\
	".\..\src\port.h"\
	".\..\src\skt4u.h"\
	".\..\src\dimens.h"\
	".\..\Include\tcp4ux.h"\
	".\..\Include\tcp4w.h"\
	".\..\Include\udp4u.h"\
	".\..\Include\http4u.h"\
	

"$(INTDIR)\htp4_err.obj" : $(SOURCE) $(DEP_CPP_HTP4_E) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=...31\src\udp4u.c
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
