/*
  Copyright 2016 Statoil ASA.

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

#ifndef PARSER_UTILITY_ITERATOR_HPP
#define PARSER_UTILITY_ITERATOR_HPP

#include <iterator>
#include <memory>
#include <vector>

namespace Opm {

    /*
     * Currently, the iterator_base is templated on two parameters - this is a
     * necessary, but temporary, hack to enable different underlying storage *types*.
     *
     * DeckRecord stores its items as vector< shared_ptr >, which when const'd
     * as a template parameter is vector< const shared_ptr >, where we want
     * vector< shared_ptr< const > >. Additionally, shared_ptr< const > is
     * different from shared_ptr<> and you can't change between the two without
     * copying (i.e. modifying Deck*'s structure).
     *
     * To overcome this we always store as vector< shared_ptr > and make sure
     * the *interface* enforces the immutability. As soon as Deck* stores its
     * items as non-shared-ptr or applies const more conveniently, the second
     * template paramter can go away.
     */

    template< typename T, typename S = T >
    class iterator_base {
        private:
            using elem_type = typename std::remove_cv< std::shared_ptr< S > >::type;
            using base = typename std::vector< elem_type >::iterator;

        public:
            typedef typename base::difference_type difference_type;
            typedef T value_type;
            typedef T* pointer;
            typedef T& reference;
            typedef std::random_access_iterator_tag iterator_category;

            iterator_base() = default;
            iterator_base( typename std::vector< elem_type >::iterator );

            iterator_base& operator++();
            iterator_base operator++(int);
            iterator_base& operator--();
            iterator_base operator--(int);

            T& operator*();
            T* operator->();
            const T& operator*() const;
            const T* operator->() const;

            bool operator==( const iterator_base& rhs ) const;
            bool operator!=( const iterator_base& rhs ) const;

        private:
            typename std::vector< std::shared_ptr< S > >::iterator itr;
    };
}

#endif //PARSER_UTILITY_ITERATOR_HPP
