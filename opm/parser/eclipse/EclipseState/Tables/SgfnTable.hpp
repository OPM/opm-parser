/*
  Copyright (C) 2015 IRIS AS

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
#ifndef OPM_PARSER_SGFN_TABLE_HPP
#define OPM_PARSER_SGFN_TABLE_HPP

#include "SimpleTable.hpp"
#include <opm/parser/eclipse/EclipseState/Tables/TableEnums.hpp>

namespace Opm {
    // forward declaration
    class TableManager;

    class SgfnTable : public SimpleTable {

        friend class TableManager;

        /*!
         * \brief Read the SGFN keyword and provide some convenience
         *        methods for it.
         */
        SgfnTable(Opm::DeckItemConstPtr item)
        {
            m_schema = std::make_shared<TableSchema>();

            m_schema->addColumn( ColumnSchema("SG"  , Table::STRICTLY_INCREASING , Table::DEFAULT_NONE ) );
            m_schema->addColumn( ColumnSchema("KRG" , Table::INCREASING , Table::DEFAULT_LINEAR));
            m_schema->addColumn( ColumnSchema("PCOG" , Table::INCREASING , Table::DEFAULT_LINEAR));

            SimpleTable::init(item);
        }

    public:
        SgfnTable() = default;

#ifdef BOOST_TEST_MODULE
        // DO NOT TRY TO CALL THIS METHOD! it is only for the unit tests!
        void initFORUNITTESTONLY(Opm::DeckItemConstPtr item)
        { init(item); }
#endif

        using SimpleTable::numTables;
        using SimpleTable::numRows;
        using SimpleTable::numColumns;
        using SimpleTable::evaluate;

        const TableColumn& getSgColumn() const
        { return SimpleTable::getColumn(0); }

        const TableColumn& getKrgColumn() const
        { return SimpleTable::getColumn(1); }

        // this column is p_g - p_o (non-wetting phase pressure minus
        // wetting phase pressure for a given gas saturation)
        const TableColumn& getPcogColumn() const
        { return SimpleTable::getColumn(2); }
    };
}

#endif

