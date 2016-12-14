/*
  Copyright 2016 Statoil ASA.

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

#ifndef BITS_PARSERS_HPP
#define BITS_PARSERS_HPP

#include <string>

#include <opm/parser/eclipse/bits/Deck/Deck.hpp>
#include <opm/parser/eclipse/Parser.hpp>

namespace Opm {
namespace ecl {

/*
 * A set of free functions that parses input files into internal
 * representations
 */

/* deck and deckString are implemented in eclipse/Parser.cpp */
Deck parseDeck( const Parser&, const std::string& file, const ParseContext& );
Deck parseDeckString( const Parser&, const std::string& file, const ParseContext& );

}
}

#endif //BITS_PARSERS_HPP

