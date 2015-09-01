/*
  Copyright (C) 2012 IRIS AS

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
#ifndef OPM_PARSER_SOF2_TABLE_HPP
#define OPM_PARSER_SOF2_TABLE_HPP

#include "SingleRecordTable.hpp"

namespace Opm {
    // forward declaration
    class Tables;

    class Sof2Table : protected SingleRecordTable {
        typedef SingleRecordTable ParentType;

        friend class Tables;

        /*!
         * \brief Read the SOF2 keyword and provide some convenience
         *        methods for it.
         */
        void init(Opm::DeckKeywordConstPtr keyword,
                  int recordIdx)
        {
            ParentType::init(keyword,
                             std::vector<std::string>{"SO", "KRO" },
                             recordIdx,
                             /*firstEntityOffset=*/0);

            ParentType::checkNonDefaultable("SO");
            ParentType::checkNonDefaultable("KRO");
            ParentType::checkMonotonic("SO", /*isAscending=*/true);
            ParentType::checkMonotonic("KRO", /*isAscending=*/true, /*strict*/false);
        }

    public:
        Sof2Table() = default;

#ifdef BOOST_TEST_MODULE
        // DO NOT TRY TO CALL THIS METHOD! it is only for the unit tests!
        void initFORUNITTESTONLY(Opm::DeckKeywordConstPtr keyword, size_t tableIdx)
        { init(keyword, tableIdx); }
#endif

        using ParentType::numTables;
        using ParentType::numRows;
        using ParentType::numColumns;
        using ParentType::evaluate;

        const std::vector<double> &getSoColumn() const
        { return ParentType::getColumn(0); }

        const std::vector<double> &getKroColumn() const
        { return ParentType::getColumn(1); }
    };
}

#endif

