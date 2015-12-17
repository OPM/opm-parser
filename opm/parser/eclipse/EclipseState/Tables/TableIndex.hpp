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


#ifndef _TABLE_INDEX_HPP_
#define _TABLE_INDEX_HPP_

#include <cstddef>

/*
  Small class used to simplify lookup in tables based on linear
  interpolation; first binear search in an ordered column is used to
  instantiate a TableIndex object, and then subsequently that
  TableIndex instance is used to evaluate the corresponding value over
  another column.
*/

namespace Opm {

    class TableIndex {
    public:
        TableIndex( size_t index1 , double weight1 , double weight2);

    private:
        size_t m_index1;
        double m_weight1  , m_weight2;
    };
}


#endif
