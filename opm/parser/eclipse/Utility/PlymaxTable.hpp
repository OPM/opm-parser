/*
  Copyright (C) 2014 by Andreas Lauser

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
#ifndef OPM_PARSER_PLYMAX_TABLE_HPP
#define	OPM_PARSER_PLYMAX_TABLE_HPP

#include "SingleRecordTable.hpp"

namespace Opm {
    class PlymaxTable : protected SingleRecordTable {
        typedef SingleRecordTable ParentType;

    public:
        using ParentType::numTables;

        /*!
         * \brief Read the PLYMAX keyword and provide some convenience
         *        methods for it.
         */
        PlymaxTable(Opm::DeckKeywordConstPtr keyword, int recordIdx = 0)
            : ParentType(keyword,
                         std::vector<std::string>{"C_POLYMER", "C_POLYMER_MAX"},
                         recordIdx,
                         /*firstEntityOffset=*/0)
        {}

        int numRows() const
        { return ParentType::numRows(); };

        int numColumns() const
        { return ParentType::numColumns(); };

        const std::vector<double> &getPolymerConcentrationColumn() const
        { return ParentType::getColumn(0); }

        const std::vector<double> &getMaxPolymerConcentrationColumn() const
        { return ParentType::getColumn(1); }
    };
}

#endif	// OPM_PARSER_SIMPLE_TABLE_HPP

