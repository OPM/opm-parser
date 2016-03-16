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
#ifndef ECLIPSE_GRIDPROPERTY_INITIALIZERS_HPP
#define ECLIPSE_GRIDPROPERTY_INITIALIZERS_HPP

#include <vector>

/*
  This classes initialize GridProperty objects. Most commonly, they just get a constant
  value but some properties (e.g, some of these related to endpoint scaling) need more
  complex schemes like table lookups.
*/
namespace Opm {

class Deck;
class EclipseGrid;
class TableManager;
template <typename T> class GridProperties;

template< typename T >
    class GridPropertyInitFunction {
        public:
            using signature = std::vector< T >(*)(
                        size_t,
                        std::shared_ptr<const TableManager>,
                        std::shared_ptr<const EclipseGrid>,
                        std::shared_ptr<GridProperties<int>>
                    );

            GridPropertyInitFunction(
                    signature,
                    std::shared_ptr<const TableManager>,
                    std::shared_ptr<const EclipseGrid>,
                    std::shared_ptr<GridProperties<int>>
                    );

            GridPropertyInitFunction( T );
            std::vector< T > operator()( size_t ) const;

        private:
            signature f = nullptr;
            T constant;
            std::shared_ptr<const TableManager>  tm  = nullptr;
            std::shared_ptr<const EclipseGrid>   eg   = nullptr;
            std::shared_ptr<GridProperties<int>> gp = nullptr;
              };

template< typename T >
    class GridPropertyPostFunction {
        public:
            using signature = void(*)( std::vector< T >&,
                                       //const EclipseState&
                                       std::shared_ptr<const TableManager>,
                                       std::shared_ptr<const EclipseGrid>,
                                       std::shared_ptr<GridProperties<int>>

            GridPropertyPostFunction() = default;
            GridPropertyPostFunction(
                    signature,
                    // const EclipseState& );
                    std::shared_ptr<const TableManager>,
                    std::shared_ptr<const EclipseGrid>,
                    std::shared_ptr<GridProperties<int>>
                );

            void operator()( std::vector< T >& ) const;

        private:
            signature f = nullptr;
            // const EclipseState* es = nullptr;
            std::shared_ptr<const TableManager> tm;
            std::shared_ptr<const EclipseGrid> eg;
            std::shared_ptr<GridProperties<int>> gp;

    };

    // initialize the TEMPI grid property using the temperature vs depth
    // table (stemming from the TEMPVD or the RTEMPVD keyword)
    // std::vector< double > temperature_lookup( size_t, const EclipseState& );
    std::vector< double > temperature_lookup( size_t,
                                              std::shared_ptr<const TableManager>,
                                              std::shared_ptr<const EclipseGrid>,
                                              std::shared_ptr<GridProperties<int>>);


}

#endif
