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
#ifndef ECLIPSE_GRIDPROPERTY_HPP_
#define ECLIPSE_GRIDPROPERTY_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <boost/lexical_cast.hpp>

#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/Box.hpp>

/*
  This class implemenents a class representing properties which are
  define over an ECLIPSE grid, i.e. with one value for each logical
  cartesian cell in the grid. 

  The class is implemented as a thin wrapper around std::vector<T>;
  where the most relevant specialisations of T are 'int' and 'float'.
*/




namespace Opm {

template <class DataType>
class GridPropertySupportedKeywordInfo;

template <>
class GridPropertySupportedKeywordInfo<int>
{
public:
    GridPropertySupportedKeywordInfo()
    {}

    GridPropertySupportedKeywordInfo(const std::string& name,
                                     int defaultValue)
        : m_keywordName(name)
        , m_defaultValue(defaultValue)
    {}

    GridPropertySupportedKeywordInfo(const GridPropertySupportedKeywordInfo &other)
        : m_keywordName(other.m_keywordName)
        , m_defaultValue(other.m_defaultValue)
    {}

    const std::string& getKeywordName() const {
        return m_keywordName;
    }

    int getDefaultValue() const {
        return m_defaultValue;
    }

private:
    std::string m_keywordName;
    int m_defaultValue;
};

template <>
class GridPropertySupportedKeywordInfo<double>
{
public:
    GridPropertySupportedKeywordInfo()
    {}

    GridPropertySupportedKeywordInfo(const std::string& name,
                                     double defaultValue,
                                     const std::string& dimensionString)
        : m_keywordName(name)
        , m_defaultValue(defaultValue)
        , m_dimensionString(dimensionString)
    {}

    GridPropertySupportedKeywordInfo(const GridPropertySupportedKeywordInfo &other)
        : m_keywordName(other.m_keywordName)
        , m_defaultValue(other.m_defaultValue)
        , m_dimensionString(other.m_dimensionString)
    {}

    const std::string& getKeywordName() const {
        return m_keywordName;
    }

    double getDefaultValue() const {
        return m_defaultValue;
    }

    const std::string& getDimensionString() const {
        return m_dimensionString;
    }

private:
    std::string m_keywordName;
    double m_defaultValue;
    std::string m_dimensionString;
};

template <typename T>
class GridProperty {
public:
    typedef GridPropertySupportedKeywordInfo<T> SupportedKeywordInfo;

    GridProperty(size_t nx , size_t ny , size_t nz , const SupportedKeywordInfo& kwInfo) {
        m_nx = nx;
        m_ny = ny;
        m_nz = nz;
        m_kwInfo = kwInfo;
        m_data.resize( nx * ny * nz );
        std::fill( m_data.begin() , m_data.end() ,  m_kwInfo.getDefaultValue());
    }

    size_t size() const {
        return m_data.size();
    }

    
    T iget(size_t index) const {
        if (index < m_data.size()) {
            return m_data[index];
        } else {
            throw std::invalid_argument("Index out of range \n");
        }
    }


    T iget(size_t i , size_t j , size_t k) const {
        size_t g = i + j*m_nx + k*m_nx*m_ny;
        return iget(g);
    }


    void multiplyValueAtIndex(size_t index, T factor) {
        m_data[index] *= factor;
    }

    const std::vector<T>& getData() const {
        return m_data;
    }

    void loadFromDeckKeyword(std::shared_ptr<const Box> inputBox , DeckKeywordConstPtr deckKeyword);
    void loadFromDeckKeyword(DeckKeywordConstPtr deckKeyword);    


    
    void copyFrom(const GridProperty<T>& src, std::shared_ptr<const Box> inputBox) {
        if (inputBox->isGlobal()) {
            for (size_t i = 0; i < src.size(); ++i)
                m_data[i] = src.m_data[i];
        } else {
            const std::vector<size_t>& indexList = inputBox->getIndexList();
            for (size_t i = 0; i < indexList.size(); i++) {
                size_t targetIndex = indexList[i];
                m_data[targetIndex] = src.m_data[targetIndex];
            }
        }
    }
    
    void scale(T scaleFactor , std::shared_ptr<const Box> inputBox) {
        if (inputBox->isGlobal()) {
            for (size_t i = 0; i < m_data.size(); ++i)
                m_data[i] *= scaleFactor;
        } else {
            const std::vector<size_t>& indexList = inputBox->getIndexList();
            for (size_t i = 0; i < indexList.size(); i++) {
                size_t targetIndex = indexList[i];
                m_data[targetIndex] *= scaleFactor;
            }
        }
    }


    void add(T shiftValue , std::shared_ptr<const Box> inputBox) {
        if (inputBox->isGlobal()) {
            for (size_t i = 0; i < m_data.size(); ++i)
                m_data[i] += shiftValue;
        } else {
            const std::vector<size_t>& indexList = inputBox->getIndexList();
            for (size_t i = 0; i < indexList.size(); i++) {
                size_t targetIndex = indexList[i];
                m_data[targetIndex] += shiftValue;
            }
        }
    }


    

    void setScalar(T value , std::shared_ptr<const Box> inputBox) {
        if (inputBox->isGlobal()) {
            std::fill(m_data.begin(), m_data.end(), value);
        } else {
            const std::vector<size_t>& indexList = inputBox->getIndexList();
            for (size_t i = 0; i < indexList.size(); i++) {
                size_t targetIndex = indexList[i];
                m_data[targetIndex] = value;
            }
        }
    }

    const std::string& getKeywordName() const {
        return m_kwInfo.getKeywordName();
    }

    const SupportedKeywordInfo& getKeywordInfo() const {
        return m_kwInfo;
    }

private:
    void setFromVector(const std::vector<T>& data) {
        if (data.size() <= m_data.size()) {
            // note that the 'data' vector (which gets specified in the deck) may be
            // smaller than the size of the grid property (i.e., the 'm_data'
            // vector). The remaining values are left at their defaults...
            for (size_t i = 0; i < data.size(); i++)
                m_data[i] = data[i];
        } else
            throw std::invalid_argument("Size mismatch when setting data for:" + getKeywordName() + " keyword size: " + boost::lexical_cast<std::string>(m_data.size()) + " input size: " + boost::lexical_cast<std::string>(data.size()));
    }

    void setFromVector(std::shared_ptr<const Box> inputBox , const std::vector<T>& data) {
        if (inputBox->isGlobal()) {
            setFromVector(data);
        } else {
            const std::vector<size_t>& indexList = inputBox->getIndexList();
            if (data.size() == indexList.size()) {
                for (size_t i = 0; i < data.size(); i++) {
                    size_t targetIndex = indexList[i];
                    m_data[targetIndex] = data[i];
                }
            } else
                throw std::invalid_argument("Size mismatch when setting data for:" + getKeywordName() + " box size: " + boost::lexical_cast<std::string>(inputBox->size()) + " input size: " + boost::lexical_cast<std::string>(data.size()));
        }
    }

    size_t      m_nx,m_ny,m_nz;
    SupportedKeywordInfo m_kwInfo;
    std::vector<T> m_data;
};

}
#endif
