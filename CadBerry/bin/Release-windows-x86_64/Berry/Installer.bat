::This script makes Cadberry_Updater.exe download a minimal CadBerry 
::install

::Set up the folder structure (will fix this eventually)
mkdir Build
mkdir Data
cd Data
mkdir data_tables
cd data_tables
mkdir dists
mkdir rsample
cd ..
cd ..
mkdir GILModules
mkdir Modules
mkdir Tests

::Download components
CadBerry_updater "https://github.com/CamelCaseCam/CadBerry/raw/beta/CadBerry/bin/Release-windows-x86_64/Berry/" %cd%/ 9 "Berry.exe" "Modules/Core.dll" "CadBerry.dll" "CadBerry.CDBPKG" "CDBRNA.dll" "cpr.dll" "libcurl.dll" "zlib.dll"