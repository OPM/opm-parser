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

#include <opm/parser/eclipse/Parser/ParserLog.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/BoxManager.hpp>


namespace Opm {

    BoxManager::BoxManager(int nx , int ny , int nz) {
        m_globalBox = std::make_shared<const Box>(nx,ny,nz);
    }


    std::shared_ptr<const Box> BoxManager::getGlobalBox() const {
        return m_globalBox;
    }


    std::shared_ptr<const Box> BoxManager::getInputBox() const {
        return m_inputBox;
    }


    std::shared_ptr<const Box> BoxManager::getKeywordBox() const {
        return m_keywordBox;
    }

    std::shared_ptr<const Box> BoxManager::getActiveBox() const {
        if (m_keywordBox)
            return m_keywordBox;

        if (m_inputBox)
            return m_inputBox;

        return m_globalBox;
    }


    void BoxManager::setInputBox(Opm::ParserLogPtr parserLog, const std::string& fileName, int lineNumber, int i1,int i2 , int j1 , int j2 , int k1 , int k2) {
        if (i1 < 0 || (int) m_globalBox->getDim(0) <= i1 ||
            i2 < 0 || (int) m_globalBox->getDim(0) <= i2) {
            std::string msg("X indices for boxes must be in range [1, "+std::to_string((long long) m_globalBox->getDim(0)) + "]. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (j1 < 0 || (int) m_globalBox->getDim(1) <= j1 ||
                 j2 < 0 || (int) m_globalBox->getDim(1) <= j2) {
            std::string msg("Y indices for boxes must be in range [1, "+std::to_string((long long) m_globalBox->getDim(1)) + "]. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (k1 < 0 || (int) m_globalBox->getDim(2) <= k1 ||
                 k2 < 0 || (int) m_globalBox->getDim(2) <= k2) {
            std::string msg("Z indices for boxes must be in range [1, "+std::to_string((long long) m_globalBox->getDim(2)) + "]. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (i1 > i2) {
            std::string msg("The 'begin' index for the X coordinate must be smaller than or equal to the end index in boxes. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (j1 > j2) {
            std::string msg("The 'begin' index for the Y coordinate must be smaller than or equal to the end index in boxes. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (k1 > k2) {
            std::string msg("The 'begin' index for the Y coordinate must be smaller than or equal to the end index in boxes. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else
            m_inputBox.reset( new Box( *m_globalBox , i1,i2,j1,j2,k1,k2) );
    }

    void BoxManager::endInputBox() {
        if (m_keywordBox)
            throw std::invalid_argument("Hmmm - this seems like an internal error - the SECTION is terminated with an active keyword box");
        
        m_inputBox.reset( );
    }

    void BoxManager::endSection() {
        endInputBox();
    }

    void BoxManager::setKeywordBox(Opm::ParserLogPtr parserLog, const std::string& fileName, int lineNumber, int i1,int i2 , int j1 , int j2 , int k1 , int k2) {
        if (i1 < 0 || (int) m_globalBox->getDim(0) <= i1 ||
            i2 < 0 || (int) m_globalBox->getDim(0) <= i2) {
            std::string msg("X indices for boxes must be in range [1, "+std::to_string((long long) m_globalBox->getDim(0)) + "]. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (j1 < 0 || (int) m_globalBox->getDim(1) <= j1 ||
                 j2 < 0 || (int) m_globalBox->getDim(1) <= j2) {
            std::string msg("Y indices for boxes must be in range [1, "+std::to_string((long long) m_globalBox->getDim(1)) + "]. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (k1 < 0 || (int) m_globalBox->getDim(2) <= k1 ||
                 k2 < 0 || (int) m_globalBox->getDim(2) <= k2) {
            std::string msg("Z indices for boxes must be in range [1, "+std::to_string((long long) m_globalBox->getDim(2)) + "]. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (i1 > i2) {
            std::string msg("The 'begin' index for the X coordinate must be smaller than or equal to the end index in boxes. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (j1 > j2) {
            std::string msg("The 'begin' index for the Y coordinate must be smaller than or equal to the end index in boxes. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else if (k1 > k2) {
            std::string msg("The 'begin' index for the Y coordinate must be smaller than or equal to the end index in boxes. Ignoring box.");
            parserLog->addError(fileName, lineNumber, msg);
        }
        else
            m_keywordBox.reset( new Box( *m_globalBox , i1,i2,j1,j2,k1,k2) );
    }

    void BoxManager::endKeyword() {
        m_keywordBox.reset( );
    }

}
