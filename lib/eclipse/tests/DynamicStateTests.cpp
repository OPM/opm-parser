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

#include <config.h>
#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>


#if HAVE_DYNAMIC_BOOST_TEST
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE DynamicStateTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include <opm/parser/eclipse/EclipseState/Schedule/Well.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/TimeMap.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/DynamicState.hpp>



BOOST_AUTO_TEST_CASE(CreateDynamicTest) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    Opm::DynamicState<double> state(timeMap , 9.99);
}


BOOST_AUTO_TEST_CASE(DynamicStateGetOutOfRangeThrows) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    Opm::DynamicState<double> state(timeMap , 9.99);
    BOOST_CHECK_THROW( state.get(1) , std::out_of_range );
}


BOOST_AUTO_TEST_CASE(DynamicStateGetDefault) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    Opm::DynamicState<int> state(timeMap , 137);
    BOOST_CHECK_EQUAL( 137 , state.get(0));
    BOOST_CHECK_EQUAL( 137 , state.back() );
}


BOOST_AUTO_TEST_CASE(DynamicStateSetOutOfRangeThrows) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 2; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_THROW( state.update(3 , 100) , std::out_of_range );
}


BOOST_AUTO_TEST_CASE(DynamicStateSetOK) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };

    for (size_t i = 0; i < 10; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);
    Opm::DynamicState<int> state(timeMap , 137);

    state.update(2 , 23 );
    BOOST_CHECK_EQUAL( 137 , state.get(0));
    BOOST_CHECK_EQUAL( 137 , state.get(1));
    BOOST_CHECK_EQUAL( 23 , state.get(2));
    BOOST_CHECK_EQUAL( 23 , state.get(5));

    state.update(2 , 17);
    BOOST_CHECK_EQUAL( 137 , state.get(0));
    BOOST_CHECK_EQUAL( 137 , state.get(1));
    BOOST_CHECK_EQUAL( 17 , state.get(2));
    BOOST_CHECK_EQUAL( 17 , state.get(5));

    state.update(6 , 60);
    BOOST_CHECK_EQUAL( 17 , state.get(2));
    BOOST_CHECK_EQUAL( 17 , state.get(5));
    BOOST_CHECK_EQUAL( 60 , state.get(6));
    BOOST_CHECK_EQUAL( 60 , state.get(8));
    BOOST_CHECK_EQUAL( 60 , state.get(9));
    BOOST_CHECK_EQUAL( 60 , state.back());
}

BOOST_AUTO_TEST_CASE(DynamicStateAddAt) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 10; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 0);

    state.update( 10 , 77 );
    {
        const int& v1 = state.at(10);
        int v2 = state.get(10);
        BOOST_CHECK_EQUAL( v1 , 77 );
        BOOST_CHECK_EQUAL( v1 , v2 );
        BOOST_CHECK( &v1 != &v2 );
    }
}

BOOST_AUTO_TEST_CASE(DynamicStateOperatorSubscript) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 10; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);
    Opm::DynamicState<int> state(timeMap , 137);

    state.update( 10 , 200 );
    BOOST_CHECK_EQUAL( state[9] , 137 );
    BOOST_CHECK_EQUAL( state[0] , 137 );

}


BOOST_AUTO_TEST_CASE(DynamicStateInitial) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 10; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 137);
    Opm::DynamicState<int> state2(timeMap , 137);


    state.update( 10 , 200 );
    BOOST_CHECK_EQUAL( state[9] , 137 );
    BOOST_CHECK_EQUAL( state[0] , 137 );
    BOOST_CHECK_EQUAL( state[10] , 200 );

    state.updateInitial( 63 );
    BOOST_CHECK_EQUAL( state[9] , 63 );
    BOOST_CHECK_EQUAL( state[0] , 63 );
    BOOST_CHECK_EQUAL( state[10] , 200 );

    state.updateInitial( 73 );
    BOOST_CHECK_EQUAL( state[9] , 73 );
    BOOST_CHECK_EQUAL( state[0] , 73 );
    BOOST_CHECK_EQUAL( state[10] , 200 );


    state2.update( 10 , 200 );
    BOOST_CHECK_EQUAL( state2[9] , 137 );
    BOOST_CHECK_EQUAL( state2[0] , 137 );
    BOOST_CHECK_EQUAL( state2[10] , 200 );
    state.updateInitial( 73 );
    BOOST_CHECK_EQUAL( state2[9] , 137 );
    BOOST_CHECK_EQUAL( state2[0] , 137 );
    BOOST_CHECK_EQUAL( state2[10] , 200 );
}

BOOST_AUTO_TEST_CASE( ResetGlobal ) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 10; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 137);

    state.update(5 , 100);
    BOOST_CHECK_EQUAL( state[0] , 137 );
    BOOST_CHECK_EQUAL( state[4] , 137 );
    BOOST_CHECK_EQUAL( state[5] , 100 );
    BOOST_CHECK_EQUAL( state[9] , 100 );

    state.updateInitial( 22 );
    BOOST_CHECK_EQUAL( state[0] , 22 );
    BOOST_CHECK_EQUAL( state[4] , 22 );
    BOOST_CHECK_EQUAL( state[5] , 100 );
    BOOST_CHECK_EQUAL( state[9] , 100 );

    state.globalReset( 88 );
    BOOST_CHECK_EQUAL( state[0] , 88 );
    BOOST_CHECK_EQUAL( state[4] , 88 );
    BOOST_CHECK_EQUAL( state[5] , 88 );
    BOOST_CHECK_EQUAL( state[9] , 88 );
}


BOOST_AUTO_TEST_CASE( CheckReturn ) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 10; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_EQUAL( false , state.update( 0 , 137 ));
    BOOST_CHECK_EQUAL( false , state.update( 3 , 137 ));
    BOOST_CHECK_EQUAL( true , state.update( 5 , 200 ));
}


BOOST_AUTO_TEST_CASE( UpdateEmptyInitial ) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 10; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_EQUAL( state[5] , 137 );
    state.updateInitial( 99 );
    BOOST_CHECK_EQUAL( state[5] , 99 );
}


BOOST_AUTO_TEST_CASE( find ) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 5; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_EQUAL( state.find( 137 ) , 0 );
    BOOST_CHECK_EQUAL( state.find( 200 ) , -1 );
    state.update( 0 , 200 );
    BOOST_CHECK_EQUAL( state.find( 137 ) , -1 );
    BOOST_CHECK_EQUAL( state.find( 200 ) ,  0 );

    state.update( 2 , 300 );
    BOOST_CHECK_EQUAL( state.find( 200 ) ,  0 );
    BOOST_CHECK_EQUAL( state.find( 300 ) ,  2 );

    state.update( 4 , 400 );
    BOOST_CHECK_EQUAL( state.find( 200 ) ,  0 );
    BOOST_CHECK_EQUAL( state.find( 300 ) ,  2 );
    BOOST_CHECK_EQUAL( state.find( 400 ) ,  4 );
    BOOST_CHECK_EQUAL( state.find( 500 ) ,  -1 );
}


BOOST_AUTO_TEST_CASE( update_elm ) {
    const std::time_t startDate = Opm::TimeMap::mkdate(2010, 1, 1);
    Opm::TimeMap timeMap{ startDate };
    for (size_t i = 0; i < 5; i++)
        timeMap.addTStep((i+1) * 24 * 60 * 60);

    Opm::DynamicState<int> state(timeMap , 137);
    state.update( 5, 88 );
    BOOST_CHECK_THROW( state.update_elm(10,88) , std::out_of_range );
    BOOST_CHECK_EQUAL( state[2],137 );
    BOOST_CHECK_EQUAL( state[3],137 );
    BOOST_CHECK_EQUAL( state[4],137 );

    state.update_elm(3,88);
    BOOST_CHECK_EQUAL( state[2],137 );
    BOOST_CHECK_EQUAL( state[3],88 );
    BOOST_CHECK_EQUAL( state[4],137 );

    for (auto& v : state)
        v += 2;

    BOOST_CHECK_EQUAL( state[2],139 );
    BOOST_CHECK_EQUAL( state[3],90  );
    BOOST_CHECK_EQUAL( state[4],139 );
}
