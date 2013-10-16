/*
  Copyright 2013 Statoil ASA.

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

#include <string>
#include <iostream>
#include <stdexcept>

#include <opm/parser/eclipse/RawDeck/StarToken.hpp>



namespace Opm {

    bool tokenContainsStar(const std::string& token) {
        if (token.find('*') == std::string::npos)
            return false;
        else
            return true;
    }


  
    template <>
    int readValueToken(const std::string& valueToken) {
        char * error_ptr;
        int value = strtol( valueToken.c_str() , &error_ptr , 10);
        if (error_ptr[0] != C_EOS)
            throw std::invalid_argument("Parsing integer value from: " + valueToken + " failed");
        return value;
    }
    

    template <>
    double readValueToken(const std::string& valueToken) {
        char * error_ptr;
        double value = strtod( valueToken.c_str() , &error_ptr);
        if (error_ptr[0] != C_EOS)
            throw std::invalid_argument("Parsing double value from: " + valueToken + " failed");
        return value;
    }


    template <>
    std::string readValueToken(const std::string& valueToken) {
        return valueToken;
    }


    
}
