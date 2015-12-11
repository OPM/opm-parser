foreach(tapp TableManagerTests TabdimsTests PvtxTableTests TableContainerTests
             TableColumnTests ColumnSchemaTests)

  opm_add_test(run${tapp} SOURCES ${tapp}.cpp
                          LIBRARIES opmparser ${Boost_LIBRARIES})

endforeach()
