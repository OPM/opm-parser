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

#define BOOST_TEST_MODULE ColumnSchemaTests

#include <opm/common/utility/platform_dependent/disable_warnings.h>
#include <boost/test/unit_test.hpp>
#include <opm/common/utility/platform_dependent/reenable_warnings.h>


#include <opm/parser/eclipse/EclipseState/Tables/ColumnSchema.hpp>

using namespace Opm;


BOOST_AUTO_TEST_CASE( CreateTest ) {
    ColumnSchema schema("Name" , Table::INCREASING );
    BOOST_CHECK_EQUAL( schema.name() , "Name");
}


BOOST_AUTO_TEST_CASE( TestOrder) {
    {
        ColumnSchema schema("Name" , Table::INCREASING );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 1 , 0 ) );
    }

    {
        ColumnSchema schema("Name" , Table::DECREASING );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 1 , 0 ) );
    }

    {
        ColumnSchema schema("Name" , Table::STRICTLY_INCREASING );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 1 , 0 ) );
    }

    {
        ColumnSchema schema("Name" , Table::STRICTLY_DECREASING );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 1 , 0 ) );
    }
}

