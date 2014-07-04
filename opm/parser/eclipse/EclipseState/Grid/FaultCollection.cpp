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



    void FaultCollection::addFace(const std::string& faultName, int I1 , int I2 , int J1 , int J2 , int K1 , int K2 , FaceDir::DirEnum faceDir) {
        /*std::shared_ptr<FaultFace> face = std::make_shared<FaultFace>(m_nx , m_ny , m_nz , 
          static_cast<size_t>(I1) , static_cast<size_t>(I2) , 
          static_cast<size_t>(J1) , static_cast<size_t>(J2) , 
          static_cast<size_t>(K1) , static_cast<size_t>(K2) , 
          faceDir);
        */

        
        if (!hasFault(faultName)) {
            std::shared_ptr<Fault> fault = std::make_shared<Fault>( faultName );
            addFault( fault );
        }
        {
            std::shared_ptr<Fault> fault = getFault( faultName );
        
        }
    }


}
