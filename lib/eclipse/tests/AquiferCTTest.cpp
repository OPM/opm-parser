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
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE AquiferCTTest

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opm/parser/eclipse/EclipseState/Schedule/TimeMap.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/ThresholdPressure.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/SimulationConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Eclipse3DProperties.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/checkDeck.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/Box.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperty.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/Fault.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/FaultCollection.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/TransMult.hpp>
#include <opm/parser/eclipse/EclipseState/IOConfig/IOConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableManager.hpp>
#include <opm/parser/eclipse/Units/Units.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>

#include <opm/parser/eclipse/EclipseState/AquiferCT.hpp>

using namespace Opm;

inline std::vector<AquiferCT::AQUCT_data> test_init(){
    Parser parser;
    std::string filename = "./SPE1CASE1.data";
    EclipseState eclState = parser.parse(filename,ParseContext());
    Deck deck = parser.parseFile(filename,ParseContext());
    AquiferCT aquct = AquiferCT(eclState, deck);
    std::cout << "Porosity = " << aquct.getAqPorosity(0) << std::endl;
    std::vector<AquiferCT::AQUCT_data> aquifers = aquct.getAquifers();
    return aquifers;
}

BOOST_AUTO_TEST_CASE(AquiferCTTest) {
    test_init();
    BOOST_CHECK_EQUAL( 1 , 1);
}

