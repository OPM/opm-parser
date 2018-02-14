# -*- mode: cmake; tab-width: 2; indent-tabs-mode: t; truncate-lines: t; compile-command: "cmake -Wdev" -*-
# vim: set filetype=cmake autoindent tabstop=2 shiftwidth=2 noexpandtab softtabstop=2 nowrap:

list(APPEND MAIN_SOURCE_FILES
  lib/eclipse/Deck/Deck.cpp
  lib/eclipse/Deck/DeckItem.cpp
  lib/eclipse/Deck/DeckKeyword.cpp
  lib/eclipse/Deck/DeckRecord.cpp
  lib/eclipse/Deck/DeckOutput.cpp
  lib/eclipse/Deck/Section.cpp
  lib/eclipse/EclipseState/Aquancon.cpp
  lib/eclipse/EclipseState/checkDeck.cpp
  lib/eclipse/EclipseState/Eclipse3DProperties.cpp
  lib/eclipse/EclipseState/EclipseConfig.cpp
  lib/eclipse/EclipseState/EclipseState.cpp
  lib/eclipse/EclipseState/EndpointScaling.cpp
  lib/eclipse/EclipseState/Grid/Box.cpp
  lib/eclipse/EclipseState/Grid/BoxManager.cpp
  lib/eclipse/EclipseState/Grid/EclipseGrid.cpp
  lib/eclipse/EclipseState/Grid/FaceDir.cpp
  lib/eclipse/EclipseState/Grid/FaultCollection.cpp
  lib/eclipse/EclipseState/Grid/Fault.cpp
  lib/eclipse/EclipseState/Grid/FaultFace.cpp
  lib/eclipse/EclipseState/Grid/GridDims.cpp
  lib/eclipse/EclipseState/Grid/GridProperties.cpp
  lib/eclipse/EclipseState/Grid/GridProperty.cpp
  lib/eclipse/EclipseState/Grid/MULTREGTScanner.cpp
  lib/eclipse/EclipseState/Grid/NNC.cpp
  lib/eclipse/EclipseState/Grid/PinchMode.cpp
  lib/eclipse/EclipseState/Grid/SatfuncPropertyInitializers.cpp
  lib/eclipse/EclipseState/Grid/setKeywordBox.cpp
  lib/eclipse/EclipseState/Grid/TransMult.cpp
  lib/eclipse/EclipseState/InitConfig/Equil.cpp
  lib/eclipse/EclipseState/InitConfig/InitConfig.cpp
  lib/eclipse/EclipseState/IOConfig/IOConfig.cpp
  lib/eclipse/EclipseState/IOConfig/RestartConfig.cpp
  lib/eclipse/EclipseState/Runspec.cpp
  lib/eclipse/EclipseState/Schedule/Completion.cpp
  lib/eclipse/EclipseState/Schedule/CompletionSet.cpp
  lib/eclipse/EclipseState/Schedule/Events.cpp
  lib/eclipse/EclipseState/Schedule/Group.cpp
  lib/eclipse/EclipseState/Schedule/GroupTree.cpp
  lib/eclipse/EclipseState/Schedule/MessageLimits.cpp
  lib/eclipse/EclipseState/Schedule/MSW/Compsegs.cpp
  lib/eclipse/EclipseState/Schedule/MSW/Segment.cpp
  lib/eclipse/EclipseState/Schedule/MSW/SegmentSet.cpp
  lib/eclipse/EclipseState/Schedule/MSW/updatingCompletionsWithSegments.cpp
  lib/eclipse/EclipseState/Schedule/OilVaporizationProperties.cpp
  lib/eclipse/EclipseState/Schedule/Schedule.cpp
  lib/eclipse/EclipseState/Schedule/ScheduleEnums.cpp
  lib/eclipse/EclipseState/Schedule/TimeMap.cpp
  lib/eclipse/EclipseState/Schedule/Tuning.cpp
  lib/eclipse/EclipseState/Schedule/Well.cpp
  lib/eclipse/EclipseState/Schedule/WellEconProductionLimits.cpp
  lib/eclipse/EclipseState/Schedule/WellInjectionProperties.cpp
  lib/eclipse/EclipseState/Schedule/WellPolymerProperties.cpp
  lib/eclipse/EclipseState/Schedule/WellProductionProperties.cpp
  lib/eclipse/EclipseState/SimulationConfig/SimulationConfig.cpp
  lib/eclipse/EclipseState/SimulationConfig/ThresholdPressure.cpp
  lib/eclipse/EclipseState/SummaryConfig/SummaryConfig.cpp
  lib/eclipse/EclipseState/Tables/ColumnSchema.cpp
  lib/eclipse/EclipseState/Tables/JFunc.cpp
  lib/eclipse/EclipseState/Tables/PvtxTable.cpp
  lib/eclipse/EclipseState/Tables/SimpleTable.cpp
  lib/eclipse/EclipseState/Tables/TableColumn.cpp
  lib/eclipse/EclipseState/Tables/TableContainer.cpp
  lib/eclipse/EclipseState/Tables/TableIndex.cpp
  lib/eclipse/EclipseState/Tables/TableManager.cpp
  lib/eclipse/EclipseState/Tables/TableSchema.cpp
  lib/eclipse/EclipseState/Tables/Tables.cpp
  lib/eclipse/EclipseState/Tables/VFPInjTable.cpp
  lib/eclipse/EclipseState/Tables/VFPProdTable.cpp
  lib/eclipse/Parser/MessageContainer.cpp
  lib/eclipse/Parser/ParseContext.cpp
  lib/eclipse/Parser/Parser.cpp
  lib/eclipse/Parser/ParserEnums.cpp
  lib/eclipse/Parser/ParserItem.cpp
  lib/eclipse/Parser/ParserKeyword.cpp
  lib/eclipse/Parser/ParserRecord.cpp
  lib/eclipse/RawDeck/RawKeyword.cpp
  lib/eclipse/RawDeck/RawRecord.cpp
  lib/eclipse/RawDeck/StarToken.cpp
  lib/eclipse/Units/Dimension.cpp
  lib/eclipse/Units/UnitSystem.cpp
  lib/eclipse/Utility/Functional.cpp
  lib/eclipse/Utility/Stringview.cpp
)

# For now, we use full directory installs from install_hook
list (APPEND PUBLIC_HEADER_FILES
)

list (APPEND EXAMPLE_SOURCE_FILES
  applications/opmi.cpp
)

# programs listed here will not only be compiled, but also marked for
# installation
list (APPEND PROGRAM_SOURCE_FILES
  applications/opmi.cpp
)

list (APPEND TEST_SOURCE_FILES
  lib/eclipse/tests/ADDREGTests.cpp
  lib/eclipse/tests/AqudimsTests.cpp
  lib/eclipse/tests/AquanconTests.cpp
  lib/eclipse/tests/BoxTests.cpp
  lib/eclipse/tests/ColumnSchemaTests.cpp
  lib/eclipse/tests/CompletionTests.cpp
  lib/eclipse/tests/COMPSEGUnits.cpp
  lib/eclipse/tests/CopyRegTests.cpp
  lib/eclipse/tests/DeckTests.cpp
  lib/eclipse/tests/DynamicStateTests.cpp
  lib/eclipse/tests/DynamicVectorTests.cpp
  lib/eclipse/tests/Eclipse3DPropertiesTests.cpp
  lib/eclipse/tests/EclipseGridTests.cpp
  lib/eclipse/tests/EqualRegTests.cpp
  lib/eclipse/tests/EventTests.cpp
  lib/eclipse/tests/FaceDirTests.cpp
  lib/eclipse/tests/FaultTests.cpp
  lib/eclipse/tests/FunctionalTests.cpp
  lib/eclipse/tests/GeomodifierTests.cpp
  lib/eclipse/tests/GridPropertyTests.cpp
  lib/eclipse/tests/GroupTests.cpp
  lib/eclipse/tests/InitConfigTest.cpp
  lib/eclipse/tests/IOConfigTests.cpp
  lib/eclipse/tests/MessageContainerTest.cpp
  lib/eclipse/tests/MessageLimitTests.cpp
  lib/eclipse/tests/MultiRegTests.cpp
  lib/eclipse/tests/MultisegmentWellTests.cpp
  lib/eclipse/tests/MULTREGTScannerTests.cpp
  lib/eclipse/tests/OrderedMapTests.cpp
  lib/eclipse/tests/ParseContextTests.cpp
  lib/eclipse/tests/PORVTests.cpp
  lib/eclipse/tests/RawKeywordTests.cpp
  lib/eclipse/tests/RestartConfigTests.cpp
  lib/eclipse/tests/RunspecTests.cpp
  lib/eclipse/tests/SatfuncPropertyInitializersTests.cpp
  lib/eclipse/tests/ScheduleTests.cpp
  lib/eclipse/tests/SectionTests.cpp
  lib/eclipse/tests/SimpleTableTests.cpp
  lib/eclipse/tests/SimulationConfigTest.cpp
  lib/eclipse/tests/StarTokenTests.cpp
  lib/eclipse/tests/StringTests.cpp
  lib/eclipse/tests/SummaryConfigTests.cpp
  lib/eclipse/tests/TabdimsTests.cpp
  lib/eclipse/tests/TableColumnTests.cpp
  lib/eclipse/tests/TableContainerTests.cpp
  lib/eclipse/tests/TableManagerTests.cpp
  lib/eclipse/tests/TableSchemaTests.cpp
  lib/eclipse/tests/ThresholdPressureTest.cpp
  lib/eclipse/tests/TimeMapTest.cpp
  lib/eclipse/tests/TransMultTests.cpp
  lib/eclipse/tests/TuningTests.cpp
  lib/eclipse/tests/UnitTests.cpp
  lib/eclipse/tests/ValueTests.cpp
  lib/eclipse/tests/WellSolventTests.cpp
  lib/eclipse/tests/WellTests.cpp
)

list (APPEND TEST_DATA_FILES
)
