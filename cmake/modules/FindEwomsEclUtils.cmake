# - Try to find the utilities that come with ewoms-eclio
#
#  EWOMS_ECLIO_UTILS_FOUND - the ewoms-eclio utilities are available
#  COMPARE_ECL             - the full path to the compareECL executable
#
#  Usage:
#  find_package(EwomsEclioUtils)
find_program(COMPARE_ECL compare_ecl
  HINTS
  "${ewoms-eclio_DIR}"
  "${ewoms-eclio_DIR}/bin"
  "${ewoms-eclio_PREFIX}/bin")

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  EWOMS_ECLIO_UTILS
  "The ewoms-eclio utilities could not be found. Probably you need to install ewoms-eclio." COMPARE_ECL)

# stop the dune build system from complaining if the ewoms-eclio
# utilities were found
if (EWOMS_ECL_UTILS_FOUND)
  set(EwomsEclioUtils_FOUND 1)
endif()
