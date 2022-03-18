SET SCRIPTDIR=%~dp0
ECHO SCRIPTDIR=&SCRIPTDIR&
FOR %%i IN ("%SCRIPTDIR%\.") DO SET SUBDIR=%%~nxi
ECHO SUBDIR=%SUBDIR%

SET PATH=%PATH%;c:\Progra~1\Java\jdk1.8.0_101\bin

:: CD to parent directory. All paths below are relative to it. (i.e. RNAstructure/alignment-editor)
CD "%SCRIPTDIR%/.."

:: Location of compiled class file output (from IntelliJ IDE)
SET CLASSES=ide/out/production

:: Location of output jar file
SET OUTJAR=%SCRIPTDIR%/structure-editor.jar

:: Delete the output file
DEL %OUTJAR%

:: List the paths where class files are located. 
:: The -C argument will CD to the first path and include all files/dirs in the subsequent arguments
:: Importantly, any subdirectory information is preserved so:
::    -C PathA  PathB/PathC/PathD  will have a folder "PathB/PathC/PathD" in the jar file, whereas 
::    -C PathA/PathB  PathC/PathD  will have a folder "PathC/PathD" in the jar file.
SET INCLUDE=
SET INCLUDE=%INCLUDE%  -C %CLASSES%/StructureEditor ur_rna/StructureEditor
SET INCLUDE=%INCLUDE%  -C %CLASSES%/StructureEditor ur_rna/RNAstructure
SET INCLUDE=%INCLUDE%  -C %CLASSES%/Utilities       ur_rna/Utilities
jar cvmf %SCRIPTDIR%/Manifest.txt %OUTJAR% %INCLUDE%
pause
