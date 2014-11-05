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
#ifndef OPM_PARSER_PVTG_OUTER_TABLE_HPP
#define	OPM_PARSER_PVTG_OUTER_TABLE_HPP

#include "MultiRecordTable.hpp"

namespace Opm {
    // forward declarations
    template <class OuterTable, class InnerTable>
    class FullTable;
    class PvtgTable;
    class PvtgOuterTable;
    class PvtgInnerTable;

    class PvtgOuterTable : protected MultiRecordTable {
        typedef MultiRecordTable ParentType;

        friend class PvtgTable;
        friend class FullTable<PvtgOuterTable, PvtgInnerTable>;
        PvtgOuterTable() = default;

        /*!
         * \brief Read the per record table of the PVTG keyword and
         *        provide some convenience methods for it.
         */
        void init(Opm::DeckKeywordConstPtr keyword, size_t tableIdx, Opm::ParserLogPtr parserLog)
        {
            ParentType::init(keyword,
                             std::vector<std::string>{"P", "RV", "BG", "MUG"},
                             tableIdx,
                             /*firstEntryOffset=*/0,
                             parserLog);

            ParentType::checkNonDefaultable("P", parserLog);
            ParentType::checkMonotonic("P", /*isAscending=*/true, parserLog);
            ParentType::applyDefaultsLinear("RV", parserLog);
            ParentType::applyDefaultsLinear("BG", parserLog);
            ParentType::applyDefaultsLinear("MUG", parserLog);
        }

    public:
        using ParentType::numTables;
        using ParentType::numRows;
        using ParentType::numColumns;
        using ParentType::evaluate;
        using ParentType::firstRecordIndex;
        using ParentType::numRecords;

        const std::vector<double> &getPressureColumn() const
        { return ParentType::getColumn(0); }

        const std::vector<double> &getOilSolubilityColumn() const
        { return ParentType::getColumn(1); }

        const std::vector<double> &getGasFormationFactorColumn() const
        { return ParentType::getColumn(2); }

        const std::vector<double> &getGasViscosityColumn() const
        { return ParentType::getColumn(3); }
    };
}

#endif
