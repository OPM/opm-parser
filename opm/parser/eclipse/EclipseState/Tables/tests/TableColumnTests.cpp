/*
  Copyright (C) 2015 by Statoil ASA

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

#define BOOST_TEST_MODULE TableColumnTests

#include <opm/common/utility/platform_dependent/disable_warnings.h>
#include <boost/test/unit_test.hpp>
#include <opm/common/utility/platform_dependent/reenable_warnings.h>


#include <opm/parser/eclipse/EclipseState/Tables/TableColumn.hpp>

using namespace Opm;


BOOST_AUTO_TEST_CASE( CreateTest ) {
    TableColumn column("COLUMN" , true);
    BOOST_CHECK_EQUAL( column.size() , 0 );
    BOOST_CHECK_EQUAL( column.name() , "COLUMN");

    column.addValue( 0 );
    column.addValue( 1 );
    column.addValue( 2 );

    BOOST_CHECK_EQUAL( column.size() , 3 );

    BOOST_CHECK_EQUAL( column[0] , 0 );
    BOOST_CHECK_EQUAL( column[1] , 1 );
    BOOST_CHECK_EQUAL( column[2] , 2 );

    BOOST_CHECK_THROW( column[3] , std::invalid_argument );
}



BOOST_AUTO_TEST_CASE( TestDefault ) {
    TableColumn column("COLUMN" , true);

    column.addDefault( );
    column.addDefault( );
    column.addDefault( );
    BOOST_CHECK_EQUAL( column.size() , 3 );
    BOOST_CHECK_THROW( column[0] , std::invalid_argument );

    column.updateValue(0 , 10);
    BOOST_CHECK_EQUAL( column[0] , 10 );
}


BOOST_AUTO_TEST_CASE( TestAscending ) {
    TableColumn column("COLUMN" , true);
    BOOST_CHECK_EQUAL( column.size() , 0 );
    BOOST_CHECK_EQUAL( column.name() , "COLUMN");

    column.addValue( 10 );
    BOOST_CHECK_THROW( column.addValue( 9 ) , std::invalid_argument );
    column.addDefault( );
    BOOST_CHECK_THROW( column.updateValue( 1, 9 ) , std::invalid_argument );
    column.addValue( 12 );
    BOOST_CHECK_THROW( column.updateValue( 1, 13 ) , std::invalid_argument );
    column.updateValue( 1, 11 );

    column.addDefault( );
    column.addDefault( );
    column.addDefault( );
    column.addValue(16);

    column.updateValue( 3,13 );
    column.updateValue( 4,14 );
    column.updateValue( 5,15 );
}



BOOST_AUTO_TEST_CASE( TestDescending ) {
    TableColumn column("COLUMN" , false);
    BOOST_CHECK_EQUAL( column.size() , 0 );
    BOOST_CHECK_EQUAL( column.name() , "COLUMN");

    column.addValue( -10 );
    BOOST_CHECK_THROW( column.addValue( -9 ) , std::invalid_argument );
    column.addDefault( );
    BOOST_CHECK_THROW( column.updateValue( 1, -9 ) , std::invalid_argument );
    column.addValue( -12 );
    BOOST_CHECK_THROW( column.updateValue( 1, -13 ) , std::invalid_argument );
    column.updateValue( 1, -11 );

    column.addDefault( );
    column.addDefault( );
    column.addDefault( );
    column.addValue(-16);

    column.updateValue( 3,-13 );
    column.updateValue( 4,-14 );
    column.updateValue( 5,-15 );
}

