# .. cmake_module::
#
# This module's content is executed whenever a Dune module requires or
# suggests opm-parser!
#

find_package(ecl REQUIRED)

if (ecl_FOUND)
  set(HAVE_ERT "1")
  set(HAVE_LIBECL "1")
endif()

find_package(Boost
  COMPONENTS regex filesystem system date_time
  REQUIRED)
