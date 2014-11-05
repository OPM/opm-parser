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

#define BOOST_TEST_MODULE ParserIntegrationTests
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/checkDeck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParserRecord.hpp>
#include <opm/parser/eclipse/Parser/ParserIntItem.hpp>
#include <opm/parser/eclipse/Parser/ParserStringItem.hpp>

#include <opm/parser/eclipse/Parser/ParserEnums.hpp>

using namespace Opm;

static ParserPtr createWWCTParser() {
    ParserKeywordPtr parserKeyword = ParserKeyword::createDynamicSized("WWCT");
    {
        ParserRecordPtr wwctRecord = parserKeyword->getRecord();
        wwctRecord->addItem(ParserStringItemConstPtr(new ParserStringItem("WELL", ALL)));
    }
    ParserKeywordPtr summaryKeyword = ParserKeyword::createFixedSized("SUMMARY" , (size_t) 0);

    ParserPtr parser(new Parser());
    parser->addParserKeyword(parserKeyword);
    parser->addParserKeyword(summaryKeyword);
    return parser;
}

BOOST_AUTO_TEST_CASE(parse_fileWithWWCTKeyword_deckReturned) {
    boost::filesystem::path singleKeywordFile("testdata/integration_tests/wwct.data");
    ParserPtr parser = createWWCTParser();
    BOOST_CHECK( parser->isRecognizedKeyword("WWCT"));
    BOOST_CHECK( parser->isRecognizedKeyword("SUMMARY"));
    BOOST_CHECK_NO_THROW(DeckPtr deck =  parser->parseFile(singleKeywordFile.string()));
}

BOOST_AUTO_TEST_CASE(parse_stringWithWWCTKeyword_deckReturned) {
    const char *wwctString =
        "SUMMARY\n"
        "\n"
        "-- Kommentar\n"
        "WWCT\n"
        "  'WELL-1' 'WELL-2' / -- Ehne mehne muh\n"
        "/\n";
    ParserPtr parser = createWWCTParser();
    BOOST_CHECK( parser->isRecognizedKeyword("WWCT"));
    BOOST_CHECK( parser->isRecognizedKeyword("SUMMARY"));
    BOOST_CHECK_NO_THROW(DeckPtr deck =  parser->parseString(wwctString));
}

BOOST_AUTO_TEST_CASE(parse_streamWithWWCTKeyword_deckReturned) {
    const char *wwctString =
        "SUMMARY\n"
        "\n"
        "-- Kommentar\n"
        "WWCT\n"
        "  'WELL-1' 'WELL-2' / -- Rumpelstilzchen\n"
        "/\n";
    std::shared_ptr<std::istream> wwctStream(new std::istringstream(wwctString));

    ParserPtr parser = createWWCTParser();
    BOOST_CHECK( parser->isRecognizedKeyword("WWCT"));
    BOOST_CHECK( parser->isRecognizedKeyword("SUMMARY"));
    BOOST_CHECK_NO_THROW(DeckPtr deck =  parser->parseStream(wwctStream));
}

BOOST_AUTO_TEST_CASE(parse_fileWithWWCTKeyword_deckHasWWCT) {
    boost::filesystem::path singleKeywordFile("testdata/integration_tests/wwct.data");
    ParserPtr parser = createWWCTParser();
    DeckPtr deck =  parser->parseFile(singleKeywordFile.string());
    BOOST_CHECK(deck->hasKeyword("SUMMARY"));
    BOOST_CHECK(deck->hasKeyword("WWCT"));
}

BOOST_AUTO_TEST_CASE(parse_fileWithWWCTKeyword_dataIsCorrect) {
    boost::filesystem::path singleKeywordFile("testdata/integration_tests/wwct.data");
    ParserPtr parser = createWWCTParser();
    DeckPtr deck =  parser->parseFile(singleKeywordFile.string());
    BOOST_CHECK_EQUAL("WELL-1", deck->getKeyword("WWCT" , 0)->getRecord(0)->getItem(0)->getString(0));
    BOOST_CHECK_EQUAL("WELL-2", deck->getKeyword("WWCT" , 0)->getRecord(0)->getItem(0)->getString(1));
}

BOOST_AUTO_TEST_CASE(parser_internal_name_vs_deck_name) {
    ParserPtr parser(new Opm::Parser());

    // the WELL_PROBE keyword is present by default
    BOOST_CHECK(parser->hasInternalKeyword("WELL_PROBE"));

    // the NONEXISTING_PROBE keyword is _not_ present by default
    BOOST_CHECK(!parser->hasInternalKeyword("NONEXISTING_PROBE"));

    // internal names cannot appear in the deck if the deck names and/or deck regular
    // match expressions are given
    BOOST_CHECK(!parser->isRecognizedKeyword("WELL_PROBE"));

    // an existing deck name
    BOOST_CHECK(parser->isRecognizedKeyword("WWPR"));

    // a non-existing deck name
    BOOST_CHECK(!parser->isRecognizedKeyword("WWPRFOO"));

    // user defined quantity. (regex needs to be used.)
    BOOST_CHECK(parser->isRecognizedKeyword("WUFOO"));
}

static ParserPtr createBPRParser() {
    ParserKeywordPtr parserKeyword = ParserKeyword::createDynamicSized("BPR");
    {
        ParserRecordPtr bprRecord = parserKeyword->getRecord();
        bprRecord->addItem(ParserIntItemConstPtr(new ParserIntItem("I", SINGLE)));
        bprRecord->addItem(ParserIntItemConstPtr(new ParserIntItem("J", SINGLE)));
        bprRecord->addItem(ParserIntItemConstPtr(new ParserIntItem("K", SINGLE)));
    }
    ParserKeywordPtr summaryKeyword = ParserKeyword::createFixedSized("SUMMARY" , (size_t) 0);
    ParserPtr parser(new Parser());
    parser->addParserKeyword(parserKeyword);
    parser->addParserKeyword(summaryKeyword);
    return parser;
}

BOOST_AUTO_TEST_CASE(parse_fileWithBPRKeyword_deckReturned) {
    boost::filesystem::path singleKeywordFile("testdata/integration_tests/bpr.data");
    ParserPtr parser = createBPRParser();

    BOOST_CHECK_NO_THROW(DeckPtr deck =  parser->parseFile(singleKeywordFile.string()));
}

BOOST_AUTO_TEST_CASE(parse_fileWithBPRKeyword_DeckhasBRP) {
    boost::filesystem::path singleKeywordFile("testdata/integration_tests/bpr.data");

    ParserPtr parser = createBPRParser();
    DeckPtr deck =  parser->parseFile(singleKeywordFile.string());

    BOOST_CHECK_EQUAL(true, deck->hasKeyword("BPR"));
}

BOOST_AUTO_TEST_CASE(parse_fileWithBPRKeyword_dataiscorrect) {
    boost::filesystem::path singleKeywordFile("testdata/integration_tests/bpr.data");

    ParserPtr parser = createBPRParser();
    DeckPtr deck =  parser->parseFile(singleKeywordFile.string());

    DeckKeywordConstPtr keyword = deck->getKeyword("BPR" , 0);
    BOOST_CHECK_EQUAL(2U, keyword->size());

    DeckRecordConstPtr record1 = keyword->getRecord(0);
    BOOST_CHECK_EQUAL(3U, record1->size());

    DeckItemConstPtr I1 = record1->getItem(0);
    BOOST_CHECK_EQUAL(1, I1->getInt(0));
    I1 = record1->getItem("I");
    BOOST_CHECK_EQUAL(1, I1->getInt(0));

    DeckItemConstPtr J1 = record1->getItem(1);
    BOOST_CHECK_EQUAL(2, J1->getInt(0));
    J1 = record1->getItem("J");
    BOOST_CHECK_EQUAL(2, J1->getInt(0));

    DeckItemConstPtr K1 = record1->getItem(2);
    BOOST_CHECK_EQUAL(3, K1->getInt(0));
    K1 = record1->getItem("K");
    BOOST_CHECK_EQUAL(3, K1->getInt(0));


    DeckRecordConstPtr record2 = keyword->getRecord(0);
    BOOST_CHECK_EQUAL(3U, record2->size());

    I1 = record2->getItem(0);
    BOOST_CHECK_EQUAL(1, I1->getInt(0));
    I1 = record2->getItem("I");
    BOOST_CHECK_EQUAL(1, I1->getInt(0));

    J1 = record2->getItem(1);
    BOOST_CHECK_EQUAL(2, J1->getInt(0));
    J1 = record2->getItem("J");
    BOOST_CHECK_EQUAL(2, J1->getInt(0));

    K1 = record2->getItem(2);
    BOOST_CHECK_EQUAL(3, K1->getInt(0));
    K1 = record2->getItem("K");
    BOOST_CHECK_EQUAL(3, K1->getInt(0));
}


/***************** Testing non-recognized keywords ********************/
BOOST_AUTO_TEST_CASE(parse_unknownkeyword) {
    ParserPtr parser(new Parser());
    DeckPtr deck =  parser->parseFile("testdata/integration_tests/someobscureelements.data");
    BOOST_CHECK_EQUAL(4U, deck->size());
    DeckKeywordConstPtr unknown = deck->getKeyword("GRUDINT");
    BOOST_CHECK(!unknown->isKnown());
    ParserLogPtr parserLog(new ParserLog());
    BOOST_CHECK(!Opm::checkDeck(deck, parserLog, Opm::UnknownKeywords));
}

/*********************Testing truncated (default) records ***************************/


// Datafile contains 3 RADFIN4 keywords. One fully specified, one with 2 out of 11 items, and one with no items.
BOOST_AUTO_TEST_CASE(parse_truncatedrecords_deckFilledWithDefaults) {
    ParserPtr parser(new Parser());
    DeckPtr deck =  parser->parseFile("testdata/integration_tests/truncated_records.data");
    BOOST_CHECK_EQUAL(4U, deck->size());
    DeckKeywordConstPtr radfin4_0_full= deck->getKeyword("RADFIN4", 0);
    DeckKeywordConstPtr radfin4_1_partial= deck->getKeyword("RADFIN4", 1);
    DeckKeywordConstPtr radfin4_2_nodata= deck->getKeyword("RADFIN4", 2);

    // Specified in datafile
    BOOST_CHECK_EQUAL("NAME", radfin4_0_full->getRecord(0)->getItem(0)->getString(0));
    BOOST_CHECK_EQUAL("NAME", radfin4_1_partial->getRecord(0)->getItem(0)->getString(0));
    // Default string

    BOOST_CHECK_NO_THROW(radfin4_2_nodata->getRecord(0)->getItem(0)->getString(0));
    BOOST_CHECK( radfin4_2_nodata->getRecord(0)->getItem(0)->defaultApplied(0));

    
    // Specified in datafile
    BOOST_CHECK_EQUAL(213, radfin4_0_full->getRecord(0)->getItem(1)->getInt(0));
    BOOST_CHECK_EQUAL(213, radfin4_1_partial->getRecord(0)->getItem(1)->getInt(0));
    // Default int
    BOOST_CHECK_NO_THROW( radfin4_2_nodata->getRecord(0)->getItem(1)->getInt(0));
    BOOST_CHECK( radfin4_2_nodata->getRecord(0)->getItem(1)->defaultApplied(0));
    
    
    ParserKeywordConstPtr parserKeyword = parser->getParserKeywordFromDeckName("RADFIN4");
    ParserRecordConstPtr parserRecord = parserKeyword->getRecord();
    ParserItemConstPtr nwmaxItem = parserRecord->get("NWMAX");
    ParserIntItemConstPtr intItem = std::static_pointer_cast<const ParserIntItem>(nwmaxItem);
    
    BOOST_CHECK_EQUAL(18, radfin4_0_full->getRecord(0)->getItem(10)->getInt(0));
    BOOST_CHECK_EQUAL(intItem->getDefault(), radfin4_1_partial->getRecord(0)->getItem(10)->getInt(0));
    BOOST_CHECK_EQUAL(intItem->getDefault(), radfin4_2_nodata->getRecord(0)->getItem(10)->getInt(0));
    
}
