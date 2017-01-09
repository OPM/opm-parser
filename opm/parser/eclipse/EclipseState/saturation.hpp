#ifndef OPM_SATURATION_HPP
#define OPM_SATURATION_HPP

#include <iosfwd>
#include <vector>

namespace Opm {

class Deck;

class saturation {
    public:
        saturation() = default;
        saturation( const Deck&, int );

        enum class direction {
            undir,
            Xp,
            Xm,
            Yp,
            Ym,
            Zp,
            Zm,
        };

        bool operator!=( const saturation& rhs ) const { return !( *this == rhs ); }
        bool operator==( const saturation& ) const;

        double oil_relperm( size_t cell_index, double swat ) const;

    private:
        struct oil_gas {
            double So;

            double Krg;
            double Krog;
            double Pcog;

            bool operator!=( const oil_gas& rhs ) const { return !( *this == rhs ); }
            bool operator==( const oil_gas& rhs ) const {
                return this->So   == rhs.So
                    && this->Krg  == rhs.Krg
                    && this->Krog == rhs.Krog
                    && this->Pcog == rhs.Pcog;
            }
        };

        struct water_oil {
            double Sw;
            double Krw;
            double Pcow;

            double Sw_;
            double Krow;

            bool operator!=( const water_oil& rhs ) const { return !( *this == rhs ); }
            bool operator==( const water_oil& rhs ) const {
                constexpr const auto epsilon = 1e-10;

                return this->Sw   - rhs.Sw   < epsilon
                    && this->Sw_  - rhs.Sw_  < epsilon
                    && this->Krw  - rhs.Krw  < epsilon
                    && this->Krow - rhs.Krow < epsilon
                    && this->Pcow - rhs.Pcow < epsilon;
            }
        };

        double swat_scaled( size_t index, double swat, direction dir = direction::undir ) const;

        struct connate_water {
            double Xp;
            double Xm;
            double Yp;
            double Ym;
            double Zp;
            double Zm;
        };

        struct maximum_water {
            double Xp;
            double Xm;
            double Yp;
            double Ym;
            double Zp;
            double Zm;
        };

        std::vector< std::vector< water_oil > > wo;
        std::vector< std::vector< oil_gas > > og;
        std::vector< connate_water > conwat;
        std::vector< maximum_water > maxwat;

        friend struct satfuns;
};

}

#endif //OPM_SATURATION_HPP
