/*
  Copyright 2014 Andreas Lauser

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

#ifndef OPM_CHECK_DECK_HPP
#define OPM_CHECK_DECK_HPP

#include <opm/parser/eclipse/Log/Logger.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>

namespace Opm {
enum DeckChecks {
    SectionTopology = 0x0001,

    // KeywordSection check only has an effect of the SectionTopology test is enabled
    KeywordSection = 0x0002,

    UnknownKeywords = 0x0004,

    TableSizes = 0x0008,

    AllChecks = 0xffff
};

// some semantical correctness checks of the deck. this method adds a warning to
// the deck object if any issue is found ...
bool checkDeck(DeckConstPtr deck, LoggerPtr logger, size_t enabledChecks = AllChecks);
}

#endif
