#include <iostream>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/saturation.hpp>
#include <opm/parser/eclipse/EclipseState/Runspec.hpp>

namespace Opm {

struct satfuns {

    static std::vector< std::vector< saturation::water_oil > >
    woSWOF( const DeckKeyword& kw ) {
        decltype( woSWOF( kw ) ) v;
        v.emplace_back();

        for( const auto& record : kw ) {
            v.emplace_back();
            auto& current = v.back();

            for( const auto& item : record ) {
                assert( item.size() % 4 == 0 );

                for( int i = 0; i < int( item.size() ); i += 4 ) {
                    current.push_back(
                        saturation::water_oil {
                            item.get< double >( i + 0 ),
                            item.get< double >( i + 1 ),
                            item.get< double >( i + 3 ),
                            item.get< double >( i + 0 ),
                            item.get< double >( i + 2 ),
                            }
                        );
                }
            }
        }

        return v;
    }

    static std::vector< std::vector< saturation::water_oil > >
    woSWFN( const DeckKeyword& kw, std::vector< std::vector< saturation::water_oil > > res ) {
        res.resize( kw.size() + 1 );

        int reci = 1;
        for( const auto& record : kw ) {
            auto& current = res[ reci++ ];

            for( const auto& item : record ) {
                assert( item.size() % 3 == 0 );

                current.resize( item.size() / 3 );

                for( int i = 0; i < int( item.size() / 3 ); ++i ) {
                    current[ i ].Sw   = item.get< double >( (i * 3) + 0 );
                    current[ i ].Krw  = item.get< double >( (i * 3) + 1 );
                    current[ i ].Pcow = item.get< double >( (i * 3) + 2 );
                }
            }
        }

        return res;
    }

    static std::vector< std::vector< saturation::water_oil > >
    woSOF3( const DeckKeyword& kw, std::vector< std::vector< saturation::water_oil > > res ) {
        res.resize( kw.size() + 1 );

        int reci = 1;
        for( const auto& record : kw ) {
            auto& current = res.at( reci++ );

            for( const auto& item : record ) {
                assert( item.size() % 3 == 0 );

                current.resize( item.size() / 3 );

                int sz = (item.size() / 3) - 1;
                for( int i = 0; i < int( item.size() / 3 ); ++i ) {
                    current.at( sz - i ).Sw_  = 1.0 - item.get< double >( (i * 3) + 0 );
                    current.at( sz - i ).Krow = item.get< double >( (i * 3) + 1 );
                }
            }
        }

        return res;
    }
};


inline bool consistent_families( const Deck& deck, const Phases& phases ) {
    const bool swof  = deck.hasKeyword( "SWOF" );
    const bool sgof  = deck.hasKeyword( "SGOF" );
    const bool slgof = deck.hasKeyword( "SLGOF" );

    const bool swfn  = deck.hasKeyword( "SWFN" );
    const bool sgfn  = deck.hasKeyword( "SGFN" );
    const bool sof2  = deck.hasKeyword( "SOF2" );
    const bool sof3  = deck.hasKeyword( "SOF3" );
    const bool sgwfn = deck.hasKeyword( "SGWFN" );

    const bool familyI  = swof || sgof || slgof;
    const bool familyII = swfn || sgfn || sof2 || sof3 || sgwfn;

    if( familyI && familyII ) return false;

    const bool wat = phases.active( Phase::WATER );
    const bool oil = phases.active( Phase::OIL );
    const bool gas = phases.active( Phase::GAS );

    if( swof  && !( wat && oil ) ) return false;
    if( sgof  && !( gas && oil ) ) return false;
    if( slgof && !( gas && oil ) ) return false;

    if( swfn && !wat ) return false;
    if( sgfn && !gas ) return false;
    if( sof2 && !oil && phases.size() != 2 ) return false;
    if( sof3 && !oil && phases.size() != 3 ) return false;
    if( sgwfn && !( gas && wat ) ) return false;

    return true;
}

saturation::saturation( const Deck& deck, int grid_size ) :
    og( grid_size ) {

    const auto& satnum = deck.getKeyword( "SATNUM" ).getIntData();
    const auto maxsat = deck.getKeyword( "TABDIMS" ).getRecord( 0 ).getItem( 0 ).get< int >( 0 );

    auto runspec = Runspec( deck );
    if( !consistent_families( deck, runspec.phases() ) )
        throw std::invalid_argument( "Inconsistent families" );

    decltype( this->wo ) watoil;
    if( deck.hasKeyword( "SWOF" ) ) {
        const auto& swof = deck.getKeyword( "SWOF" );
        assert( int( swof.size() ) == maxsat );
        watoil = satfuns::woSWOF( swof );
    } else {
        const auto& swfn = deck.getKeyword( "SWFN" );
        assert( int( swfn.size() ) == maxsat );
        watoil = satfuns::woSWFN( swfn, watoil );

        const auto& sof3 = deck.getKeyword( "SOF3" );
        assert( int( sof3.size() ) == maxsat );
        watoil = satfuns::woSOF3( sof3, watoil );
    }

    for( int i = 0; i < grid_size; ++i ) {
        this->wo.push_back( watoil.at( satnum.at( i ) ) );
    }

    if( !deck.hasKeyword( "ENDSCALE" ) ) {
        /* default all directions so that scaled_swat = unscaled_swat */
        for( size_t i = 0; i < this->wo.size(); ++i ) {
            const double Swco = this->wo[ i ].front().Sw;
            const double Swmax = this->wo[ i ].back().Sw;

            this->conwat.push_back( connate_water { Swco, Swco, Swco, Swco, Swco, Swco } );
            this->maxwat.push_back( maximum_water { Swmax, Swmax, Swmax, Swmax, Swmax, Swmax } );
        }

    }
    else {
        const auto& swl = deck.getKeyword( "SWL" ).getSIDoubleData();
        const auto& swu = deck.getKeyword( "SWU" ).getSIDoubleData();

        std::cout << "Have SWL, SWU " << std::endl;

        for( int i = 0; i < grid_size; ++i ) {
            const double Swco = swl.at( i );
            const double Swmax = swu.at( i );
            this->conwat.push_back( connate_water { Swco, Swco, Swco, Swco, Swco, Swco } );
            this->maxwat.push_back( maximum_water { Swmax, Swmax, Swmax, Swmax, Swmax, Swmax } );
        }
    }
}

double saturation::oil_relperm( size_t index, double swat ) const {
    const double swat1 = this->swat_scaled( index, swat );

    auto fn = std::find_if( this->wo.at( index ).begin(),
                            this->wo.at( index ).end(),
            [=]( const water_oil& x ) {
                constexpr const auto epsilon = 1e-5;
                return std::abs(swat1 - x.Sw) < epsilon;
            } );

    if( fn == this->wo.at( index ).end() )
        throw std::invalid_argument( "Saturation entry " + std::to_string( swat1 )
                                     + " does not exist in cell " + std::to_string( index ) );

    std::cout << "Unscaled swat: " << swat1
              << ". Scaled swat: " << swat
              << ". Found swat: " << fn->Sw << std::endl;

    return fn->Pcow;
}

double saturation::swat_scaled( size_t index, double Sw, direction dir ) const {
    double Swc, Swu;

    switch( dir ) {
        case direction::undir:
        case direction::Xp:
            Swc = this->conwat[ index ].Xp;
            Swu = this->maxwat[ index ].Xp;
            break;

        case direction::Xm:
            Swc = this->conwat[ index ].Xm;
            Swu = this->maxwat[ index ].Xm;
            break;

        case direction::Yp:
            Swc = this->conwat[ index ].Yp;
            Swu = this->maxwat[ index ].Yp;
            break;

        case direction::Ym:
            Swc = this->conwat[ index ].Ym;
            Swu = this->maxwat[ index ].Ym;
            break;

        case direction::Zp:
            Swc = this->conwat[ index ].Zp;
            Swu = this->maxwat[ index ].Zp;
            break;

        case direction::Zm:
            Swc = this->conwat[ index ].Zm;
            Swu = this->maxwat[ index ].Zm;
            break;
    }

    std::cout << "Swc, Swu: " << Swc << " " << Swu << std::endl;

    const auto& tab = this->wo.at( index );
    double Swco = tab.front().Sw;
    double Swcmax = tab.back().Sw;

    return Swco +  ( (Sw - Swc) * ( Swcmax - Swco )
                                / ( Swu - Swc ) );
}

bool saturation::operator==( const saturation& rhs ) const {

    for( size_t i = 0; i < this->wo.size(); ++i ) {
        if( !std::equal( this->wo.at( i ).begin(), this->wo.at( i ).end(),
                         rhs.wo.at( i ).begin() ) ) return false;
    }

    return true;
}

}
