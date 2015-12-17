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

#include "SimpleTable.hpp"
#include <opm/parser/eclipse/EclipseState/Tables/TableEnums.hpp>

namespace Opm {
    // forward declaration
    class TableManager;

    class PlymaxTable : public SimpleTable {
    public:
        friend class TableManager;

        // This is not really a table; every column has only one element.
        PlymaxTable(Opm::DeckRecordConstPtr record)
        {
            m_schema = std::make_shared<TableSchema>( );

            m_schema->addColumn( ColumnSchema("C_POLYMER",     Table::RANDOM , Table::DEFAULT_NONE) );
            m_schema->addColumn( ColumnSchema("C_POLYMER_MAX", Table::RANDOM , Table::DEFAULT_NONE) );

            addColumns();
            for (size_t colIdx = 0; colIdx < record->size(); colIdx++) {
                auto item = record->getItem( colIdx );
                auto& column = getColumn( colIdx );

                column.addValue( item->getSIDouble(0) );
            }
        }

        using SimpleTable::numTables;
        using SimpleTable::numRows;
        using SimpleTable::numColumns;
        using SimpleTable::evaluate;

        const TableColumn& getPolymerConcentrationColumn() const
        { return SimpleTable::getColumn(0); }

        const TableColumn& getMaxPolymerConcentrationColumn() const
        { return SimpleTable::getColumn(1); }
    };
}

#endif
