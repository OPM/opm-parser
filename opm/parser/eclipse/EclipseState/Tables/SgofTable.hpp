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
#ifndef OPM_PARSER_SGOF_TABLE_HPP
#define	OPM_PARSER_SGOF_TABLE_HPP

#include "SingleRecordTable.hpp"

namespace Opm {
    // forward declaration
    class EclipseState;

    class SgofTable : protected SingleRecordTable {
        typedef SingleRecordTable ParentType;

        friend class EclipseState;

        /*!
         * \brief Read the SGOF keyword and provide some convenience
         *        methods for it.
         */
        void init(Opm::DeckKeywordConstPtr keyword,
                  int recordIdx,
                  Opm::ParserLogPtr parserLog)
        {
            ParentType::init(keyword,
                             std::vector<std::string>{"SG", "KRG", "KROG", "PCOG"},
                             recordIdx,
                             /*firstEntityOffset=*/0,
                             parserLog);

            ParentType::checkNonDefaultable("SG", parserLog);
            ParentType::checkMonotonic("SG", /*isAscending=*/true, parserLog);
            ParentType::applyDefaultsLinear("KRG", parserLog);
            ParentType::applyDefaultsLinear("KROG", parserLog);
            ParentType::applyDefaultsLinear("PCOG", parserLog);
        }

    public:
        SgofTable() = default;

#ifdef BOOST_TEST_MODULE
        // DO NOT TRY TO CALL THIS METHOD! it is only for the unit tests!
        void initFORUNITTESTONLY(Opm::DeckKeywordConstPtr keyword, size_t tableIdx, Opm::ParserLogPtr parserLog)
        { init(keyword, tableIdx, parserLog); }
#endif

        using ParentType::numTables;
        using ParentType::numRows;
        using ParentType::numColumns;
        using ParentType::evaluate;

        const std::vector<double> &getSgColumn() const
        { return ParentType::getColumn(0); }

        const std::vector<double> &getKrgColumn() const
        { return ParentType::getColumn(1); }

        const std::vector<double> &getKrogColumn() const
        { return ParentType::getColumn(2); }

        // this column is p_g - p_o (non-wetting phase pressure minus
        // wetting phase pressure for a given gas saturation. the name
        // is inconsistent, but it is the one used in the Eclipse
        // manual...)
        const std::vector<double> &getPcogColumn() const
        { return ParentType::getColumn(3); }
    };
}

#endif
