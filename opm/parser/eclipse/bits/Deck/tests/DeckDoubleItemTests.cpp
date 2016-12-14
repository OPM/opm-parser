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


#define BOOST_TEST_MODULE DeckItemTests

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <opm/parser/eclipse/bits/Deck/DeckItem.hpp>

#include <opm/parser/eclipse/Units/Dimension.hpp>

using namespace Opm;

BOOST_AUTO_TEST_CASE(InitializeDouble) {
    BOOST_REQUIRE_NO_THROW( DeckItem( "HEI", double() ) );
}

BOOST_AUTO_TEST_CASE(GetDoubleAtIndex_NoData_ExceptionThrown) {
    DeckItem deckDoubleItem( "TEST", double() );

    BOOST_CHECK_THROW(deckDoubleItem.get< double >(0), std::out_of_range);
    deckDoubleItem.push_back(1.89);
    BOOST_CHECK_THROW(deckDoubleItem.get< double >(1), std::out_of_range);
}


BOOST_AUTO_TEST_CASE(sizeDouble_correct) {
    DeckItem deckDoubleItem( "TEST", double() );

    BOOST_CHECK_EQUAL( 0U , deckDoubleItem.size());
    deckDoubleItem.push_back( 100.0 );
    BOOST_CHECK_EQUAL( 1U , deckDoubleItem.size());

    deckDoubleItem.push_back( 100.0 );
    deckDoubleItem.push_back( 100.0 );
    BOOST_CHECK_EQUAL( 3U , deckDoubleItem.size());
}



BOOST_AUTO_TEST_CASE(SetInDeck) {
    DeckItem deckDoubleItem( "TEST", double() );
    BOOST_CHECK( deckDoubleItem.size() == 0 );

    deckDoubleItem.push_backDefault( 1.0 );
    BOOST_CHECK( deckDoubleItem.size() == 1 );
    BOOST_CHECK_EQUAL( true , deckDoubleItem.defaultApplied(0) );

    deckDoubleItem.push_back( 10.0 );
    BOOST_CHECK( deckDoubleItem.size() == 2 );
    BOOST_CHECK_EQUAL( false , deckDoubleItem.defaultApplied(1) );

    deckDoubleItem.push_backDefault( 1.0 );
    BOOST_CHECK( deckDoubleItem.size() == 3 );
    BOOST_CHECK_EQUAL( true , deckDoubleItem.defaultApplied(2) );
}

BOOST_AUTO_TEST_CASE(DummyDefaults) {
    DeckItem deckDoubleItem( "TEST", double() );
    BOOST_CHECK_EQUAL(deckDoubleItem.size(), 0);

    deckDoubleItem.push_backDummyDefault();
    BOOST_CHECK_EQUAL(deckDoubleItem.size(), 0);
    BOOST_CHECK_EQUAL(true, deckDoubleItem.defaultApplied(0));
    BOOST_CHECK_THROW(deckDoubleItem.get< double >(0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(PushBackMultiple) {
    DeckItem item( "HEI", double() );
    item.push_back(10.22 , 100 );
    BOOST_CHECK_EQUAL( 100U , item.size() );
    for (size_t i=0; i < 100; i++)
        BOOST_CHECK_EQUAL(10.22 , item.get< double >(i));
}

BOOST_AUTO_TEST_CASE(PushBackDimension) {
    DeckItem item( "HEI", double() );
    Dimension activeDimension{ "Length" , 100 };
    Dimension defaultDimension{ "Length" , 10 };

    item.push_back(1.234);
    item.push_backDimension( activeDimension , defaultDimension);

    item.push_backDefault(5.678);
    item.push_backDimension( activeDimension , defaultDimension);
}

BOOST_AUTO_TEST_CASE(PushBackDimensionInvalidType) {
    DeckItem item( "HEI", int() );
    Dimension dim{ "Length" , 100 };
    BOOST_CHECK_THROW( item.push_backDimension( dim , dim ) , std::logic_error );
}

BOOST_AUTO_TEST_CASE(GetSIWithoutDimensionThrows) {
    DeckItem item( "HEI", double() );
    item.push_back(10.22 , 100 );

    BOOST_CHECK_THROW( item.getSIDouble(0) , std::invalid_argument );
    BOOST_CHECK_THROW( item.getSIDoubleData() , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(GetSISingleDimensionCorrect) {
    DeckItem item( "HEI", double() );
    Dimension dim{ "Length" , 100 };

    item.push_back(1.0 , 100 );
    item.push_backDimension( dim , dim );

    BOOST_CHECK_EQUAL( 1.0   , item.get< double >(0) );
    BOOST_CHECK_EQUAL( 100 , item.getSIDouble(0) );
}

BOOST_AUTO_TEST_CASE(GetSISingleDefault) {
    DeckItem item( "HEI", double() );
    Dimension dim{ "Length" , 1 };
    Dimension defaultDim{ "Length" , 100 };

    item.push_backDefault( 1.0 );
    item.push_backDimension( dim , defaultDim );

    BOOST_CHECK_EQUAL( 1   , item.get< double >(0) );
    BOOST_CHECK_EQUAL( 100 , item.getSIDouble(0) );
}

BOOST_AUTO_TEST_CASE(GetSIMultipleDim) {
    DeckItem item( "HEI", double() );
    Dimension dim1{ "Length" , 2 };
    Dimension dim2{ "Length" , 4 };
    Dimension dim3{ "Length" , 8 };
    Dimension dim4{ "Length" ,16 };
    Dimension defaultDim{ "Length" , 100 };

    item.push_back( 1.0, 16 );
    item.push_backDimension( dim1 , defaultDim );
    item.push_backDimension( dim2 , defaultDim );
    item.push_backDimension( dim3 , defaultDim );
    item.push_backDimension( dim4 , defaultDim );

    for (size_t i=0; i < 16; i+= 4) {
        BOOST_CHECK_EQUAL( 2   , item.getSIDouble(i) );
        BOOST_CHECK_EQUAL( 4   , item.getSIDouble(i+ 1) );
        BOOST_CHECK_EQUAL( 8   , item.getSIDouble(i+2) );
        BOOST_CHECK_EQUAL(16   , item.getSIDouble(i+3) );
    }
}

