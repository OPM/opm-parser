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
#ifndef OPM_PARSER_PVTO_INNER_TABLE_HPP
#define	OPM_PARSER_PVTO_INNER_TABLE_HPP

#include "SimpleTable.hpp"

namespace Opm {

    class PvtoInnerTable : public SimpleTable {
    public:

        /*!
         * \brief Read the per record table of the PVTO keyword and
         *        provide some convenience methods for it.
         *
         * The first value of the record (-> Rs) is skipped.
         */

        PvtoInnerTable(Opm::DeckItemConstPtr item)
        {
            m_schema = std::make_shared<TableSchema>( );

            m_schema->addColumn( ColumnSchema( "P"  , Table::STRICTLY_INCREASING , Table::DEFAULT_NONE ));
            m_schema->addColumn( ColumnSchema( "BO" , Table::RANDOM , Table::DEFAULT_LINEAR ));
            m_schema->addColumn( ColumnSchema( "MU" , Table::RANDOM , Table::DEFAULT_LINEAR ));

            SimpleTable::init( item );
        }

        const TableColumn& getPressureColumn() const
        { return SimpleTable::getColumn(0); }

        const TableColumn& getOilFormationFactorColumn() const
        { return SimpleTable::getColumn(1); }

        const TableColumn& getOilViscosityColumn() const
        { return SimpleTable::getColumn(2); }
    };
}

#endif
