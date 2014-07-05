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

#define BOOST_TEST_MODULE FaultTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opm/parser/eclipse/EclipseState/Grid/FaultCollection.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/Fault.hpp>


BOOST_AUTO_TEST_CASE(CreateFault) {
    Opm::Fault fault("FAULT1");
    BOOST_CHECK_EQUAL( "FAULT1" , fault.getName());
    BOOST_CHECK_EQUAL( 1.0 , fault.getTransMult());
}


BOOST_AUTO_TEST_CASE(CreateFaultCollection) {
    Opm::FaultCollection faults(100,10,6);
    BOOST_CHECK_EQUAL( faults.size() , 0 );
    BOOST_CHECK(! faults.hasFault("NO-NotThisOne"));
    BOOST_CHECK_THROW( faults.getFault("NO") , std::invalid_argument );
}


BOOST_AUTO_TEST_CASE(AddFaultsToCollection) {
    Opm::FaultCollection faults(10,10,10);
    std::shared_ptr<Opm::Fault> fault = std::make_shared<Opm::Fault>("FAULT");
    
    faults.addFault(fault);
    BOOST_CHECK_EQUAL( faults.size() , 1 );
    BOOST_CHECK(faults.hasFault("FAULT"));

    std::shared_ptr<Opm::Fault> fault2 = faults.getFault("FAULT");
    BOOST_CHECK_EQUAL( fault , fault2 );
}


