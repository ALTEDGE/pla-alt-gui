echo off

echo Packaging program...
cd ..\Release
C:\Qt\Tools\QtInstallerFramework\4.1\bin\archivegen.exe ..\Installer\packages\com.pla.gui\data\program.7z assets\* drivers\* iconengines\* imageformats\* platforms\* styles\* *.dll Pla_GUI.exe

echo Creating installer...
cd ..\Installer
C:\Qt\Tools\QtInstallerFramework\4.1\bin\binarycreator.exe --offline-only -c config.xml -p packages "PLA Installer.exe"

pause
echo on
