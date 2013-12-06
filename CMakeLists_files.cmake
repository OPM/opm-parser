# -*- mode: cmake; tab-width: 2; indent-tabs-mode: t; truncate-lines: t; compile-command: "cmake -Wdev" -*-
# vim: set filetype=cmake autoindent tabstop=2 shiftwidth=2 noexpandtab softtabstop=2 nowrap:

# This file sets up five lists:
#	MAIN_SOURCE_FILES     List of compilation units which will be included in
#	                      the library. If it isn't on this list, it won't be
#	                      part of the library. Please try to keep it sorted to
#	                      maintain sanity.
#
#	TEST_SOURCE_FILES     List of programs that will be run as unit tests.
#
#	TEST_DATA_FILES       Files from the source three that should be made
#	                      available in the corresponding location in the build
#	                      tree in order to run tests there.
#
#	EXAMPLE_SOURCE_FILES  Other programs that will be compiled as part of the
#	                      build, but which is not part of the library nor is
#	                      run as tests.
#
#	PUBLIC_HEADER_FILES   List of public header files that should be
#	                      distributed together with the library. The source
#	                      files can of course include other files than these;
#	                      you should only add to this list if the *user* of
#	                      the library needs it.
#
# ATTIC_FILES           Unmaintained files. This for the projects developers
#                       only. Don't expect these files to build.

# originally generated with the command:
# find opm -name '*.c*' -printf '\t%p\n' | sort
list (APPEND MAIN_SOURCE_FILES
        opm/json/cjson/cJSON.c
        opm/json/JsonObject.cpp
        opm/parser/eclipse/Applications/EclipseDataDoctor.cpp
        opm/parser/eclipse/Applications/Schedule.cpp
        opm/parser/eclipse/Deck/Deck.cpp
        opm/parser/eclipse/Deck/DeckDoubleItem.cpp
        opm/parser/eclipse/Deck/DeckIntItem.cpp
        opm/parser/eclipse/Deck/DeckItem.cpp
        opm/parser/eclipse/Deck/DeckKeyword.cpp
        opm/parser/eclipse/Deck/DeckRecord.cpp
        opm/parser/eclipse/Deck/DeckStringItem.cpp
        opm/parser/eclipse/Deck/KeywordContainer.cpp
        opm/parser/eclipse/EclipseState/Schedule/Completion.cpp
        opm/parser/eclipse/EclipseState/Schedule/CompletionSet.cpp
        opm/parser/eclipse/EclipseState/Schedule/Group.cpp
        opm/parser/eclipse/EclipseState/Schedule/GroupTree.cpp
        opm/parser/eclipse/EclipseState/Schedule/GroupTreeNode.cpp
        opm/parser/eclipse/EclipseState/Schedule/Schedule.cpp
        opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.cpp
        opm/parser/eclipse/EclipseState/Schedule/TimeMap.cpp
        opm/parser/eclipse/EclipseState/Schedule/Well.cpp                                                                                                                                                                                                                      
        opm/parser/eclipse/EclipseState/Schedule/WellSet.cpp                                                                                                                                                                                                                   
        opm/parser/eclipse/Parser/createDefaultKeywordList.cpp                                                                                                                                                                                                                 
        opm/parser/eclipse/Parser/ParserBoolItem.cpp                                                                                                                                                                                                                           
        opm/parser/eclipse/Parser/Parser.cpp                                                                                                                                                                                                                                   
        opm/parser/eclipse/Parser/ParserDoubleItem.cpp                                                                                                                                                                                                                         
        opm/parser/eclipse/Parser/ParserEnums.cpp                                                                                                                                                                                                                              
        opm/parser/eclipse/Parser/ParserIntItem.cpp                                                                                                                                                                                                                            
        opm/parser/eclipse/Parser/ParserItem.cpp                                                                                                                                                                                                                               
        opm/parser/eclipse/Parser/ParserKeyword.cpp                                                                                                                                                                                                                            
        opm/parser/eclipse/Parser/ParserRecord.cpp                                                                                                                                                                                                                             
        opm/parser/eclipse/Parser/ParserStringItem.cpp                                                                                                                                                                                                                         
        opm/parser/eclipse/RawDeck/RawKeyword.cpp
        opm/parser/eclipse/RawDeck/RawRecord.cpp
        opm/parser/eclipse/RawDeck/StarToken.cpp
	)

# originally generated with the command:
# find tests -name '*.cpp' -a ! -wholename '*/not-unit/*' -printf '\t%p\n' | sort
list (APPEND TEST_SOURCE_FILES)

# originally generated with the command:
# find tests -name '*.xml' -a ! -wholename '*/not-unit/*' -printf '\t%p\n' | sort
list (APPEND TEST_DATA_FILES)

# originally generated with the command:
list (APPEND EXAMPLE_SOURCE_FILES
        opm/parser/eclipse/Applications/EclipseDataDoctor.cpp
        opm/parser/eclipse/Applications/Schedule.cpp
)

# originally generated with the command:
# find attic -name '*.c*' -printf '\t%p\n' | sort
list (APPEND ATTIC_FILES )

# programs listed here will not only be compiled, but also marked for
# installation
# find opm/parser/eclipse/Applications -name '*.c*' -printf '\t%p\n' | sort
list (APPEND PROGRAM_SOURCE_FILES
        opm/parser/eclipse/Applications/EclipseDataDoctor.cpp
        opm/parser/eclipse/Applications/Schedule.cpp
)

# originally generated with the command:
# find opm -name '*.h*' -a ! -name '*-pch.hpp' -printf '\t%p\n' | sort
list (APPEND PUBLIC_HEADER_FILES
        opm/json/cjson/cJSON.h
        opm/json/JsonObject.hpp
        opm/parser/eclipse/Deck/DeckDoubleItem.hpp
        opm/parser/eclipse/Deck/Deck.hpp
        opm/parser/eclipse/Deck/DeckIntItem.hpp
        opm/parser/eclipse/Deck/DeckItem.hpp
        opm/parser/eclipse/Deck/DeckKeyword.hpp
        opm/parser/eclipse/Deck/DeckRecord.hpp
        opm/parser/eclipse/Deck/DeckStringItem.hpp
        opm/parser/eclipse/Deck/KeywordContainer.hpp
        opm/parser/eclipse/EclipseState/Schedule/Completion.hpp
        opm/parser/eclipse/EclipseState/Schedule/CompletionSet.hpp
        opm/parser/eclipse/EclipseState/Schedule/DynamicState.hpp
        opm/parser/eclipse/EclipseState/Schedule/Group.hpp
        opm/parser/eclipse/EclipseState/Schedule/GroupTree.hpp
        opm/parser/eclipse/EclipseState/Schedule/GroupTreeNode.hpp
        opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp
        opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp
        opm/parser/eclipse/EclipseState/Schedule/TimeMap.hpp
        opm/parser/eclipse/EclipseState/Schedule/Well.hpp
        opm/parser/eclipse/EclipseState/Schedule/WellSet.hpp
        opm/parser/eclipse/Parser/ParserBoolItem.hpp
        opm/parser/eclipse/Parser/ParserConst.hpp
        opm/parser/eclipse/Parser/ParserDoubleItem.hpp
        opm/parser/eclipse/Parser/ParserEnums.hpp
        opm/parser/eclipse/Parser/Parser.hpp
        opm/parser/eclipse/Parser/ParserIntItem.hpp
        opm/parser/eclipse/Parser/ParserItem.hpp
        opm/parser/eclipse/Parser/ParserKeyword.hpp
        opm/parser/eclipse/Parser/ParserRecord.hpp
        opm/parser/eclipse/Parser/ParserStringItem.hpp
        opm/parser/eclipse/RawDeck/RawConsts.hpp
        opm/parser/eclipse/RawDeck/RawEnums.hpp
        opm/parser/eclipse/RawDeck/RawKeyword.hpp
        opm/parser/eclipse/RawDeck/RawRecord.hpp
        opm/parser/eclipse/RawDeck/StarToken.hpp
	)
