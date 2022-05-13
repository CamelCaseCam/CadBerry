:: This file is meant to be used with Windows machines only.

:: Get the path to the directory that contains this batch file.
SET JAR_DIR=%~dp0

:: The PATH is used to load the native RNAstructure_GUI library (DLL)
SET PATH=%PATH%;%JAR_DIR%

:: The DATAPATH environment variable can be set to the directory 
:: in which thermodynamic files are stored. This is only necessary 
:: if you want use a non-standard directory for the data tables.
:: Otherwise RNAstructure auto-detects the correct directory.
:: SET DATAPATH=%JAR_DIR%\..\data_tables

java -jar "%JAR_DIR%\RNAstructure.jar"
