SET DEST_DIR=%~dp0..\exe\%2
mkdir "%DEST_DIR%"
xcopy %1  "%DEST_DIR%" /H/R/Y
xcopy %~dpn1.lib  "%DEST_DIR%" /H/R/Y

echo Copied %1 to "%DEST_DIR%"