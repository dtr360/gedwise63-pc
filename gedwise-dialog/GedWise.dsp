# Microsoft Developer Studio Project File - Name="GedWise" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GedWise - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GedWise.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GedWise.mak" CFG="GedWise - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GedWise - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF "$(CFG)" == "GedWise - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMSW"
# PROP BASE Intermediate_Dir "ReleaseMSW"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMSW"
# PROP Intermediate_Dir "ReleaseMSW"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /Fd"ReleaseMSW\GedWise.pdb" /MD /O2 /GR /EHsc /W4  /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /I "C:\wxWidgets/include" /I "C:\wxWidgets/contrib/include" /I "C:\wxWidgets/lib/vc_lib/msw" /I "C:\Program Files\Microsoft Visual Studio\vc98\include" /c
# ADD CPP /nologo /FD /Fd"ReleaseMSW\GedWise.pdb" /MD /O2 /GR /EHsc /W4  /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /I "C:\wxWidgets/include" /I "C:\wxWidgets/contrib/include" /I "C:\wxWidgets/lib/vc_lib/msw" /I "C:\Program Files\Microsoft Visual Studio\vc98\include" /c
# ADD BASE MTL /nologo /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /mktyplib203 /win32
# ADD MTL /nologo /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /mktyplib203 /win32
# ADD BASE RSC /l 0x405 /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /I "C:\wxWidgets/include" /I "C:\wxWidgets/contrib/include" /I "C:\wxWidgets/lib/vc_lib/msw" /I "C:\Program Files\Microsoft Visual Studio\vc98\include"
# ADD RSC /l 0x405 /DNOPCH /DWIN32 /D__WXMSW__ /D_WINDOWS /I "C:\wxWidgets/include" /I "C:\wxWidgets/contrib/include" /I "C:\wxWidgets/lib/vc_lib/msw" /I "C:\Program Files\Microsoft Visual Studio\vc98\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxmsw28_adv.lib wxmsw28_html.lib wxmsw28_xrc.lib wxbase28_net.lib wxbase28_xml.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /libpath:"C:\wxWidgets/lib/vc_lib" /libpath:"C:\Program Files\Microsoft Visual Studio\vc98\lib" /nologo /SUBSYSTEM:WINDOWS /machine:i386 /out:"ReleaseMSW\GedWise.exe"
# ADD LINK32 wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxmsw28_adv.lib wxmsw28_html.lib wxmsw28_xrc.lib wxbase28_net.lib wxbase28_xml.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib /libpath:"C:\wxWidgets/lib/vc_lib" /libpath:"C:\Program Files\Microsoft Visual Studio\vc98\lib" /nologo /SUBSYSTEM:WINDOWS /machine:i386 /out:"ReleaseMSW\GedWise.exe"

!ENDIF

# Begin Target

# Name "GedWise - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=gedwise.cpp
# End Source File
# Begin Source File

SOURCE=gedwise
# End Source File
# Begin Source File

SOURCE=GedWise.rc
# End Source File

# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=gedwise.h
# End Source File
# Begin Source File

SOURCE=gedwise
# End Source File

# End Group
# End Target
# End Project

