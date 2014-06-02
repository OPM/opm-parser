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
#ifndef ECLIPSE_GRIDPROPERTIES_HPP_
#define ECLIPSE_GRIDPROPERTIES_HPP_


#include <string>
#include <vector>
#include <unordered_map>

#include <opm/parser/eclipse/EclipseState/Grid/GridProperty.hpp>

/*
  This class implements a container (std::unordered_map<std::string ,
  Gridproperty<T>>) of Gridproperties. Usage is as follows:

    1. Instantiate the class; passing the number of grid cells and the
       supported keywords as a list of strings to the constructor.

    2. Query the container with the supportsKeyword() and hasKeyword()
       methods. 
       
    3. When you ask the container to get a keyword with the
       getKeyword() method it will automatically create a new
       GridProperty object if the container does not have this
       property. 
*/


namespace Opm {

template <typename T>
class GridProperties {
public:

    GridProperties(size_t nx , size_t ny , size_t nz , const std::vector<std::pair<std::string, T> > & supportedKeywords) {
        m_nx = nx;
        m_ny = ny;
        m_nz = nz;
        
        for (auto iter = supportedKeywords.begin(); iter != supportedKeywords.end(); ++iter) 
            m_supportedKeywords.insert( std::pair<std::string , T>(*iter));
    }

    
    bool supportsKeyword(const std::string& keyword) {
        if (m_supportedKeywords.find( keyword ) == m_supportedKeywords.end())
            return false;
        else
            return true;
    }

    bool hasKeyword(const std::string& keyword) {
        if (m_properties.find( keyword ) == m_properties.end())
            return false;
        else
            return true;
    }


    std::shared_ptr<GridProperty<T> > getKeyword(const std::string& keyword) {
        if (!hasKeyword(keyword))
            addKeyword(keyword);
        
        {
            auto pair = m_properties.find( keyword );
            return (*pair).second;
        }
    }

    
    bool addKeyword(const std::string& keyword) {
        if (!supportsKeyword( keyword )) 
            throw std::invalid_argument("The keyword: " + keyword + " is not supported in this container");

        if (hasKeyword(keyword)) 
            return false;
        else {
            auto supportedKeyword = m_supportedKeywords.find( keyword );
            std::shared_ptr<GridProperty<T> > newProperty(new GridProperty<T>(m_nx , m_ny , m_nz , keyword , (*supportedKeyword).second));
            m_properties.insert( std::pair<std::string , std::shared_ptr<GridProperty<T> > > ( keyword , newProperty ));
            return true;
        }
    }

    
private:
    size_t m_nx, m_ny, m_nz;
    std::unordered_map<std::string, T> m_supportedKeywords;
    std::map<std::string , std::shared_ptr<GridProperty<T> > > m_properties;
};

}

#endif
