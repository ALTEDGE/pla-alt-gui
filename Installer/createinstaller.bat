echo off

echo Packaging program...
cd ..\Release
C:\Qt\Tools\QtInstallerFramework\4.1\bin\archivegen.exe ..\Installer\packages\com.pla.gui\data\program.7z assets\* drivers\* iconengines\* imageformats\* platforms\* styles\* *.dll PLA_FPS.exe

echo Creating installer...
cd ..\Installer
C:\Qt\Tools\QtInstallerFramework\4.1\bin\binarycreator.exe --offline-only -c config.xml -p packages "PLA FPS v0.8.1 Installer.exe"

pause
echo on
