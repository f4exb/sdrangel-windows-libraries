#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libsigmf::libsigmf" for configuration "Debug"
set_property(TARGET libsigmf::libsigmf APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libsigmf::libsigmf PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libsigmfd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libsigmfd.dll"
  )

list(APPEND _cmake_import_check_targets libsigmf::libsigmf )
list(APPEND _cmake_import_check_files_for_libsigmf::libsigmf "${_IMPORT_PREFIX}/lib/libsigmfd.lib" "${_IMPORT_PREFIX}/lib/libsigmfd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
