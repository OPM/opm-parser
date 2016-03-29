#include <algorithm>
#include <limits>
#include <vector>
#include <array>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperty.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridPropertyInitializers.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/RtempvdTable.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableManager.hpp>


namespace Opm {

    template< typename T >
    GridPropertyInitFunction< T >::GridPropertyInitFunction( T x ) :
        constant( x )
    {}

    template< typename T >
    GridPropertyInitFunction< T >::GridPropertyInitFunction(
            signature fn,
            const TableManager* tables,
            const EclipseGrid* grid,
            GridProperties<int>* ig_props,
            GridProperties<double>* dg_props)
        :
            f( fn ),
            // es( &state )
            tm ( tables ),
            eg ( grid ),
            igp ( ig_props ),
            dgp ( dg_props )
    {}

    template< typename T >
    std::vector< T > GridPropertyInitFunction< T >::operator()( size_t size ) const {
        if( !this->f ) return std::vector< T >( size, this->constant );

        return (*this->f)( size, this->tm, this->eg, this->igp, this->dgp );
    }

    template< typename T >
    GridPropertyPostFunction< T >::GridPropertyPostFunction(
            signature fn,
            const TableManager* tables,
            const EclipseGrid* grid,
            GridProperties<int>* ig_props,
            GridProperties<double>* dg_props)
        :
            f( fn ),
            tm ( tables ),
            eg ( grid ),
            igp ( ig_props ),
            dgp ( dg_props )
    {}

    template< typename T >
    void GridPropertyPostFunction< T >::operator()( std::vector< T >& values ) const {
        if (!this->f)
            return;
        return (*this->f)( values, this->tm, this->eg, this->igp, this->dgp );
    }

    std::vector< double > temperature_lookup(
            size_t size,
            const TableManager* tables,
            const EclipseGrid* grid,
            GridProperties<int>* ig_props,
            GridProperties<double>* dg_props) {

        if( !tables->useEqlnum() ) {
            /* if values are defaulted in the TEMPI keyword, but no
             * EQLNUM is specified, you will get NaNs
             */
            return std::vector< double >( size, std::numeric_limits< double >::quiet_NaN() );
        }

        std::vector< double > values( size, 0 );

        const auto& rtempvdTables = tables->getRtempvdTables();
        const std::vector< int >& eqlNum = ig_props->getKeyword("EQLNUM").getData();

        for (size_t cellIdx = 0; cellIdx < eqlNum.size(); ++ cellIdx) {
            int cellEquilNum = eqlNum[cellIdx];
            const RtempvdTable& rtempvdTable = rtempvdTables.getTable<RtempvdTable>(cellEquilNum);
            double cellDepth = std::get<2>(grid->getCellCenter(cellIdx));
            values[cellIdx] = rtempvdTable.evaluate("Temperature", cellDepth);
        }

        return values;
    }
}

template class Opm::GridPropertyInitFunction< int >;
template class Opm::GridPropertyInitFunction< double >;
template class Opm::GridPropertyPostFunction< int >;
template class Opm::GridPropertyPostFunction< double >;
