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


#define BOOST_TEST_MODULE ParserEnumTests
#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/Parser/ParserEnums.hpp>

using namespace Opm;

BOOST_AUTO_TEST_CASE(TestItemSizeEnumToString) {
    BOOST_CHECK_EQUAL( "ALL"    , ParserItemSizeEnumToString(ALL));
    BOOST_CHECK_EQUAL( "SINGLE" , ParserItemSizeEnumToString(SINGLE));
}


BOOST_AUTO_TEST_CASE(TestItemSizeEnumFromString) {
    BOOST_CHECK_THROW( ParserItemSizeEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( ALL , ParserItemSizeEnumFromString("ALL"));
    BOOST_CHECK_EQUAL( SINGLE , ParserItemSizeEnumFromString("SINGLE"));
}



BOOST_AUTO_TEST_CASE(TestItemSizeEnumLoop) {
    BOOST_CHECK_EQUAL( ALL    , ParserItemSizeEnumFromString( ParserItemSizeEnumToString( ALL ) ));
    BOOST_CHECK_EQUAL( SINGLE , ParserItemSizeEnumFromString( ParserItemSizeEnumToString( SINGLE ) ));

    BOOST_CHECK_EQUAL( "ALL"    , ParserItemSizeEnumToString(ParserItemSizeEnumFromString(  "ALL" ) ));
    BOOST_CHECK_EQUAL( "SINGLE" , ParserItemSizeEnumToString(ParserItemSizeEnumFromString(  "SINGLE" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestKeywordSizeEnumToString) {
    BOOST_CHECK_EQUAL( "SLASH_TERMINATED" , ParserKeywordSizeEnumToString(SLASH_TERMINATED));
    BOOST_CHECK_EQUAL( "FIXED"     , ParserKeywordSizeEnumToString(FIXED));
    BOOST_CHECK_EQUAL( "OTHER_KEYWORD_IN_DECK"     , ParserKeywordSizeEnumToString(OTHER_KEYWORD_IN_DECK));
    BOOST_CHECK_EQUAL( "UNKNOWN"     , ParserKeywordSizeEnumToString(UNKNOWN));
}


BOOST_AUTO_TEST_CASE(TestKeywordSizeEnumFromString) {
    BOOST_CHECK_THROW( ParserKeywordSizeEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( FIXED     , ParserKeywordSizeEnumFromString("FIXED"));
    BOOST_CHECK_EQUAL( SLASH_TERMINATED , ParserKeywordSizeEnumFromString("SLASH_TERMINATED"));
    BOOST_CHECK_EQUAL( "OTHER_KEYWORD_IN_DECK"     , ParserKeywordSizeEnumToString(OTHER_KEYWORD_IN_DECK));
    BOOST_CHECK_EQUAL( "UNKNOWN"     , ParserKeywordSizeEnumToString(UNKNOWN));
}



BOOST_AUTO_TEST_CASE(TestKeywordSizeEnumLoop) {
    BOOST_CHECK_EQUAL( FIXED     , ParserKeywordSizeEnumFromString( ParserKeywordSizeEnumToString( FIXED ) ));
    BOOST_CHECK_EQUAL( SLASH_TERMINATED , ParserKeywordSizeEnumFromString( ParserKeywordSizeEnumToString( SLASH_TERMINATED ) ));
    BOOST_CHECK_EQUAL( OTHER_KEYWORD_IN_DECK     , ParserKeywordSizeEnumFromString( ParserKeywordSizeEnumToString( OTHER_KEYWORD_IN_DECK ) ));
    BOOST_CHECK_EQUAL( UNKNOWN , ParserKeywordSizeEnumFromString( ParserKeywordSizeEnumToString( UNKNOWN ) ));
    
    BOOST_CHECK_EQUAL( "FIXED"     , ParserKeywordSizeEnumToString(ParserKeywordSizeEnumFromString(  "FIXED" ) ));
    BOOST_CHECK_EQUAL( "SLASH_TERMINATED" , ParserKeywordSizeEnumToString(ParserKeywordSizeEnumFromString(  "SLASH_TERMINATED" ) ));
    BOOST_CHECK_EQUAL( "OTHER_KEYWORD_IN_DECK"     , ParserKeywordSizeEnumToString(ParserKeywordSizeEnumFromString(  "OTHER_KEYWORD_IN_DECK" ) ));
    BOOST_CHECK_EQUAL( "UNKNOWN" , ParserKeywordSizeEnumToString(ParserKeywordSizeEnumFromString(  "UNKNOWN" ) ));
}


/*****************************************************************/


BOOST_AUTO_TEST_CASE(TestValueTypeEnumToString) {
    BOOST_CHECK_EQUAL( "INT"    , ParserValueTypeEnumToString(INT));
    BOOST_CHECK_EQUAL( "FLOAT" , ParserValueTypeEnumToString(FLOAT));
    BOOST_CHECK_EQUAL( "STRING"    , ParserValueTypeEnumToString(STRING));
}


BOOST_AUTO_TEST_CASE(TestValueTypeEnumFromString) {
    BOOST_CHECK_THROW( ParserValueTypeEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( INT , ParserValueTypeEnumFromString("INT"));
    BOOST_CHECK_EQUAL( STRING , ParserValueTypeEnumFromString("STRING"));
    BOOST_CHECK_EQUAL( FLOAT , ParserValueTypeEnumFromString("FLOAT"));
}



BOOST_AUTO_TEST_CASE(TestValueTypeEnumLoop) {
    BOOST_CHECK_EQUAL( INT    , ParserValueTypeEnumFromString( ParserValueTypeEnumToString( INT ) ));
    BOOST_CHECK_EQUAL( FLOAT , ParserValueTypeEnumFromString( ParserValueTypeEnumToString( FLOAT ) ));
    BOOST_CHECK_EQUAL( STRING    , ParserValueTypeEnumFromString( ParserValueTypeEnumToString( STRING ) ));

    BOOST_CHECK_EQUAL( "INT"    , ParserValueTypeEnumToString(ParserValueTypeEnumFromString(  "INT" ) ));
    BOOST_CHECK_EQUAL( "FLOAT" , ParserValueTypeEnumToString(ParserValueTypeEnumFromString(  "FLOAT" ) ));
    BOOST_CHECK_EQUAL( "STRING"    , ParserValueTypeEnumToString(ParserValueTypeEnumFromString(  "STRING" ) ));
}


/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestKeywordActionEnumToString) {
    BOOST_CHECK_EQUAL( "INTERNALIZE"     , ParserKeywordActionEnumToString(INTERNALIZE));
    BOOST_CHECK_EQUAL( "IGNORE"          , ParserKeywordActionEnumToString(IGNORE));
    BOOST_CHECK_EQUAL( "IGNORE_WARNING"  , ParserKeywordActionEnumToString(IGNORE_WARNING));
    BOOST_CHECK_EQUAL( "THROW_EXCEPTION" , ParserKeywordActionEnumToString(THROW_EXCEPTION));
}
 

BOOST_AUTO_TEST_CASE(TestKeywordActionEnumFromString) {
    BOOST_CHECK_THROW( ParserKeywordActionEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( INTERNALIZE    , ParserKeywordActionEnumFromString("INTERNALIZE"));
    BOOST_CHECK_EQUAL( IGNORE_WARNING , ParserKeywordActionEnumFromString("IGNORE_WARNING"));
    BOOST_CHECK_EQUAL( IGNORE  , ParserKeywordActionEnumFromString("IGNORE"));
    BOOST_CHECK_EQUAL( THROW_EXCEPTION  , ParserKeywordActionEnumFromString("THROW_EXCEPTION"));
}



BOOST_AUTO_TEST_CASE(TestKeywordActionEnumLoop) {
    BOOST_CHECK_EQUAL( INTERNALIZE    , ParserKeywordActionEnumFromString( ParserKeywordActionEnumToString( INTERNALIZE ) ));
    BOOST_CHECK_EQUAL( IGNORE , ParserKeywordActionEnumFromString( ParserKeywordActionEnumToString( IGNORE ) ));
    BOOST_CHECK_EQUAL( IGNORE_WARNING    , ParserKeywordActionEnumFromString( ParserKeywordActionEnumToString( IGNORE_WARNING ) ));
    BOOST_CHECK_EQUAL( THROW_EXCEPTION    , ParserKeywordActionEnumFromString( ParserKeywordActionEnumToString( THROW_EXCEPTION ) ));
    
    BOOST_CHECK_EQUAL( "INTERNALIZE"    , ParserKeywordActionEnumToString(ParserKeywordActionEnumFromString(  "INTERNALIZE" ) ));
    BOOST_CHECK_EQUAL( "IGNORE" , ParserKeywordActionEnumToString(ParserKeywordActionEnumFromString(  "IGNORE" ) ));
    BOOST_CHECK_EQUAL( "IGNORE_WARNING"    , ParserKeywordActionEnumToString(ParserKeywordActionEnumFromString(  "IGNORE_WARNING" ) ));
    BOOST_CHECK_EQUAL( "THROW_EXCEPTION" , ParserKeywordActionEnumToString(ParserKeywordActionEnumFromString(  "THROW_EXCEPTION" ) ));
}


