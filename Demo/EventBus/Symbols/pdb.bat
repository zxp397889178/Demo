SETLOCAL ENABLEDELAYEDEXPANSION

set curdir=%~sdp0

::echo %curdir%
::echo %1%
cd /d %curdir%
net use \\192.168.19.57\pdb pdb /user:pdb
echo symstore.exe add /r /f %1% /s \\192.168.19.57\pdb /t "101PPT" /v "Release"
symstore.exe add /r /f %1% /s \\192.168.19.57\pdb /t "101PPT" /v "Release"