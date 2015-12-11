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
#ifndef OPM_PARSER_PVTO_TABLE_HPP
#define	OPM_PARSER_PVTO_TABLE_HPP

#include <opm/parser/eclipse/EclipseState/Tables/PvtxTable.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/PvtoInnerTable.hpp>

namespace Opm {
    class PvtoTable : public PvtxTable {
    public:

        PvtoTable(Opm::DeckKeywordConstPtr keyword , size_t tableIdx) :
            m_columnSchema( "RS" , Table::STRICTLY_INCREASING , Table::DEFAULT_NONE ),
            m_column( m_columnSchema )
        {
            auto ranges = recordRanges( keyword );
            if (tableIdx >= ranges.size())
                throw std::invalid_argument("Asked for table: " + std::to_string( tableIdx ) + " in keyword + " + keyword->name() + " which only has " + std::to_string( ranges.size() ) + " tables");

            m_recordRange = ranges[ tableIdx ];
            for (size_t  rowIdx = m_recordRange.first; rowIdx < m_recordRange.second; rowIdx++) {
                Opm::DeckRecordConstPtr deckRecord = keyword->getRecord(rowIdx);
                {
                    Opm::DeckItemConstPtr indexItem = deckRecord->getItem(0);
                    m_column.addValue( indexItem->getSIDouble( 0 ));
                }
                {
                    Opm::DeckItemConstPtr dataItem = deckRecord->getItem(1);
                    //m_tables.emplace_back( dataItem );
                    m_tables.push_back( std::make_shared<const PvtoInnerTable>( dataItem ));
                }
            }
        }


        const PvtoInnerTable& getInnerTable(size_t tableNumber) const {
            if (tableNumber >= size())
                throw std::invalid_argument("Invalid table number: " + std::to_string( tableNumber) + " max: " + std::to_string( size() - 1 ));
            return *m_tables[ tableNumber ];
        }


        const TableColumn& getGasSolubilityColumn() const
        {
            return m_column;
        }


        size_t size() const {
            return m_column.size();
        }


        const std::pair<size_t, size_t>& recordRange() const {
            return m_recordRange;
        }

    private:
        ColumnSchema m_columnSchema;
        TableColumn m_column;
        std::pair<size_t, size_t> m_recordRange;
        std::vector<std::shared_ptr<const PvtoInnerTable> > m_tables;
        //std::vector<PvtoInnerTable> m_tables;
    };
}

#endif
