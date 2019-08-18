echo off

echo Packaging program...
cd ..\build-Pla_GUI-Qt_Static-Release\release
C:\Qt\Tools\QtInstallerFramework\3.0\bin\archivegen.exe ..\..\Installer\packages\com.pla.gui\data\program.7z *.dll *.png stylesheet Pla_GUI.exe platforms\* assets\* driver\*

echo Creating installer...
cd ..\..\Installer
C:\Qt\Tools\QtInstallerFramework\3.0\bin\binarycreator.exe --offline-only -c config.xml -p packages "PLA Installer.exe"

pause
echo on
