
!define COMPANY "Introversion Software"
!ifdef DEMO_INSTALL
  !define PRODUCT "Uplink Demo"
  !define PRODUCT_INSTALLER_NAME "UplinkDemo"
!else
  !define PRODUCT "Uplink"
  !define PRODUCT_INSTALLER_NAME "Uplink"
!endif
!define RES "resources\"
!define DAT "data\"
!define DAT_DOWNLOAD "data_download\"
!define DEFINSTDIR "$PROGRAMFILES\${PRODUCT}"
!define STARTMENU "$SMPROGRAMS\${PRODUCT}"

!define BAKDIR "$INSTDIR\${PRODUCT}Patch-${VERSION}-backup"
!define CHANGELOG "changes.txt"
!define README "readme.txt"
!define USERINFO "userinfo.txt"
!define EXE_FILE "uplink.exe"

!define SHOW_SPLASH
!define SHOW_CHANGELOG
#!define EDIT_USERINFO
!define MAKE_ICONS
!define DO_BACKUP
!define DO_INSTALL_CHECK
!define DO_PATCH_CHECK
!ifndef DEMO_INSTALL 
  !define DO_WORLD_SIZE_CHECK
  #!define DO_WORLD_MD5_CHECK
!endif

!ifdef FRESH_INSTALL
  !ifdef DO_BACKUP
    !undef DO_BACKUP
  !endif
  !ifdef DO_INSTALL_CHECK
    !undef DO_INSTALL_CHECK
  !endif
  !ifdef DO_PATCH_CHECK
    !undef DO_PATCH_CHECK
  !endif
  !ifdef DO_WORLD_SIZE_CHECK
    !undef DO_WORLD_SIZE_CHECK
  !endif
  !ifdef DO_WORLD_MD5_CHECK
    !undef DO_WORLD_MD5_CHECK
  !endif
!else
  !ifdef MAKE_ICONS
    !undef MAKE_ICONS
  !endif
!endif

!ifdef DOWNLOAD_INSTALL
  !define INSTALLER_NAME "${PRODUCT_INSTALLER_NAME}DownloadSetup-${VERSION}.exe"
  !define UNINSTALLER_NAME "Uninstall.exe"
!else ifdef FRESH_INSTALL
  !define INSTALLER_NAME "${PRODUCT_INSTALLER_NAME}Setup-${VERSION}.exe"
  !define UNINSTALLER_NAME "Uninstall.exe"
!else
  !define INSTALLER_NAME "${PRODUCT_INSTALLER_NAME}Patch-${VERSION}.exe"
  !define UNINSTALLER_NAME "Patch${VERSION}-Uninstall.exe"
!endif

Name "${PRODUCT}"
OutFile "${INSTALLER_NAME}"

#LicenseLangString license ${LANG_ENGLISH} "${RES}license-english.txt"
LicenseLangString license ${LANG_ENGLISH} "${DAT}license.txt"

VIAddVersionKey "ProductName" "${PRODUCT}"
VIAddVersionKey "CompanyName" "${COMPANY}"
VIAddVersionKey "LegalCopyright" "© ${COMPANY}"
!ifdef FRESH_INSTALL
  VIAddVersionKey "FileDescription" "${PRODUCT} Installer"
!else
  VIAddVersionKey "FileDescription" "${PRODUCT} Patch Installer"
!endif
VIAddVersionKey "FileVersion" "${VERSION}"
VIProductVersion "0.0.1.0"

LicenseData $(license)
SetCompressor /SOLID LZMA
Icon "${RES}uplink-installer.ico"
UninstallIcon "${RES}uplink-installer.ico"

!ifdef FRESH_INSTALL
  DirText "This will install ${PRODUCT} ${VERSION}. Please select the folder where ${PRODUCT} is installed." "" ""
  UninstallText "This will uninstall ${PRODUCT}."
!else
  DirText "This patch will modify an existing ${PRODUCT} installation. Please select the folder where ${PRODUCT} is installed." \
          "${PRODUCT} directory" "" ""
  UninstallText "This will uninstall the ${PRODUCT} ${VERSION} patch."
!endif



Page license
Page directory
!ifdef MAKE_ICONS
  Page components
!endif
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles


!macro BackupAndInstallFile FILE
  !ifdef DO_BACKUP
    IfFileExists "${BAKDIR}\*.*" +2
      CreateDirectory "${BAKDIR}"
    IfFileExists "$INSTDIR\${FILE}" 0 +2
      Rename "$INSTDIR\${FILE}" "${BAKDIR}\${FILE}"
  !endif
  File "/oname=$INSTDIR\${FILE}" "${DAT}${FILE}"
!macroend

!macro BackupAndInstallFileDownload FILE
  !ifdef DO_BACKUP
    IfFileExists "${BAKDIR}\*.*" +2
      CreateDirectory "${BAKDIR}"
    IfFileExists "$INSTDIR\${FILE}" 0 +2
      Rename "$INSTDIR\${FILE}" "${BAKDIR}\${FILE}"
  !endif
  File "/oname=$INSTDIR\${FILE}" "${DAT_DOWNLOAD}${FILE}"
!macroend

!macro DeleteAndRestoreFile FILE
  Delete "$INSTDIR\${FILE}"
  !ifdef DO_BACKUP
    IfFileExists "${BAKDIR}\${FILE}" 0 +2
      Rename "${BAKDIR}\${FILE}" "$INSTDIR\${FILE}"
  !endif
!macroend

!macro BackupAndRemoveFile FILE
  !ifdef DO_BACKUP
    IfFileExists "${BAKDIR}\*.*" +2
      CreateDirectory "${BAKDIR}"
    IfFileExists "$INSTDIR\${FILE}" 0 +2
      Rename "$INSTDIR\${FILE}" "${BAKDIR}\${FILE}"
  !endif
!macroend

!macro RestoreFile FILE
  !ifdef DO_BACKUP
    IfFileExists "${BAKDIR}\${FILE}" 0 +2
      Rename "${BAKDIR}\${FILE}" "$INSTDIR\${FILE}"
  !endif
!macroend

!macro DeleteIfEmpty DIR
  Push $0
  StrCpy $0 "${DIR}"
  Call un.DeleteDirIfEmpty
  Pop $0
!macroend

!ifdef SHOW_SPLASH
ReserveFile "${RES}splash.bmp"  # Make sure splash is decompressed first
!endif


Section "-Install Files"
  !ifdef FRESH_INSTALL
    IfFileExists "$INSTDIR\*.*" +2
      CreateDirectory "$INSTDIR"
  !else
    Call CheckUplinkInstall
    Call CheckUplinkNotPatched
  !endif

  !insertmacro BackupAndInstallFile "${EXE_FILE}"
  !insertmacro BackupAndInstallFile "patch.dat"
  !insertmacro BackupAndInstallFile "patch2.dat"
  !insertmacro BackupAndInstallFile "patch3.dat"
  #!insertmacro BackupAndInstallFile "UplinkSupport.exe"
  #!insertmacro BackupAndInstallFile "UplinkSupport.ini"
  !insertmacro BackupAndInstallFile "dbghelp.dll"
  !insertmacro BackupAndInstallFile "tcp4w32.dll"
  !insertmacro BackupAndInstallFile "SDL.dll"
  !insertmacro BackupAndInstallFile "SDL_mixer.dll"
  !insertmacro BackupAndInstallFile "opengl32.dll.bak"
  !insertmacro BackupAndInstallFile "${README}"
  !insertmacro BackupAndInstallFile "${CHANGELOG}"
  #!insertmacro BackupAndInstallFile "${USERINFO}"
  !ifdef DOWNLOAD_INSTALL
    !insertmacro BackupAndInstallFileDownload "codecard.txt"
  !endif
  IfFileExists "$INSTDIR\docs\*.*" +2
    CreateDirectory "$INSTDIR\docs"
  !insertmacro BackupAndInstallFile "docs\README-SDL.txt"
  !insertmacro BackupAndInstallFile "docs\README-SDL_mixer.txt"
  #!insertmacro BackupAndInstallFile "docs\README-UplinkSupport.txt"
  !insertmacro BackupAndInstallFile "docs\SDL_Mixer.patch"
  !insertmacro BackupAndInstallFile "docs\FTGL.patch"

  #Added for compatibility with the Dev CD mods
  !insertmacro BackupAndInstallFile  "GLU32.DLL"
  !insertmacro BackupAndInstallFile  "glut32.dll"

  !insertmacro BackupAndRemoveFile  "vcredist_x86.exe"
  !insertmacro BackupAndRemoveFile  "XCrashReport.exe"
  !insertmacro BackupAndRemoveFile  "XCrashReport.ini"

  !insertmacro BackupAndInstallFile "sounds.dat"
  !ifdef FRESH_INSTALL
    !insertmacro BackupAndInstallFile "data.dat"
    !insertmacro BackupAndInstallFile "fonts.dat"
    !insertmacro BackupAndInstallFile "graphics.dat"
    !insertmacro BackupAndInstallFile "loading.dat"
    !insertmacro BackupAndInstallFile "music.dat"
    !ifdef DOWNLOAD_INSTALL
      !insertmacro BackupAndInstallFileDownload "world.dat"
    !else ifndef DEMO_INSTALL
      !insertmacro BackupAndInstallFile "world.dat"
    !endif
  !endif  

  !insertmacro BackupAndRemoveFile "Microsoft.VC80.CRT.manifest"
  !insertmacro BackupAndRemoveFile "msvcm80.dll"
  !insertmacro BackupAndRemoveFile "msvcp80.dll"
  !insertmacro BackupAndRemoveFile "msvcr80.dll"
  !insertmacro BackupAndRemoveFile "Microsoft.VC80.MFC.manifest"
  !insertmacro BackupAndRemoveFile "mfc80.dll"

  !ifdef FRESH_INSTALL
    ;write uninstall information to the registry
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayName" "${PRODUCT} (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "UninstallString" "$INSTDIR\${UNINSTALLER_NAME}"
  !endif

  WriteUninstaller "$INSTDIR\${UNINSTALLER_NAME}"
SectionEnd


!ifdef MAKE_ICONS
Section "Start Menu Icons"
  ;create start-menu items
  CreateDirectory "${STARTMENU}"
  CreateShortCut "${STARTMENU}\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  CreateShortCut "${STARTMENU}\${PRODUCT}.lnk" "$INSTDIR\${EXE_FILE}" "" "$INSTDIR\${EXE_FILE}" 0
  CreateShortCut "${STARTMENU}\Release Notes.lnk" "$INSTDIR\${CHANGELOG}"
  CreateShortCut "${STARTMENU}\Read Me.lnk" "$INSTDIR\${README}"
SectionEnd
!endif


!ifdef MAKE_ICONS
Section "Desktop Icon"
    ;create desktop shortcut
    CreateShortCut "$DESKTOP\${PRODUCT}.lnk" "$INSTDIR\${EXE_FILE}" ""
SectionEnd
!endif


Section "Uninstall"
  !insertmacro DeleteAndRestoreFile "uplink.exe"
  !insertmacro DeleteAndRestoreFile "patch.dat"
  !insertmacro DeleteAndRestoreFile "patch2.dat"
  !insertmacro DeleteAndRestoreFile "patch3.dat"
  #!insertmacro DeleteAndRestoreFile "UplinkSupport.exe"
  #!insertmacro DeleteAndRestoreFile "UplinkSupport.ini"
  !insertmacro DeleteAndRestoreFile "dbghelp.dll"
  !insertmacro DeleteAndRestoreFile "tcp4w32.dll"
  !insertmacro DeleteAndRestoreFile "SDL.dll"
  !insertmacro DeleteAndRestoreFile "SDL_mixer.dll"
  !insertmacro DeleteAndRestoreFile "opengl32.dll.bak"
  !insertmacro DeleteAndRestoreFile "${README}"
  !insertmacro DeleteAndRestoreFile "${CHANGELOG}"
  !insertmacro DeleteAndRestoreFile "${USERINFO}"
  !insertmacro DeleteAndRestoreFile "codecard.txt"
  !insertmacro DeleteAndRestoreFile "docs\README-SDL.txt"
  !insertmacro DeleteAndRestoreFile "docs\README-SDL_mixer.txt"
  #!insertmacro DeleteAndRestoreFile "docs\README-UplinkSupport.txt"
  !insertmacro DeleteAndRestoreFile "docs\SDL_Mixer.patch"
  !insertmacro DeleteAndRestoreFile "docs\FTGL.patch"

  #Added for compatibility with the Dev CD mods
  !insertmacro DeleteAndRestoreFile "GLU32.DLL"
  !insertmacro DeleteAndRestoreFile "glut32.dll"

  !insertmacro RestoreFile          "vcredist_x86.exe"
  !insertmacro RestoreFile          "XCrashReport.exe"
  !insertmacro RestoreFile          "XCrashReport.ini"
  
  !insertmacro RestoreFile          "Microsoft.VC80.CRT.manifest"
  !insertmacro RestoreFile          "msvcm80.dll"
  !insertmacro RestoreFile          "msvcp80.dll"
  !insertmacro RestoreFile          "msvcr80.dll"
  !insertmacro RestoreFile          "Microsoft.VC80.MFC.manifest"
  !insertmacro RestoreFile          "mfc80.dll"
  
  !insertmacro DeleteAndRestoreFile "sounds.dat"
  !ifdef FRESH_INSTALL
    !insertmacro DeleteAndRestoreFile "data.dat"
    !insertmacro DeleteAndRestoreFile "fonts.dat"
    !insertmacro DeleteAndRestoreFile "graphics.dat"
    !insertmacro DeleteAndRestoreFile "loading.dat"
    !insertmacro DeleteAndRestoreFile "music.dat"
    !ifndef DEMO_INSTALL
      !insertmacro DeleteAndRestoreFile "world.dat"
    !endif
  !endif

  IfFileExists "$INSTDIR\docs\*.*" 0 +2
    RMDir "$INSTDIR\docs"
  
  IfFileExists "${BAKDIR}\*.*" 0 +2
    RMDir "${BAKDIR}"

  !ifdef MAKE_ICONS
    Delete "${STARTMENU}\*.*"
    !insertmacro DeleteIfEmpty "${STARTMENU}"
    Delete "$DESKTOP\${PRODUCT}.lnk"
  !endif
  
  !ifdef FRESH_INSTALL
    ;delete uninstall information from the registry
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
    
    Delete "$INSTDIR"
  !endif

  Delete "$INSTDIR\${UNINSTALLER_NAME}"
  !insertmacro DeleteIfEmpty "$INSTDIR"
SectionEnd


Function .onInit
  StrCpy $INSTDIR "${DEFINSTDIR}"
  Call ShowSplash
FunctionEnd


Function .onVerifyInstDir
  Call CheckUplinkInstall
FunctionEnd


Function .onInstSuccess
!ifdef EDIT_USERINFO
  MessageBox MB_OK "Please edit ${USERINFO} in your Uplink directory, so that we can contact you for support purposes." IDOK 0
  Exec '"notepad.exe" "$INSTDIR\${USERINFO}"'
!endif
!ifdef SHOW_CHANGELOG
  MessageBox MB_YESNO "Uplink ${VERSION} install successful. View changelog?" IDNO NoReadme
    Exec '"notepad.exe" "$INSTDIR\${CHANGELOG}"'
  NoReadme:
!endif
FunctionEnd


Function CheckUplinkInstall
!ifdef DO_INSTALL_CHECK
  Push $0

  IfFileExists "$INSTDIR\uplink.exe" 0 cui_badness
  IfFileExists "$INSTDIR\data.dat" 0 cui_badness
  !ifndef DEMO_INSTALL
    IfFileExists "$INSTDIR\world.dat" 0 cui_badness

    !ifdef DO_WORLD_SIZE_CHECK
    StrCpy $0 "$INSTDIR\world.dat"
    Call GetFileSize
    IfErrors cui_szerror 0
    IntCmp $0 14400792 0 cui_badness cui_badness
    !endif

    !ifdef DO_WORLD_MD5_CHECK
    md5dll::GetMD5File "$INSTDIR\world.dat"
    Pop $0
	# Download world.dat
    StrCmp $0 "7ef401ab95b64d58cae4975a05e2b7d3" cui_end_md5_check 0
    StrCmp $0 "4390bb107c12a4a1fd9ff309cabc95fc" 0 cui_badness
cui_end_md5_check:
    !endif
  !endif

  Pop $0
  Return
cui_szerror:
  #DetailPrint "There was an error getting Uplink installation information."
cui_badness:
  Pop $0
  Abort # "No valid Uplink installation found."
!endif
FunctionEnd


Function CheckUplinkNotPatched
!ifdef DO_PATCH_CHECK
  IfFileExists "${BAKDIR}\*.*" 0 +2
  Abort "It seems the patch is already installed. Continuing would overwrite backup data."
!endif
FunctionEnd


Function ShowSplash
!ifdef SHOW_SPLASH
  # the plugins dir is automatically deleted when the installer exits
  InitPluginsDir
  File /oname=$PLUGINSDIR\splash.bmp "${RES}splash.bmp"

  advsplash::show 800 400 300 -1 $PLUGINSDIR\splash

  Pop $0   ; $0 has '1' if the user closed the splash screen early,
           ; '0' if everything closed normally, and '-1' if some error occurred.

  Delete $PLUGINSDIR\splash.bmp
!endif
FunctionEnd


Function GetFileSize
  Push $1
  ClearErrors
  FileOpen $1 $0 r
  IfErrors gfs_error
  FileSeek $1 0 END $0
  FileClose $1
  Goto gfs_done
gfs_error:
  SetErrors
gfs_done:
  Pop $1
FunctionEnd

Function un.DeleteDirIfEmpty
  Push $R0
  Push $R1
  FindFirst $R0 $R1 "$0\*.*"
  strcmp $R1 "." 0 NoDelete
   FindNext $R0 $R1
   strcmp $R1 ".." 0 NoDelete
    ClearErrors
    FindNext $R0 $R1
    IfErrors 0 NoDelete
     FindClose $R0
     Sleep 1000
     RMDir "$0"
  NoDelete:
   FindClose $R0
  Pop $R1
  Pop $R0
FunctionEnd


