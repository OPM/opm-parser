# defines that must be present in config.h for our headers
set (opm-parser_CONFIG_VAR
  HAVE_ERT
  )

# dependencies
set (opm-parser_DEPS
  # compile with C++0x/11 support if available
  "CXX11Features REQUIRED"
  # various runtime library enhancements
  "Boost 1.44.0
    COMPONENTS system filesystem regex unit_test_framework REQUIRED"
  # Ensembles-based Reservoir Tools (ERT)
  "ecl REQUIRED"
  # System cjson library
  "cjson"
  )

find_package_deps(opm-parser)
