:: The DATAPATH variable in this file must be set to the directory in which
:: thermodynamic files are found.
:: This file is meant to be used with Windows machines only.
@ECHO OFF
CD %~dp0\..
SET RNA_PROG=%CD%
SET DATAPATH=%RNA_PROG%\data_tables
SET PATH=%PATH%;%RNA_PROG%\exe
:: echo RNA_PROG=%RNA_PROG%
cd "%USERPROFILE%"
IF EXIST "%USERPROFILE%\Documents" (
	CD "%USERPROFILE%\Documents" 
) ELSE ( 
	IF EXIST "%USERPROFILE%\My Documents" CD "%USERPROFILE%\My Documents" )
)
IF EXIST .\RNAstructure ( 
	CD .\RNAstructure 
) ELSE ( 
	IF EXIST .\RNAstructure-examples CD .\RNAstructure-examples
)

:: Get a list of RNAstructure commands
SET RNA_COMMANDS=
FOR /F "usebackq delims=" %%G IN (`dir /B "%RNA_PROG%"\exe\*.exe ^| find /V "-smp" `) DO CALL :add_command %%~nG

:: Display version and list of commands.
FINDSTR /v /b "#" "%RNA_PROG%\Version.txt"
ECHO. 
ECHO. 
ECHO Available RNAstructure Commands: %RNA_COMMANDS%

GOTO :eof
:add_command
  SET RNA_COMMANDS=%RNA_COMMANDS%  %1
GOTO :eof