/*
  Copyright 2015 Statoil ASA.

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
#ifndef OPM_PARSER_PVTX_TABLE_HPP
#define	OPM_PARSER_PVTX_TABLE_HPP

#include <vector>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

/*
  This class is a common base class for the PVTG and PVTO tables.
*/

namespace Opm {
    class PvtxTable
    {
    public:
        static size_t numTables(Opm::DeckKeywordConstPtr keyword);
        static std::vector<std::pair<size_t , size_t> > recordRanges(Opm::DeckKeywordConstPtr keyword);
    };

}

#endif
