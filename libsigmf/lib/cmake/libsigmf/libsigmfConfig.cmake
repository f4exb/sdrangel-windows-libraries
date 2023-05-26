########################################################################
#
# Copyright 2019 DeepSig Inc.
#
# SPDX-License-Identifier: Apache 2.0
#
########################################################################
include(FindPackageHandleStandardArgs)
set(${CMAKE_FIND_PACKAGE_NAME}_CONFIG ${CMAKE_CURRENT_LIST_FILE})
find_package_handle_standard_args(libsigmf CONFIG_MODE)

if(NOT TARGET libsigmf::libsigmf)
  include(CMakeFindDependencyMacro)

  find_dependency(nlohmann_json REQUIRED)
  find_dependency(Flatbuffers REQUIRED)
  include("${CMAKE_CURRENT_LIST_DIR}/libsigmfTargets.cmake")
endif()
