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
#ifndef OPM_PARSER_PLMIXPAR_TABLE_HPP
#define	OPM_PARSER_PLMIXPAR_TABLE_HPP

#include "SingleRecordTable.hpp"

namespace Opm {
    // forward declaration
    class EclipseState;

    class PlmixparTable : protected SingleRecordTable {
        typedef SingleRecordTable ParentType;

        friend class EclipseState;
        PlmixparTable() = default;

        /*!
         * \brief Read the TLMIXPAR keyword and provide some convenience
         *        methods for it.
         */
        void init(Opm::DeckKeywordConstPtr keyword, int recordIdx)
        {
            ParentType::init(keyword,
                             std::vector<std::string>{"VISC_PARA"},
                             recordIdx,
                             /*firstEntityOffset=*/0);

            // make sure the first column is not defaulted and copy the value from the
            // first column to the second one if the second column is defaulted
            int nRows = numRows();
            auto& viscColumnDefaulted = m_valueDefaulted[0];
            for (int rowIdx = 0; rowIdx < nRows; ++ rowIdx) {
                if (viscColumnDefaulted[rowIdx])
                    throw std::invalid_argument("The first column of the TLMIXPAR table cannot be defaulted");
            }
        }

    public:
        using ParentType::numTables;
        using ParentType::numRows;
        using ParentType::numColumns;

        // this table is not necessarily monotonic, so it cannot be evaluated!
        //using ParentType::evaluate;

        const std::vector<double> &getViscosityParameterColumn() const
        { return ParentType::getColumn(0); }

    };
}

#endif
