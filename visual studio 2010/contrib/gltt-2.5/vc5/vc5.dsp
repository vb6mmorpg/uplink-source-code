# Microsoft Developer Studio Project File - Name="vc5" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vc5 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vc5.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vc5.mak" CFG="vc5 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vc5 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vc5 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vc5 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /YX /FD /c /Tp
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../gltt.lib"

!ELSEIF  "$(CFG)" == "vc5 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc5___Wi"
# PROP BASE Intermediate_Dir "vc5___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc5___Wi"
# PROP Intermediate_Dir "vc5___Wi"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /YX /FD /c /Tp
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /YX /FD /c /Tp
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../gltt.lib"
# ADD LIB32 /nologo /out:"../gltt.lib"

!ENDIF 

# Begin Target

# Name "vc5 - Win32 Release"
# Name "vc5 - Win32 Debug"
# Begin Source File

SOURCE=..\BottomText.C
# End Source File
# Begin Source File

SOURCE=..\FTBitmapFont.C
# End Source File
# Begin Source File

SOURCE=..\FTBitmapFont.h
# End Source File
# Begin Source File

SOURCE=..\FTEngine.C
# End Source File
# Begin Source File

SOURCE=..\FTEngine.h
# End Source File
# Begin Source File

SOURCE=..\FTFace.C
# End Source File
# Begin Source File

SOURCE=..\FTFace.h
# End Source File
# Begin Source File

SOURCE=..\FTFont.C
# End Source File
# Begin Source File

SOURCE=..\FTFont.h
# End Source File
# Begin Source File

SOURCE=..\FTGlyph.C
# End Source File
# Begin Source File

SOURCE=..\FTGlyph.h
# End Source File
# Begin Source File

SOURCE=..\FTGlyphBitmap.C
# End Source File
# Begin Source File

SOURCE=..\FTGlyphBitmap.h
# End Source File
# Begin Source File

SOURCE=..\FTGlyphPixmap.C
# End Source File
# Begin Source File

SOURCE=..\FTGlyphPolygonizer.h
# End Source File
# Begin Source File

SOURCE=..\FTGlyphVectorizer.C
# End Source File
# Begin Source File

SOURCE=..\FTGlyphVectorizer.h
# End Source File
# Begin Source File

SOURCE=..\FTInstance.C
# End Source File
# Begin Source File

SOURCE=..\FTInstance.h
# End Source File
# Begin Source File

SOURCE=..\FTPixmapFont.C
# End Source File
# Begin Source File

SOURCE=..\GLTTBitmapFont.C
# End Source File
# Begin Source File

SOURCE=..\GLTTBitmapFont.h
# End Source File
# Begin Source File

SOURCE=..\GLTTboolean.h
# End Source File
# Begin Source File

SOURCE=..\GLTTFont.C
# End Source File
# Begin Source File

SOURCE=..\GLTTFont.h
# End Source File
# Begin Source File

SOURCE=..\GLTTGlyphPolygonizer.C
# End Source File
# Begin Source File

SOURCE=..\GLTTGlyphPolygonizer.h
# End Source File
# Begin Source File

SOURCE=..\GLTTGlyphPolygonizerHandler.C
# End Source File
# Begin Source File

SOURCE=..\GLTTGlyphPolygonizerHandler.h
# End Source File
# Begin Source File

SOURCE=..\GLTTGlyphTriangulator.C
# End Source File
# Begin Source File

SOURCE=..\GLTTGlyphTriangulator.h
# End Source File
# Begin Source File

SOURCE=..\GLTTOutlineFont.C
# End Source File
# Begin Source File

SOURCE=..\GLTTOutlineFont.h
# End Source File
# Begin Source File

SOURCE=..\GLTTPixmapFont.C
# End Source File
# End Target
# End Project
