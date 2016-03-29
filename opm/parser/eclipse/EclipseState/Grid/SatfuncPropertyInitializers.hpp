/*
  Copyright 2014 Andreas Lauser

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
#ifndef ECLIPSE_SATFUNCPROPERTY_INITIALIZERS_HPP
#define ECLIPSE_SATFUNCPROPERTY_INITIALIZERS_HPP

#include <vector>
#include <string>

namespace Opm {

    template< typename > class GridProperties;

    class EclipseGrid;
    class TableManager;

    typedef const TableManager* TM; //FIXME PGDR use using?
    typedef const EclipseGrid*      EG;
    typedef GridProperties<int>*    IGP;
    typedef GridProperties<double>* DGP;

    std::vector< double > SGLEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISGLEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > SGUEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISGUEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > SWLEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISWLEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > SWUEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISWUEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > SGCREndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISGCREndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > SOWCREndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISOWCREndpoint( size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > SOGCREndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISOGCREndpoint( size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > SWCREndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > ISWCREndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > PCWEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IPCWEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > PCGEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IPCGEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > KRWEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IKRWEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > KRWREndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IKRWREndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > KROEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IKROEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > KRORWEndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IKRORWEndpoint( size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > KRORGEndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IKRORGEndpoint( size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > KRGEndpoint(    size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IKRGEndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > KRGREndpoint(   size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);
    std::vector< double > IKRGREndpoint(  size_t, TM tm, EG eg, IGP intGridProperties, DGP doubleGridProperties);

}

#endif
