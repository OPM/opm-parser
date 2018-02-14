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
#include <sstream>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/MessageContainer.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/SummaryConfig/SummaryConfig.hpp>

inline void loadDeck( const char * deck_file) {
    Opm::ParseContext parseContext;
    Opm::Parser parser;

    auto deck = parser.parseFile(deck_file, parseContext);
    Opm::EclipseState state( deck, parseContext );
    Opm::Schedule schedule( deck, state.getInputGrid(), state.get3DProperties(), state.runspec().phases(), parseContext);
    Opm::SummaryConfig summary( deck, schedule, state.getTableManager( ), parseContext );
    {
        std::stringstream ss;

        ss << deck;
        auto deck2 = parser.parseString(ss.str(), parseContext);
        if (deck.size() != deck2.size()) {
            std::cerr << "Deck size mismatch original:" << deck.size() << " new: " << deck2.size( ) << std::endl;
            std::exit( 1 );
        }

        for (size_t index=0; index < deck.size(); index++) {
            const auto& kw1 = deck.getKeyword( index );
            const auto& kw2 = deck2.getKeyword( index );

            if (!kw1.equal( kw2 , true , true)) {
                std::cerr << "Keyword " << index << " different " << kw1.name() << " " << kw2.name() << std::endl;
                std::cerr << kw1 << std::endl;
                std::cerr << std::endl << "-----------------------------------------------------------------" << std::endl;
                std::cerr << kw2 << std::endl;
                std::exit( 1 );
            }
        }
    }
}


int main(int argc, char** argv) {
    for (int iarg = 1; iarg < argc; iarg++)
        loadDeck( argv[iarg] );
}

