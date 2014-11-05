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
#include <opm/parser/eclipse/EclipseState/Grid/FaultFace.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/FaceDir.hpp>



BOOST_AUTO_TEST_CASE(CreateInvalidFace) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    // I out of range
    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::FaultFace(parserLog, "", -1, 10,10,10,10 , 10 , 1 , 1 , 5 , 5 , Opm::FaceDir::XPlus));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    // I1 != I2 when face == X
    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::FaultFace(parserLog, "", -1, 10,10,10, 1 , 3  , 1 , 1 , 5 , 5 , Opm::FaceDir::XPlus));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    // J1 < J2
    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::FaultFace(parserLog, "", -1, 10,10,10,3 , 3  , 3 , 1 , 5 , 5 , Opm::FaceDir::XPlus));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);
}


BOOST_AUTO_TEST_CASE(CreateFace) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    std::shared_ptr<Opm::FaultFace> face1 = std::make_shared<Opm::FaultFace>(parserLog, "", -1, 10,10,10,0, 2  , 0 , 0 , 0 , 0 , Opm::FaceDir::YPlus);
    std::shared_ptr<Opm::FaultFace> face2 = std::make_shared<Opm::FaultFace>(parserLog, "", -1, 10,10,10,0, 2  , 1 , 1 , 0 , 0 , Opm::FaceDir::YPlus);
    std::shared_ptr<Opm::FaultFace> face3 = std::make_shared<Opm::FaultFace>(parserLog, "", -1, 10,10,10,0, 2  , 0 , 0 , 1 , 1 , Opm::FaceDir::YPlus);

    std::vector<size_t> trueValues1{0,1,2};
    std::vector<size_t> trueValues2{10,11,12};
    std::vector<size_t> trueValues3{100,101,102};
    size_t i = 0;

    {
        auto iter3 = face3->begin();
        auto iter2 = face2->begin();
        for (auto iter1 = face1->begin(); iter1 != face1->end(); ++iter1) {
            size_t index1 = *iter1;
            size_t index2 = *iter2;
            size_t index3 = *iter3;
        
            BOOST_CHECK_EQUAL( index1 , trueValues1[i] );
            BOOST_CHECK_EQUAL( index2 , trueValues2[i] );
            BOOST_CHECK_EQUAL( index3 , trueValues3[i] );

            ++iter2;
            ++iter3;
            ++i;
        }
    }
    BOOST_CHECK_EQUAL( face1->getDir() , Opm::FaceDir::YPlus);
}


BOOST_AUTO_TEST_CASE(CreateFault) {
    Opm::Fault fault("FAULT1");
    BOOST_CHECK_EQUAL( "FAULT1" , fault.getName());
    BOOST_CHECK_EQUAL( 1.0 , fault.getTransMult());
}



BOOST_AUTO_TEST_CASE(AddFaceToFaults) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    Opm::Fault fault("FAULT1");
    std::shared_ptr<Opm::FaultFace> face1 = std::make_shared<Opm::FaultFace>(parserLog, "", -1, 10,10,10,0, 2  , 0 , 0 , 0 , 0 , Opm::FaceDir::YPlus);
    std::shared_ptr<Opm::FaultFace> face2 = std::make_shared<Opm::FaultFace>(parserLog, "", -1, 10,10,10,0, 2  , 1 , 1 , 0 , 0 , Opm::FaceDir::YPlus);
    std::shared_ptr<Opm::FaultFace> face3 = std::make_shared<Opm::FaultFace>(parserLog, "", -1, 10,10,10,0, 2  , 0 , 0 , 1 , 1 , Opm::FaceDir::YPlus);
    fault.addFace( face1 );
    fault.addFace( face2 );
    fault.addFace( face3 );

    {
        auto iter = fault.begin();
        BOOST_CHECK_EQUAL( *iter , face1 ); ++iter;
        BOOST_CHECK_EQUAL( *iter , face2 ); ++iter;
        BOOST_CHECK_EQUAL( *iter , face3 ); ++iter;
    }    
    
}



BOOST_AUTO_TEST_CASE(CreateFaultCollection) {
    Opm::FaultCollection faults;
    BOOST_CHECK_EQUAL( faults.size() , 0 );
    BOOST_CHECK(! faults.hasFault("NO-NotThisOne"));
    BOOST_CHECK_THROW( faults.getFault("NO") , std::invalid_argument );
}


BOOST_AUTO_TEST_CASE(AddFaultsToCollection) {
    Opm::FaultCollection faults;
    std::shared_ptr<Opm::Fault> fault = std::make_shared<Opm::Fault>("FAULT");
    
    faults.addFault(fault);
    BOOST_CHECK_EQUAL( faults.size() , 1 );
    BOOST_CHECK(faults.hasFault("FAULT"));

    std::shared_ptr<Opm::Fault> fault2 = faults.getFault("FAULT");
    std::shared_ptr<Opm::Fault> fault0 = faults.getFault(0);
    BOOST_CHECK_EQUAL( fault , fault2 );
    BOOST_CHECK_EQUAL( fault , fault0 );

    std::shared_ptr<Opm::Fault> faultx = std::make_shared<Opm::Fault>("FAULTX");
    faults.addFault(faultx);
    BOOST_CHECK_EQUAL( faults.size() , 2 );
    BOOST_CHECK(faults.hasFault("FAULTX"));
    BOOST_CHECK_EQUAL( faultx , faults.getFault(1));
}
