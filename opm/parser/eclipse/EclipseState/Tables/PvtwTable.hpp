#ifndef OPM_PVTWTABLE_HPP
#define OPM_PVTWTABLE_HPP

#include <vector>

namespace Opm {

class DeckKeyword;

class PvtwTable {
    public:
        struct record {
            double reference_pressure;
            double volume_factor;
            double compressibility;
            double viscosity;
            double viscosibility;
        };

        PvtwTable() = default;
        explicit PvtwTable( const DeckKeyword& );

        const record& operator[]( std::size_t region ) const;
        const record& at( std::size_t region ) const;
        bool empty() const;
        std::size_t size() const;

        using const_iterator = std::vector< record >::const_iterator;
        const_iterator begin() const;
        const_iterator end() const;

    private:
        std::vector< record > records;
};

}

#endif //OPM_PVTWTABLE_HPP
