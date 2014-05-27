# this file tells CTest where it should upload its results. To run
# CTest manually, run
#
#  ctest -D Experimental
#

set(CTEST_PROJECT_NAME "opm-parser")
set(CTEST_NIGHTLY_START_TIME "00:01:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "opm-project.org")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=opm-parser")
set(CTEST_DROP_SITE_CDASH TRUE)
