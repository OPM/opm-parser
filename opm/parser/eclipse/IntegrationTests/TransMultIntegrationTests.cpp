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

#define BOOST_TEST_MODULE TransMultTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>

using namespace Opm;


BOOST_AUTO_TEST_CASE(MULTFLT_IN_SCHEDULE) {
    Parser parser;
    std::string scheduleFile("testdata/integration_tests/TRANS/Deck1");
    ParseContext parseContext;
    auto deck = parser.parseFile(scheduleFile, parseContext);
    EclipseState state(deck, parseContext);
    const auto& trans = state.getTransMult();
    const auto& schedule = state.getSchedule();
    const Events& events = schedule.getEvents();

    BOOST_CHECK_EQUAL( 0.10 , trans.getMultiplier( 3,2,0,FaceDir::XPlus ));
    BOOST_CHECK_EQUAL( 0.10 , trans.getMultiplier( 2,2,0,FaceDir::XPlus ));
    BOOST_CHECK( events.hasEvent( ScheduleEvents::GEO_MODIFIER , 3 ) );
    {
        const auto& mini_deck = schedule.getModifierDeck(3);
        state.applyModifierDeck( mini_deck );
    }
    BOOST_CHECK_EQUAL( 2.00 , trans.getMultiplier( 2,2,0,FaceDir::XPlus ));
    BOOST_CHECK_EQUAL( 0.10 , trans.getMultiplier( 3,2,0,FaceDir::XPlus ));
}
