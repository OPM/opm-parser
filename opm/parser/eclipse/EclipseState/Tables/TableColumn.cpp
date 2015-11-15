/*
  Copyright 2015 Statoil ASA.

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
#include <stdexcept>

#include <opm/parser/eclipse/EclipseState/Tables/TableColumn.hpp>

namespace Opm {

    TableColumn::TableColumn(const ColumnSchema& schema) :
        m_schema( schema )
    {
    }



    size_t TableColumn::size() const {
        return m_values.size();
    }


    void TableColumn::assertOrder(double value1 , double value2) const {
        if (!m_schema.validOrder( value1 , value2) )
            throw std::invalid_argument("Incorrect ordering of values");
    }



    void TableColumn::assertNext(size_t index , double value) const {
        size_t nextIndex = index + 1;
        if (nextIndex < m_values.size()) {
            if (!m_default[nextIndex]) {
                double nextValue = m_values[nextIndex];
                assertOrder( value , nextValue );
            }
        }
    }


    void TableColumn::assertPrevious(size_t index , double value) const {
        if (index > 0) {
            size_t prevIndex = index - 1;
            if (!m_default[prevIndex]) {
                double prevValue = m_values[prevIndex];
                assertOrder( prevValue , value );
            }
        }
    }


    void TableColumn::assertUpdate(size_t index, double value) const {
        assertNext( index , value );
        assertPrevious( index, value );
    }




    void TableColumn::addValue(double value) {
        assertUpdate( m_values.size() , value );
        m_values.push_back( value );
        m_default.push_back( false );
    }


    void TableColumn::addDefault() {
        m_values.push_back( -1 ); // Should never even be read.
        m_default.push_back( true );
    }


    void TableColumn::updateValue(  size_t index , double value ) {
        assertUpdate( index , value );

        m_values[index] = value;
        m_default[index] = false;
    }

    bool TableColumn::defaultApplied(size_t index) const {
        if (index >= m_values.size())
            throw std::invalid_argument("Value: " + std::to_string( index ) + " out of range: [0," + std::to_string( m_values.size()) + ")");

        return m_default[index];
    }

    double TableColumn::operator[](size_t index) const {
        if (index >= m_values.size())
            throw std::invalid_argument("Value: " + std::to_string( index ) + " out of range: [0," + std::to_string( m_values.size()) + ")");

        if (m_default[index])
            throw std::invalid_argument("Value at index " + std::to_string( index ) + " is defaulted - can not ask!");

        return m_values[index];
    }

    double TableColumn::back() const {
        return m_values.back( );
    }


    double TableColumn::front() const {
        return m_values.front( );
    }

}


