#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "codec2" for configuration "Release"
set_property(TARGET codec2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(codec2 PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/codec2.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/codec2.dll"
  )

list(APPEND _cmake_import_check_targets codec2 )
list(APPEND _cmake_import_check_files_for_codec2 "${_IMPORT_PREFIX}/lib/codec2.lib" "${_IMPORT_PREFIX}/bin/codec2.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
