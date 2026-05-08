@echo off
setlocal
REM ============================================================
REM  Build script for hospital_gui (Qt 6.11 + MinGW g++ 8.1.0)
REM  Uses noexcept workaround in hospital_gui.pro
REM ============================================================

echo [1/4] Removing old Makefile...
del /q Makefile Makefile.Release Makefile.Debug 2>nul

echo [2/4] Running qmake (Qt 6.11)...
"D:/Qt/6.11.0/mingw_64/bin/qmake.exe" hospital_gui.pro
if %ERRORLEVEL% neq 0 (
    echo ERROR: qmake failed! Is Qt 6.11 installed at D:/Qt/6.11.0/ ?
    pause
    exit /b 1
)

echo [3/4] Cleaning...
mingw32-make clean 2>nul

echo [4/4] Building...
mingw32-make
if %ERRORLEVEL% neq 0 (
    echo.
    echo BUILD FAILED. Check errors above.
    pause
    exit /b 1
)

echo.
echo BUILD SUCCESSFUL! Run: release\hospital_gui.exe
pause
