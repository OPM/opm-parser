#include <opm/parser/eclipse/Utility/Iterator.hpp>

namespace Opm {

    template< typename T, typename S >
    iterator_base< T, S >::iterator_base( typename std::vector< elem_type >::iterator itr ) :
        itr( itr )
    {}

    template< typename T, typename S >
    iterator_base< T, S >& iterator_base< T, S >::operator++() {
        ++this->itr;
        return *this;
    }

    template< typename T, typename S >
    iterator_base< T, S > iterator_base< T, S >::operator++( int ) {
        auto orig = *this;
        ++*this;
        return orig;
    }

    template< typename T, typename S >
    iterator_base< T, S >& iterator_base< T, S >::operator--() {
        --this->itr;
        return *this;
    }

    template< typename T, typename S >
    iterator_base< T, S > iterator_base< T, S >::operator--( int ) {
        auto orig = *this;
        --*this;
        return orig;
    }

    template< typename T, typename S >
    T& iterator_base< T, S >::operator*() {
        return **this->itr;
    }

    template< typename T, typename S >
    T* iterator_base< T, S >::operator->() {
        return this->itr->get();
    }

    template< typename T, typename S >
    const T& iterator_base< T, S >::operator*() const {
        return **this->itr;
    }

    template< typename T, typename S >
    const T* iterator_base< T, S >::operator->() const {
        return this->itr->get();
    }

    template< typename T, typename S >
    bool iterator_base< T, S >::operator==( const iterator_base< T, S >& rhs ) const {
        return !( *this != rhs );
    }

    template< typename T, typename S >
    bool iterator_base< T, S >::operator!=( const iterator_base< T, S >& rhs ) const {
        return this->itr != rhs.itr;
    }

    template class iterator_base< DeckItem >;
    template class iterator_base< const DeckItem, DeckItem >;
}
