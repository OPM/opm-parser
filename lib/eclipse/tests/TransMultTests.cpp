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

#include <config.h>
#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>

#if HAVE_DYNAMIC_BOOST_TEST
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE EclipseGridTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opm/parser/eclipse/EclipseState/Eclipse3DProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/TransMult.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/TransMult.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperty.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridDims.hpp>

BOOST_AUTO_TEST_CASE(Empty) {
    Opm::Eclipse3DProperties props;
    Opm::TransMult transMult(Opm::GridDims(10,10,10) ,{} , props);

    BOOST_CHECK_THROW( transMult.getMultiplier(12,10,10 , Opm::FaceDir::XPlus) , std::invalid_argument );
    BOOST_CHECK_THROW( transMult.getMultiplier(1000 , Opm::FaceDir::XPlus) , std::invalid_argument );

    BOOST_CHECK_EQUAL( transMult.getMultiplier(9,9,9, Opm::FaceDir::YPlus) , 1.0 );
    BOOST_CHECK_EQUAL( transMult.getMultiplier(100 , Opm::FaceDir::ZPlus) , 1.0 );

    BOOST_CHECK_EQUAL( transMult.getMultiplier(9,9,9, Opm::FaceDir::YMinus) , 1.0 );
    BOOST_CHECK_EQUAL( transMult.getMultiplier(100 , Opm::FaceDir::ZMinus) , 1.0 );
}
