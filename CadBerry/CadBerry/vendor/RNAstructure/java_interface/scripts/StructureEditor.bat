:: This file is meant to be used with Windows machines only.

SET JAR_DIR=%~dp0

:: The PATH is used to load the native RNAstructure_GUI library
SET PATH=%PATH%;%JAR_DIR%

:: The DATAPATH variable in this file must be set to the directory 
:: in which thermodynamic files are found.
SET DATAPATH=%JAR_DIR%\..\data_tables

java -jar %JAR_DIR%\StructureEditor.jar
