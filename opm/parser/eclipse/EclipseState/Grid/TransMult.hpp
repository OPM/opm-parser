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

/**
   This class implements a small container which holds the
   transmissibility mulitpliers for all the faces in the grid. There
   are a total of (nx + 1) * (ny + 1) * (nz + 1) faces in a cartesian
   grid. The method call 

        getXFaceMultiplier(i,j,k) 

   will give the transmissibility multiplier on the face between cells
   (i-1,j,k) and (i,j,k). The multipliers in this class are built up
   from the transmissibility modifier keywords: 

      {MULTX , MULTX- , MULTY , MULTY- , MULTZ , MULTZ-, MULTFLT , MULTREGT}

*/




#include <cstddef>
#include <vector>
#include <tuple>

#ifndef TRANSMULT_HPP
#define TRANSMULT_HPP

namespace Opm {
    class TransMult {

    public:
        TransMult(size_t nx , size_t ny , size_t nz);
        double getXFaceMultiplier( size_t i , size_t j , size_t k) const;
        double getYFaceMultiplier( size_t i , size_t j , size_t k) const;
        double getZFaceMultiplier( size_t i , size_t j , size_t k) const;
        std::tuple<double,double,double> getCellMultipliers(size_t i , size_t j , size_t k) const;

    private:
        size_t getGlobalIndex(size_t i , size_t j , size_t k) const;
        void assertIJK(size_t i , size_t j , size_t k) const;

        size_t m_nx , m_ny , m_nz;
        std::vector<std::tuple<double,double,double> > m_multiplier;
    };

}

#endif
