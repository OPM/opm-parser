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

#include <opm/parser/eclipse/EclipseState/Tables/ColumnSchema.hpp>

namespace Opm {

    ColumnSchema::ColumnSchema(const std::string& name , Table::ColumnOrderEnum order) :
        m_name( name ),
        m_order( order )
    {

    }


    const std::string& ColumnSchema::name() const {
        return m_name;
    }


    bool ColumnSchema::validOrder( double value1 , double value2) const {
        switch (m_order) {
        case Table::RANDOM:
            return true;
            break;
        case Table::INCREASING:
            return (value2 >= value1);
            break;
        case Table::STRICTLY_INCREASING:
            return (value2 > value1);
            break;
        case Table::DECREASING:
            return (value2 <= value1);
            break;
        case Table::STRICTLY_DECREASING:
            return (value2 < value1);
            break;
        default:
            throw std::invalid_argument("Internal error - should not be here\n");
        }
    }
}




