/*
  Copyright 2014 Statoil ASA.

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

#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE EclipseGridTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include <opm/parser/eclipse/EclipseState/Grid/TransMult.hpp>

BOOST_AUTO_TEST_CASE(Empty) {
    Opm::TransMult transMult(10,10,10);

    
    BOOST_CHECK_THROW( transMult.getXFaceMultiplier(12,10,10) , std::invalid_argument );
    BOOST_CHECK_THROW( transMult.getYFaceMultiplier(12,10,10) , std::invalid_argument );
    BOOST_CHECK_THROW( transMult.getZFaceMultiplier(12,10,10) , std::invalid_argument );

    BOOST_CHECK_EQUAL( transMult.getXFaceMultiplier(10,10,10) , 1.0 );
    BOOST_CHECK_EQUAL( transMult.getYFaceMultiplier(10,10,10) , 1.0 );
    BOOST_CHECK_EQUAL( transMult.getZFaceMultiplier(10,10,10) , 1.0 );

    BOOST_CHECK_EQUAL( std::get<0>(transMult.getCellMultipliers(0,0,0)) , 1.0);
    BOOST_CHECK_EQUAL( std::get<1>(transMult.getCellMultipliers(0,0,0)) , 1.0);
    BOOST_CHECK_EQUAL( std::get<2>(transMult.getCellMultipliers(0,0,0)) , 1.0);
    
}
