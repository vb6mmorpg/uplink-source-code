
TO GENERATE AN INSTALLER:
=========================

 - Get the Nullsoft Installer System

 - Put the files you wish to distribute into the data directory.

 - Edit the uplink-version.nsh file to include the correct version number.

 - Put the following line into the "Install Files" section of uplink-common.nsh
   for each file you're distributing.
     !insertmacro BackupAndInstallFile "file.ext"

 - This line goes in the "Uninstall" section of uplink-common.nsh:
     !insertmacro DeleteAndRestoreFile "file.ext"

 - Compile the scripts (right-click in Windows Explorer)
   uplink-patch.nsi
   uplink-full.nsi


ALSO DON'T FORGET TO:
=====================

 - Edit uplink/src/app/globals_defines.h file to include the correct version number ( VERSION_NUMBER_INT ).

 - Edit uplink/src/resources.rc file to include the correct version number. 

 - Edit Installer/data/readme.txt file to include the correct version number.

 - Edit Installer/data/changes.txt file to include the correct version number and changes.

 - Copy uplink/bin/Uplink.exe to Installer/data
 
 - Copy uplink/bin/Uplink.exe, uplink/bin/Uplink.pdb to uplink/release/[VERSION_NUMBER]
