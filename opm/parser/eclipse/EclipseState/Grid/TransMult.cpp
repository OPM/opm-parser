/*
  Copyright 2014 Statoil ASA.

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
#include <iostream>

#include <opm/parser/eclipse/EclipseState/Grid/TransMult.hpp>

namespace Opm {

    TransMult::TransMult(size_t nx , size_t ny , size_t nz) :
        m_nx(nx),
        m_ny(ny),
        m_nz(nz) 
    {
        std::tuple<double,double,double> initValue{1.0, 1.0, 1.0};
        m_multiplier.resize( (nx + 1) * (ny + 1) * (nz + 1) );
        std::fill( m_multiplier.begin() , m_multiplier.end() , initValue);
    }


    void TransMult::assertIJK(size_t i , size_t j , size_t k) const {
        if ((i > m_nx) || (j > m_ny) || (k > m_nz))
            throw std::invalid_argument("Invalid ijk");
    }


    size_t TransMult::getGlobalIndex(size_t i , size_t j , size_t k) const {
        assertIJK(i,j,k);
        return i + j*(m_nx + 1) + k*(m_nx + 1) * (m_ny + 1);
    }

    std::tuple<double,double,double> TransMult::getCellMultipliers(size_t i , size_t j , size_t k) const {
        size_t globalIndex = getGlobalIndex(i,j,k);
        return m_multiplier[globalIndex];
    }
    
    double TransMult::getXFaceMultiplier( size_t i , size_t j , size_t k) const {
        return std::get<0>(getCellMultipliers(i,j,k));
    }

    double TransMult::getYFaceMultiplier( size_t i , size_t j , size_t k) const {
        return std::get<1>(getCellMultipliers(i,j,k));
    }

    double TransMult::getZFaceMultiplier( size_t i , size_t j , size_t k) const {
        return std::get<2>(getCellMultipliers(i,j,k));
    }

}
