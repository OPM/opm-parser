/*
  Copyright 2013 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
 */

#define BOOST_TEST_MODULE RawKeywordTests
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include <opm/parser/eclipse/RawDeck/RawKeyword.hpp>
#include <opm/parser/eclipse/RawDeck/RawEnums.hpp>


using namespace Opm;

BOOST_AUTO_TEST_CASE(RawKeywordGiveKeywordToConstructorKeywordSet) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("KEYYWORD", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    BOOST_CHECK(keyword.getKeywordName() == "KEYYWORD");
    BOOST_CHECK_EQUAL(Raw::SLASH_TERMINATED , keyword.getSizeType());
}

BOOST_AUTO_TEST_CASE(RawKeywordSizeTypeInvalidThrows) {
    ParserLogPtr parserLog(new ParserLog());
    BOOST_CHECK_THROW( RawKeyword("KEYYWORD", Raw::FIXED , "FILE" , 0U, parserLog) , std::invalid_argument);
    BOOST_CHECK_THROW( RawKeyword("KEYYWORD", Raw::TABLE_COLLECTION , "FILE" , 10U, parserLog) , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(RawKeywordFinalizeWrongSizeTYpeThrows) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword kw("KEYYWORD", Raw::SLASH_TERMINATED , "FILE" , 0U, parserLog);
    BOOST_CHECK_THROW(     kw.finalizeUnknownSize() , std::invalid_argument );
}


BOOST_AUTO_TEST_CASE(RawKeywordFinalizeUnknownSize) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword kw("KEYYWORD", Raw::UNKNOWN , "FILE" , 0U, parserLog);
    BOOST_CHECK( !kw.isFinished() );
    kw.finalizeUnknownSize();
    BOOST_CHECK( kw.isFinished() );
}




BOOST_AUTO_TEST_CASE(RawKeywordGiveKeywordToConstructorTooLongThrows) {
    ParserLogPtr parserLog(new ParserLog());
    BOOST_CHECK_THROW(RawKeyword keyword("KEYYYWORD", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(RawKeywordSetKeywordInitialWhitespaceInKeywordThrows) {
    ParserLogPtr parserLog(new ParserLog());
    BOOST_CHECK_THROW(RawKeyword(" TELONG", Raw::SLASH_TERMINATED, "FILE" , 10U, parserLog), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(constructor_mixedCaseName_throws) {
    // raw keywords may be lower-case even if this is not allowed in valid deck
    // names... (this will produce a warning if the deck is checked.)
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword("Test", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    RawKeyword("test", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    BOOST_CHECK_THROW(RawKeyword keyword("1test", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(RawKeywordSetKeywordInitialTabInKeywordThrows) {
    ParserLogPtr parserLog(new ParserLog());
    BOOST_CHECK_THROW( RawKeyword("\tTELONG", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(RawKeywordSetCorrectLenghtKeywordNoError) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("GOODONE", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    BOOST_CHECK(keyword.getKeywordName() == "GOODONE");
}

BOOST_AUTO_TEST_CASE(RawKeywordSet8CharKeywordWithTrailingWhitespaceKeywordTrimmed) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("GOODONEE ", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    BOOST_CHECK(keyword.getKeywordName() == "GOODONEE");
}


BOOST_AUTO_TEST_CASE(addRecord_singleRecord_recordAdded) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("TEST", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    keyword.addRawRecordString("test 1 3 4 /");
    BOOST_CHECK_EQUAL(1U, keyword.size());
}

BOOST_AUTO_TEST_CASE(getRecord_outOfRange_throws) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("TEST", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    keyword.addRawRecordString("test 1 3 4 /");
    BOOST_CHECK_THROW(keyword.getRecord(1), std::range_error);
}



BOOST_AUTO_TEST_CASE(isFinished_undef_size) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("TEST", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);

    BOOST_CHECK(  !keyword.isFinished() );
    keyword.addRawRecordString("test 1 3 4 /");
    keyword.addRawRecordString("test 1 3 4");
    keyword.addRawRecordString("test 1 3 4");
    BOOST_CHECK(  !keyword.isFinished() );
    keyword.addRawRecordString("/");
    BOOST_CHECK(  !keyword.isFinished() );
    keyword.addRawRecordString("/");
    BOOST_CHECK(  keyword.isFinished() );
}


BOOST_AUTO_TEST_CASE(isFinished_Fixedsize0) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("TEST" , "FILE" , 10U, parserLog, 0U);
    
    BOOST_CHECK(  keyword.isFinished() );
}


BOOST_AUTO_TEST_CASE(isFinished_Fixedsize1) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("TEST" , "FILE" , 10U, parserLog, 1U);
    BOOST_CHECK(  !keyword.isFinished() );
    keyword.addRawRecordString("test 1 3 4 /");
    BOOST_CHECK(  keyword.isFinished() );
}


BOOST_AUTO_TEST_CASE(isFinished_FixedsizeMulti) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("TEST", "FILE" , 10U, parserLog, 4U);
    BOOST_CHECK(  !keyword.isFinished() );
    keyword.addRawRecordString("test 1 3 4 /");
    BOOST_CHECK(  !keyword.isFinished() );

    keyword.addRawRecordString("/");
    BOOST_CHECK(  !keyword.isFinished() );

    keyword.addRawRecordString("1 2 3 3 4");
    BOOST_CHECK(  !keyword.isFinished() );
    keyword.addRawRecordString("1 2 3 3 4 /");
    BOOST_CHECK(  !keyword.isFinished() );
    keyword.addRawRecordString("1 2 3 3 /");
    BOOST_CHECK(  keyword.isFinished() );
    
    RawRecordConstPtr record = keyword.getRecord(3);
    BOOST_CHECK_EQUAL( 4U , record->size() );

}



BOOST_AUTO_TEST_CASE(isKeywordTerminator) {
    BOOST_CHECK( RawKeyword::isTerminator("/"));
    BOOST_CHECK( RawKeyword::isTerminator("  /"));
    BOOST_CHECK( RawKeyword::isTerminator("/  "));
    BOOST_CHECK( RawKeyword::isTerminator("  /"));
    BOOST_CHECK( RawKeyword::isTerminator("  /"));

    BOOST_CHECK( !RawKeyword::isTerminator("  X/  "));
}

BOOST_AUTO_TEST_CASE(isTableCollection) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword1("TEST" , "FILE" , 10U, parserLog, 4U, false);
    RawKeyword keyword2("TEST2", Raw::SLASH_TERMINATED , "FILE" , 10U, parserLog);
    BOOST_CHECK_EQUAL( Raw::FIXED , keyword1.getSizeType());
    BOOST_CHECK_EQUAL( Raw::SLASH_TERMINATED , keyword2.getSizeType());
 }


BOOST_AUTO_TEST_CASE(CreateTableCollection) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword1("TEST" , "FILE" , 10U, parserLog, 2, true);
    BOOST_CHECK_EQUAL( Raw::TABLE_COLLECTION , keyword1.getSizeType());
}


BOOST_AUTO_TEST_CASE(CreateWithFileAndLine) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword1("TEST" , Raw::SLASH_TERMINATED , "XXX", 100, parserLog);
    BOOST_CHECK_EQUAL( "XXX" , keyword1.getFilename());
    BOOST_CHECK_EQUAL( 100U , keyword1.getLineNR() );
}

BOOST_AUTO_TEST_CASE(isUnknownSize) {
    ParserLogPtr parserLog(new ParserLog());
    RawKeyword keyword("TEST2", Raw::UNKNOWN , "FILE" , 10U, parserLog);
    BOOST_CHECK_EQUAL( Raw::UNKNOWN  , keyword.getSizeType( ));
 }

