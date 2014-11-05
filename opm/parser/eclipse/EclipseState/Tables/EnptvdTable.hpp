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
#ifndef OPM_PARSER_ENPTVD_TABLE_HPP
#define	OPM_PARSER_ENPTVD_TABLE_HPP

#include "SingleRecordTable.hpp"

namespace Opm {
    // forward declaration
    class EclipseState;

    class EnptvdTable : protected SingleRecordTable {
        typedef SingleRecordTable ParentType;

        friend class EclipseState;
        EnptvdTable() = default;

        /*!
         * \brief Read the ENPTVD keyword and provide some convenience
         *        methods for it.
         */
        void init(Opm::DeckKeywordConstPtr keyword,
                  int recordIdx,
                  Opm::ParserLogPtr parserLog)
        {
            ParentType::init(keyword,
                             std::vector<std::string>{"DEPTH",
                                     "SWCO",
                                     "SWCRIT",
                                     "SWMAX",
                                     "SGCO",
                                     "SGCRIT",
                                     "SGMAX",
                                     "SOWCRIT",
                                     "SOGCRIT"},
                             recordIdx,
                             /*firstEntityOffset=*/0,
                             parserLog);

            ParentType::checkNonDefaultable("DEPTH", parserLog);
            ParentType::checkMonotonic("DEPTH", /*isAscending=*/true, parserLog);
            ParentType::applyDefaultsLinear("SWCO", parserLog);
            ParentType::applyDefaultsLinear("SWCRIT", parserLog);
            ParentType::applyDefaultsLinear("SGCO", parserLog);
            ParentType::applyDefaultsLinear("SGCRIT", parserLog);
            ParentType::applyDefaultsLinear("SGMAX", parserLog);
            ParentType::applyDefaultsLinear("SOWCRIT", parserLog);
            ParentType::applyDefaultsLinear("SOGCRIT", parserLog);
        }

    public:
        using ParentType::numTables;
        using ParentType::numRows;
        using ParentType::numColumns;
        using ParentType::evaluate;

        // using this method is strongly discouraged but the current endpoint scaling
        // code makes it hard to avoid
        using ParentType::getColumn;

        const std::vector<double> &getDepthColumn() const
        { return ParentType::getColumn(0); }

        /*!
         * \brief Connate water saturation
         */
        const std::vector<double> &getSwcoColumn() const
        { return ParentType::getColumn(1); }

        /*!
         * \brief Critical water saturation
         */
        const std::vector<double> &getSwcritColumn() const
        { return ParentType::getColumn(2); }

        /*!
         * \brief Maximum water saturation
         */
        const std::vector<double> &getSwmaxColumn() const
        { return ParentType::getColumn(3); }

        /*!
         * \brief Connate gas saturation
         */
        const std::vector<double> &getSgcoColumn() const
        { return ParentType::getColumn(4); }

        /*!
         * \brief Critical gas saturation
         */
        const std::vector<double> &getSgcritColumn() const
        { return ParentType::getColumn(5); }

        /*!
         * \brief Maximum gas saturation
         */
        const std::vector<double> &getSgmaxColumn() const
        { return ParentType::getColumn(6); }

        /*!
         * \brief Critical oil-in-water saturation
         */
        const std::vector<double> &getSowcritColumn() const
        { return ParentType::getColumn(7); }

        /*!
         * \brief Critical oil-in-gas saturation
         */
        const std::vector<double> &getSogcritColumn() const
        { return ParentType::getColumn(8); }
    };
}

#endif

