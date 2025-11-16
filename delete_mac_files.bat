@echo off
REM Get the folder where this script is located
SET TARGET_DIR=%~dp0

echo Deleting Mac .DS_Store and ._ files in %TARGET_DIR% ...

REM Delete all .DS_Store files recursively
del /s /q /f "%TARGET_DIR%.DS_Store"

REM Delete all ._ files recursively
del /s /q /f "%TARGET_DIR%._*"

echo Done!
pause
