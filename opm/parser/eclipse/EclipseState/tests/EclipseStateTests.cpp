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

#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE EclipseStateTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Deck/DeckIntItem.hpp>
#include <opm/parser/eclipse/Deck/DeckStringItem.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>

using namespace Opm;

DeckPtr createDeck() {
    const char *deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "1000*0.25 /\n"
        "EDIT\n"
        "OIL\n"
        "\n"
        "GAS\n"
        "\n"
        "TITLE\n"
        "The title\n"
        "\n"
        "START\n"
        "8 MAR 1998 /\n"
        "\n"
        "SCHEDULE\n"
        "\n";

    ParserPtr parser(new Parser());
    return parser->parseString(deckData) ;
}



BOOST_AUTO_TEST_CASE(CreatSchedule) {
    DeckPtr deck = createDeck();
    EclipseState state(deck);
    ScheduleConstPtr schedule = state.getSchedule();
    EclipseGridConstPtr eclipseGrid = state.getEclipseGrid();

    BOOST_CHECK_EQUAL( schedule->getStartTime() , boost::posix_time::ptime(boost::gregorian::date(1998 , 3 , 8 )));
}



BOOST_AUTO_TEST_CASE(PhasesCorrect) {
    DeckPtr deck = createDeck();
    EclipseState state(deck);

    BOOST_CHECK(  state.hasPhase( Phase::PhaseEnum::OIL ));
    BOOST_CHECK(  state.hasPhase( Phase::PhaseEnum::GAS ));
    BOOST_CHECK( !state.hasPhase( Phase::PhaseEnum::WATER ));
}


BOOST_AUTO_TEST_CASE(TitleCorrect) {
    DeckPtr deck = createDeck();
    EclipseState state(deck);

    BOOST_CHECK_EQUAL( state.getTitle(), "The title");
}
