foreach(tapp ParserTests ParserKeywordTests ParserRecordTests
             ParserItemTests ParserEnumTests ParserIncludeTests
             ParseContextTests MessageContainerTest
             DeckTests)
  opm_add_test(run${tapp} SOURCES ${tapp}.cpp
                          LIBRARIES opmparser ${Boost_LIBRARIES})
endforeach()
if (NOT MSVC)
    set_property(SOURCE ParserRecordTests.cpp PROPERTY COMPILE_FLAGS "-Wno-error")
endif()

