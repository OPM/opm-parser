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

BOOST_AUTO_TEST_CASE(TestItemSizeEnum2String) {
    BOOST_CHECK_EQUAL( "ALL"    , ParserItemSizeEnum2String(ALL));
    BOOST_CHECK_EQUAL( "SINGLE" , ParserItemSizeEnum2String(SINGLE));
    BOOST_CHECK_EQUAL( "BOX"    , ParserItemSizeEnum2String(BOX));
}


BOOST_AUTO_TEST_CASE(TestItemSizeEnumFromString) {
    BOOST_CHECK_THROW( ParserItemSizeEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( ALL , ParserItemSizeEnumFromString("ALL"));
    BOOST_CHECK_EQUAL( BOX , ParserItemSizeEnumFromString("BOX"));
    BOOST_CHECK_EQUAL( SINGLE , ParserItemSizeEnumFromString("SINGLE"));
}



BOOST_AUTO_TEST_CASE(TestItemSizeEnumLoop) {
    BOOST_CHECK_EQUAL( ALL    , ParserItemSizeEnumFromString( ParserItemSizeEnum2String( ALL ) ));
    BOOST_CHECK_EQUAL( SINGLE , ParserItemSizeEnumFromString( ParserItemSizeEnum2String( SINGLE ) ));
    BOOST_CHECK_EQUAL( BOX    , ParserItemSizeEnumFromString( ParserItemSizeEnum2String( BOX ) ));

    BOOST_CHECK_EQUAL( "ALL"    , ParserItemSizeEnum2String(ParserItemSizeEnumFromString(  "ALL" ) ));
    BOOST_CHECK_EQUAL( "SINGLE" , ParserItemSizeEnum2String(ParserItemSizeEnumFromString(  "SINGLE" ) ));
    BOOST_CHECK_EQUAL( "BOX"    , ParserItemSizeEnum2String(ParserItemSizeEnumFromString(  "BOX" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestKeywordSizeEnum2String) {
    BOOST_CHECK_EQUAL( "UNDEFINED" , ParserKeywordSizeEnum2String(UNDEFINED));
    BOOST_CHECK_EQUAL( "FIXED"     , ParserKeywordSizeEnum2String(FIXED));
}


BOOST_AUTO_TEST_CASE(TestKeywordSizeEnumFromString) {
    BOOST_CHECK_THROW( ParserKeywordSizeEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( FIXED     , ParserKeywordSizeEnumFromString("FIXED"));
    BOOST_CHECK_EQUAL( UNDEFINED , ParserKeywordSizeEnumFromString("UNDEFINED"));
}



BOOST_AUTO_TEST_CASE(TestKeywordSizeEnumLoop) {
    BOOST_CHECK_EQUAL( FIXED     , ParserKeywordSizeEnumFromString( ParserKeywordSizeEnum2String( FIXED ) ));
    BOOST_CHECK_EQUAL( UNDEFINED , ParserKeywordSizeEnumFromString( ParserKeywordSizeEnum2String( UNDEFINED ) ));
    
    BOOST_CHECK_EQUAL( "FIXED"     , ParserKeywordSizeEnum2String(ParserKeywordSizeEnumFromString(  "FIXED" ) ));
    BOOST_CHECK_EQUAL( "UNDEFINED" , ParserKeywordSizeEnum2String(ParserKeywordSizeEnumFromString(  "UNDEFINED" ) ));
}


/*****************************************************************/


BOOST_AUTO_TEST_CASE(TestValueTypeEnum2String) {
    BOOST_CHECK_EQUAL( "INT"    , ParserValueTypeEnum2String(INT));
    BOOST_CHECK_EQUAL( "FLOAT" , ParserValueTypeEnum2String(FLOAT));
    BOOST_CHECK_EQUAL( "STRING"    , ParserValueTypeEnum2String(STRING));
}


BOOST_AUTO_TEST_CASE(TestValueTypeEnumFromString) {
    BOOST_CHECK_THROW( ParserValueTypeEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( INT , ParserValueTypeEnumFromString("INT"));
    BOOST_CHECK_EQUAL( STRING , ParserValueTypeEnumFromString("STRING"));
    BOOST_CHECK_EQUAL( FLOAT , ParserValueTypeEnumFromString("FLOAT"));
}



BOOST_AUTO_TEST_CASE(TestValueTypeEnumLoop) {
    BOOST_CHECK_EQUAL( INT    , ParserValueTypeEnumFromString( ParserValueTypeEnum2String( INT ) ));
    BOOST_CHECK_EQUAL( FLOAT , ParserValueTypeEnumFromString( ParserValueTypeEnum2String( FLOAT ) ));
    BOOST_CHECK_EQUAL( STRING    , ParserValueTypeEnumFromString( ParserValueTypeEnum2String( STRING ) ));

    BOOST_CHECK_EQUAL( "INT"    , ParserValueTypeEnum2String(ParserValueTypeEnumFromString(  "INT" ) ));
    BOOST_CHECK_EQUAL( "FLOAT" , ParserValueTypeEnum2String(ParserValueTypeEnumFromString(  "FLOAT" ) ));
    BOOST_CHECK_EQUAL( "STRING"    , ParserValueTypeEnum2String(ParserValueTypeEnumFromString(  "STRING" ) ));
}
