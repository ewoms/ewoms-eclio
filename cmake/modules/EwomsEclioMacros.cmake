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

# handle boost
find_package(Boost COMPONENTS regex system filesystem REQUIRED)
dune_register_package_flags(
  LIBRARIES "${Boost_LIBRARIES}"
  INCLUDE_DIRS "${Boost_INCLUDE_DIRS}")
