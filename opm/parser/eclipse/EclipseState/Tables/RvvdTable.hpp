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
#ifndef OPM_PARSER_RVVD_TABLE_HPP
#define OPM_PARSER_RVVD_TABLE_HPP

#include "SimpleTable.hpp"

namespace Opm {
    class RvvdTable : public SimpleTable {
    public:
        RvvdTable(Opm::DeckItemConstPtr item)
        {
            m_schema = std::make_shared<TableSchema>( );
            m_schema->addColumn( ColumnSchema( "DEPTH" ,  Table::STRICTLY_INCREASING , Table::DEFAULT_NONE ) );
            m_schema->addColumn( ColumnSchema( "RV"    ,  Table::RANDOM , Table::DEFAULT_LINEAR ) );
            SimpleTable::init(item);
        }

        const TableColumn& getDepthColumn() const
        { return SimpleTable::getColumn(0); }

        const TableColumn& getRvColumn() const
        { return SimpleTable::getColumn(1); }
    };
}

#endif
