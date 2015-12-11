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

#include <opm/parser/eclipse/EclipseState/Tables/PvtxTable.hpp>

namespace Opm {

    size_t PvtxTable::numTables(Opm::DeckKeywordConstPtr keyword)
    {
        auto ranges = recordRanges(keyword);
        return ranges.size();
    }


    std::vector<std::pair<size_t , size_t> > PvtxTable::recordRanges(Opm::DeckKeywordConstPtr keyword) {
        std::vector<std::pair<size_t,size_t> > ranges;
        size_t startRecord = 0;
        size_t recordIndex = 0;
        while (recordIndex < keyword->size()) {
            auto item = keyword->getRecord(recordIndex)->getItem(0);
            if (item->size( ) == 0) {
                ranges.push_back( std::make_pair( startRecord , recordIndex ) );
                startRecord = recordIndex + 1;
            }
            recordIndex++;
        }
        ranges.push_back( std::make_pair( startRecord , recordIndex ) );
        return ranges;
    }


}

