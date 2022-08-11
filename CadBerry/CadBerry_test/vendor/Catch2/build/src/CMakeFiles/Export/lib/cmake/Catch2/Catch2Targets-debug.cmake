#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Catch2::Catch2" for configuration "Debug"
set_property(TARGET Catch2::Catch2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Catch2::Catch2 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Catch2d.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Catch2::Catch2 )
list(APPEND _IMPORT_CHECK_FILES_FOR_Catch2::Catch2 "${_IMPORT_PREFIX}/lib/Catch2d.lib" )

# Import target "Catch2::Catch2WithMain" for configuration "Debug"
set_property(TARGET Catch2::Catch2WithMain APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Catch2::Catch2WithMain PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/Catch2Maind.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS Catch2::Catch2WithMain )
list(APPEND _IMPORT_CHECK_FILES_FOR_Catch2::Catch2WithMain "${_IMPORT_PREFIX}/lib/Catch2Maind.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
