# Microsoft Developer Studio Project File - Name="coolprint" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=coolprint - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "coolprint.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "coolprint.mak" CFG="coolprint - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "coolprint - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "coolprint - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "coolprint - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib opengl32.lib glu32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "coolprint - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib opengl32.lib glu32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "coolprint - Win32 Release"
# Name "coolprint - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "minifmod"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\minifmod\Fmusic.c
# End Source File
# Begin Source File

SOURCE=.\minifmod\Fsound.c
# End Source File
# Begin Source File

SOURCE=.\minifmod\mixer_clipcopy.c
# End Source File
# Begin Source File

SOURCE=.\minifmod\mixer_fpu_ramp.c
# End Source File
# Begin Source File

SOURCE=.\minifmod\music_formatxm.c
# End Source File
# Begin Source File

SOURCE=.\minifmod\system_file.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\GenTex.cpp
# End Source File
# Begin Source File

SOURCE=.\GLFont.cpp
# End Source File
# Begin Source File

SOURCE=.\GLTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MATH3.CPP
# End Source File
# Begin Source File

SOURCE=.\noise.cpp
# End Source File
# Begin Source File

SOURCE=.\originalcode.txt
# End Source File
# Begin Source File

SOURCE=.\Particles.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "minifmod headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\minifmod\minifmod.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\Mixer.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\mixer_clipcopy.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\mixer_fpu_ramp.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\Music.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\music_formatxm.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\Sound.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\system_file.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\system_memory.h
# End Source File
# Begin Source File

SOURCE=.\minifmod\xmeffects.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\GenTex.h
# End Source File
# Begin Source File

SOURCE=.\GLFont.h
# End Source File
# Begin Source File

SOURCE=.\GLTexture.h
# End Source File
# Begin Source File

SOURCE=.\Math3.h
# End Source File
# Begin Source File

SOURCE=.\noise.h
# End Source File
# Begin Source File

SOURCE=.\Particles.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\gcbitmap.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource.rc
# End Source File
# Begin Source File

SOURCE=.\SK.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\DUNESHI2.XM
# End Source File
# End Target
# End Project
