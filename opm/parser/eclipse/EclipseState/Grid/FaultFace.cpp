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

#include <opm/parser/eclipse/EclipseState/Grid/FaultFace.hpp>

namespace Opm {

    FaultFace::FaultFace(ParserLogPtr parserLog, const std::string& fileName, int lineNumber,
                         size_t nx , size_t ny , size_t nz,
                         size_t I1 , size_t I2,
                         size_t J1 , size_t J2,
                         size_t K1 , size_t K2,
                         FaceDir::DirEnum faceDir) 
        : m_faceDir( faceDir )
    {
        if (!checkCoord(parserLog, fileName, lineNumber, nx, I1, I2) ||
            !checkCoord(parserLog, fileName, lineNumber, ny, J1, J2) ||
            !checkCoord(parserLog, fileName, lineNumber, nz, K1, K2))
            return;

        if ((faceDir == FaceDir::XPlus) || (faceDir == FaceDir::XMinus)) {
            if (I1 != I2) {
                std::string msg("X coordinates must exhibit the same values it the face multiplier is for the X direction.");
                parserLog->addError(fileName, lineNumber, msg);
                return;
            }
        }
        if ((faceDir == FaceDir::YPlus) || (faceDir == FaceDir::YMinus)) {
            if (J1 != J2) {
                std::string msg("Y coordinates must exhibit the same values it the face multiplier is for the Y direction.");
                parserLog->addError(fileName, lineNumber, msg);
                return;
            }
        }

        if ((faceDir == FaceDir::ZPlus) || (faceDir == FaceDir::ZMinus)) {
            if (K1 != K2) {
                std::string msg("Z coordinates must exhibit the same values it the face multiplier is for the Z direction.");
                parserLog->addError(fileName, lineNumber, msg);
                return;
            }
        }

        for (size_t k=K1; k <= K2; k++)
            for (size_t j=J1; j <= J2; j++)
                for (size_t i=I1; i <= I2; i++) {
                    size_t globalIndex = i + j*nx + k*nx*ny;
                    m_indexList.push_back( globalIndex );
                }
    }

    
    bool FaultFace::checkCoord(ParserLogPtr parserLog, const std::string& fileName, int lineNumber,
                               size_t dim , size_t l1 , size_t l2) {
        if (l1 > l2) {
            std::string msg("Invalid coordinates: ["
                            +std::to_string((long long) l1+1)+", " +std::to_string((long long) l2+1)+"] is not a valid range");
            parserLog->addError(fileName, lineNumber, msg);
            return false;
        }

        if (l2 >= dim) {
            std::string msg("Invalid coordinates: "
                            +std::to_string((long long) l2+1)+" exceeds number of cells ("+std::to_string((long long) dim)+")");
            parserLog->addError(fileName, lineNumber, msg);
            return false;
        }
        return true;
    }


    std::vector<size_t>::const_iterator FaultFace::begin() const {
        return m_indexList.begin();
    }
    
    std::vector<size_t>::const_iterator FaultFace::end() const {
        return m_indexList.end();
    }
    

    FaceDir::DirEnum FaultFace::getDir() const {
        return m_faceDir;
    }

}
