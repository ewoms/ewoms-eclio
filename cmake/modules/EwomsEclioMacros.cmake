# .. cmake_module::
#
# This module's content is executed whenever a Dune module requires or
# suggests ewoms-eclio!
#

# since ECL file I/O is the whole point of the module, we always want
# to enable it, thank you.
set(ENABLE_ECL_INPUT ON)
set(ENABLE_ECL_OUTPUT ON)
set(HAVE_ECL_INPUT ON)
set(HAVE_ECL_OUTPUT ON)

# we need support for either std::filesystem or std::experimental::filesystem
find_package(StdFilesystem REQUIRED)
if(STD_FILESYSTEM_LIBRARIES)
  dune_register_package_flags(
    LIBRARIES "${STD_FILESYSTEM_LIBRARIES}")
endif()

# handle boost: the base package which contains the template libraries
# is currently still required for ewoms-eclio
find_package(Boost REQUIRED)
dune_register_package_flags(
  INCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
