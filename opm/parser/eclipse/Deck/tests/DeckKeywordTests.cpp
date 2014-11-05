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


#define BOOST_TEST_MODULE DeckKeywordTests

#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

using namespace Opm;

BOOST_AUTO_TEST_CASE(Initialize) {
    DeckKeyword deckKeyword1("KW");
    DeckKeywordPtr deckKeyword2(new DeckKeyword("KW"));
    DeckKeywordConstPtr deckKeyword3(new DeckKeyword("KW"));
}

BOOST_AUTO_TEST_CASE(DataKeyword) {
    DeckKeyword kw("KW");
    BOOST_CHECK_EQUAL( false , kw.isDataKeyword());
    kw.setDataKeyword( );
    BOOST_CHECK_EQUAL( true , kw.isDataKeyword());
    kw.setDataKeyword( false );
    BOOST_CHECK_EQUAL( false , kw.isDataKeyword());
    kw.setDataKeyword( true );
    BOOST_CHECK_EQUAL( true , kw.isDataKeyword());
}



BOOST_AUTO_TEST_CASE(name_nameSetInConstructor_nameReturned) {
    DeckKeywordPtr deckKeyword(new DeckKeyword("KW"));
    BOOST_CHECK_EQUAL("KW", deckKeyword->name());
}

BOOST_AUTO_TEST_CASE(size_noRecords_returnszero) {
    DeckKeywordPtr deckKeyword(new DeckKeyword("KW"));
    BOOST_CHECK_EQUAL(0U, deckKeyword->size());
}


BOOST_AUTO_TEST_CASE(addRecord_onerecord_recordadded) {
    ParserLogPtr parserLog(new ParserLog);
    DeckKeywordPtr deckKeyword(new DeckKeyword("KW"));
    deckKeyword->addRecord(DeckRecordConstPtr(new DeckRecord(parserLog, "", -1)));
    BOOST_CHECK_EQUAL(1U, deckKeyword->size());
    for (auto iter = deckKeyword->begin(); iter != deckKeyword->end(); ++iter) {
        // 
    }

}

BOOST_AUTO_TEST_CASE(getRecord_onerecord_recordretured) {
    ParserLogPtr parserLog(new ParserLog);
    DeckKeywordPtr deckKeyword(new DeckKeyword("KW"));
    DeckRecordConstPtr deckRecord(new DeckRecord(parserLog, "", -1));
    deckKeyword->addRecord(deckRecord);
    BOOST_CHECK_EQUAL(deckRecord, deckKeyword->getRecord(0));
}


BOOST_AUTO_TEST_CASE(getRecord_outofrange_exceptionthrown) {
    ParserLogPtr parserLog(new ParserLog);
    DeckKeywordPtr deckKeyword(new DeckKeyword("KW"));
    DeckRecordConstPtr deckRecord(new DeckRecord(parserLog, "", -1));
    deckKeyword->addRecord(deckRecord);
    BOOST_CHECK_THROW(deckKeyword->getRecord(1), std::range_error);
}

BOOST_AUTO_TEST_CASE(setUnknown_wasknown_nowunknown) {
    DeckKeywordPtr deckKeyword(new DeckKeyword("KW", false));
    BOOST_CHECK(!deckKeyword->isKnown());
}


BOOST_AUTO_TEST_CASE(DeckIndex) {
    DeckKeywordPtr deckKeyword(new DeckKeyword("KW"));
    BOOST_CHECK_EQUAL( -1 , deckKeyword->getDeckIndex());
    deckKeyword->setDeckIndex( 10);
    BOOST_CHECK_EQUAL(10 , deckKeyword->getDeckIndex());
}









