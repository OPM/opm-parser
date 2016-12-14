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

#include <iostream>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/MessageContainer.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/bits/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>


inline void dumpMessages( const Opm::MessageContainer& messageContainer) {
    auto extractMessage = [](const Opm::Message& msg) {
        const auto& location = msg.location;
        if (location)
            return location.filename + ":" + std::to_string( location.lineno ) + " " + msg.message;
        else
            return msg.message;
    };


    for(const auto& msg : messageContainer)
        std::cout << extractMessage(msg) << std::endl;
}


inline void loadDeck( const char * deck_file) {
    Opm::ParseContext parseContext;
    Opm::Parser parser;

    std::cout << "Loading deck: " << deck_file << " ..... "; std::cout.flush();
    auto deck = parser.parseFile(deck_file, parseContext);
    std::cout << "parse complete - creating EclipseState .... ";  std::cout.flush();
    Opm::EclipseState state( deck, parseContext );
    std::cout << "complete." << std::endl;

    dumpMessages( deck.getMessageContainer() );
}


int main(int argc, char** argv) {
    for (int iarg = 1; iarg < argc; iarg++)
        loadDeck( argv[iarg] );
}

