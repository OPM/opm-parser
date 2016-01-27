#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Utility/Iterator.hpp>
#include <opm/parser/eclipse/Utility/Iterator.cpp>

namespace Opm {
    template class iterator_base< DeckItem >;
    template class iterator_base< const DeckItem, DeckItem >;
}
