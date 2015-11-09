foreach(tapp TableManagerTests TabdimsTests MultiRecordTableTests TableContainerTests
             TableColumnTests ColumnSchemaTests)

  opm_add_test(run${tapp} SOURCES ${tapp}.cpp
                          LIBRARIES opmparser ${Boost_LIBRARIES})

endforeach()
