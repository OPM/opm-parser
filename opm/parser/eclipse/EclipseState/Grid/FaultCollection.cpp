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

#include <opm/parser/eclipse/EclipseState/Grid/FaultCollection.hpp>

namespace Opm {

    FaultCollection::FaultCollection(size_t nx , size_t ny , size_t nz) :
        m_nx(nx),
        m_ny(ny),
        m_nz(nz)
    {
        
    }

    size_t FaultCollection::size() const {
        return m_faults.size();
    }
    

    bool FaultCollection::hasFault(const std::string& faultName) const {
        if (m_faults.count(faultName) == 1)
            return true;
        else
            return false;
    }
    
    std::shared_ptr<Fault> FaultCollection::getFault(const std::string& faultName) const {
        if (hasFault( faultName))
            return m_faults.at(faultName);
        else
            throw std::invalid_argument("Unknown fault: " + faultName);
    }
    
    
    void FaultCollection::addFault(std::shared_ptr<Fault> fault) {
        m_faults[fault->getName()] = fault;
    }


    void FaultCollection::checkCoord(size_t dim , int l1 , int l2) {
        if (l1 > l2)
            throw std::invalid_argument("Invalid coordinates");

        if (l1 < 0)
            throw std::invalid_argument("Invalid coordinates");

        if (static_cast<size_t>(l2) >= dim)
            throw std::invalid_argument("Invalid coordinates");            
    }


    void FaultCollection::addFace(const std::string& faultName, int I1 , int I2 , int J1 , int J2 , int K1 , int K2 , FaceDir::DirEnum faceDir) {
        checkCoord(m_nx , I1,I2);
        checkCoord(m_ny , J1,J2);
        checkCoord(m_nz , K1,K2);

        if ((faceDir == FaceDir::XPlus) || (faceDir == FaceDir::XMinus))
            if (I1 != I2)
                throw std::invalid_argument("When the face is in X direction we must have I1 == I2");

        if ((faceDir == FaceDir::YPlus) || (faceDir == FaceDir::YMinus))
            if (J1 != J2)
                throw std::invalid_argument("When the face is in Y direction we must have J1 == J2");

        if ((faceDir == FaceDir::ZPlus) || (faceDir == FaceDir::ZMinus))
            if (K1 != K2)
                throw std::invalid_argument("When the face is in Z direction we must have K1 == K2");
        
        if (!hasFault(faultName)) {
            std::shared_ptr<Fault> fault = std::make_shared<Fault>( faultName );
            addFault( fault );
        }
    }


}
