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
#ifndef OPM_PARSER_IMKRVD_TABLE_HPP
#define	OPM_PARSER_IMKRVD_TABLE_HPP

#include "SingleRecordTable.hpp"

namespace Opm {
    // forward declaration
    class EclipseState;

    class ImkrvdTable : protected SingleRecordTable {
        typedef SingleRecordTable ParentType;

        friend class EclipseState;
        ImkrvdTable() = default;

        /*!
         * \brief Read the IMKRVD keyword and provide some convenience
         *        methods for it.
         */
        void init(Opm::DeckKeywordConstPtr keyword,
                  int recordIdx,
                  Opm::ParserLogPtr parserLog)
        {
            ParentType::init(keyword,
                             std::vector<std::string>{"DEPTH",
                                     "KRWMAX",
                                     "KRGMAX",
                                     "KROMAX",
                                     "KRWCRIT",
                                     "KRGCRIT",
                                     "KROCRITG",
                                     "KROCRITW" },
                             recordIdx,
                             /*firstEntityOffset=*/0,
                             parserLog);

            ParentType::checkNonDefaultable("DEPTH", parserLog);
            ParentType::checkMonotonic("DEPTH", /*isAscending=*/true, parserLog);
            ParentType::applyDefaultsLinear("KRWMAX", parserLog);
            ParentType::applyDefaultsLinear("KRGMAX", parserLog);
            ParentType::applyDefaultsLinear("KROMAX", parserLog);
            ParentType::applyDefaultsLinear("KRWCRIT", parserLog);
            ParentType::applyDefaultsLinear("KRGCRIT", parserLog);
            ParentType::applyDefaultsLinear("KROCRITG", parserLog);
            ParentType::applyDefaultsLinear("KROCRITW", parserLog);
        }

    public:
        using ParentType::numTables;
        using ParentType::numRows;
        using ParentType::numColumns;
        using ParentType::evaluate;

        /*!
         * \brief The datum depth for the remaining columns
         */
        const std::vector<double> &getDepthColumn() const
        { return ParentType::getColumn(0); }

        /*!
         * \brief Maximum relative permeability of water
         */
        const std::vector<double> &getKrwmaxColumn() const
        { return ParentType::getColumn(1); }

        /*!
         * \brief Maximum relative permeability of gas
         */
        const std::vector<double> &getKrgmaxColumn() const
        { return ParentType::getColumn(2); }

        /*!
         * \brief Maximum relative permeability of oil
         */
        const std::vector<double> &getKromaxColumn() const
        { return ParentType::getColumn(3); }

        /*!
         * \brief Relative permeability of water at the critical oil (or gas) saturation
         */
        const std::vector<double> &getKrwcritColumn() const
        { return ParentType::getColumn(4); }

        /*!
         * \brief Relative permeability of gas at the critical oil (or water) saturation
         */
        const std::vector<double> &getKrgcritColumn() const
        { return ParentType::getColumn(5); }

        /*!
         * \brief Oil relative permeability of oil at the critical gas saturation
         */
        const std::vector<double> &getKrocritgColumn() const
        { return ParentType::getColumn(6); }

        /*!
         * \brief Oil relative permeability of oil at the critical water saturation
         */
        const std::vector<double> &getKrocritwColumn() const
        { return ParentType::getColumn(7); }
    };
}

#endif

