ECHO OFF
cmake --build .
if %errorlevel% neq 0 exit /b %errorlevel%
start /d "E:/dev/Snake/Debug" Snake.exe