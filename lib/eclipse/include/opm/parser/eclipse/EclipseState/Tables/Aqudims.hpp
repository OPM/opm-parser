/*
  Copyright (C) 2015 Statoil ASA

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

#ifndef AQUDIMS_HPP
#define AQUDIMS_HPP

/*
  The Aqudims class is a small utility class designed to hold on to
  the values from the AQUDIMS keyword.
*/

#include <opm/parser/eclipse/Parser/ParserKeywords/A.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
namespace Opm {
    class Aqudims {
    public:

        Aqudims() :
            m_mxnaqn( ParserKeywords::AQUDIMS::MXNAQN::defaultValue ),
            m_mxnaqc( ParserKeywords::AQUDIMS::MXNAQC::defaultValue ),
            m_niftbl( ParserKeywords::AQUDIMS::NIFTBL::defaultValue ),
            m_nriftb( ParserKeywords::AQUDIMS::NRIFTB::defaultValue ),
            m_nanaqu( ParserKeywords::AQUDIMS::NANAQU::defaultValue ),
            m_ncamax( ParserKeywords::AQUDIMS::NCAMAX::defaultValue ),
            m_mxnali( ParserKeywords::AQUDIMS::MXNALI::defaultValue ),
            m_mxaaql( ParserKeywords::AQUDIMS::MXAAQL::defaultValue )
            
        { }

/*
        Aqudims( size_t mxnaqn , size_t mxnaqc , size_t niftbl , size_t nriftb , size_t nanaqu, size_t ncamax , size_t mxnali , size_t mxaaql) :
            m_mxnaqn( mxnaqn ),
            m_mxnaqc( mxnaqc ),
            m_niftbl( niftbl ),
            m_nriftb( nriftb ),
            m_nanaqu( nanaqu ),
            m_ncamax( ncamax ),
            m_mxnali( mxnali ),
            m_mxaaql( mxaaql )
    
        { }
*/

        explicit Aqudims(const Deck& deck) :
            Aqudims()
        {
            if (deck.hasKeyword("AQUDIMS")) {
                const auto& record = deck.getKeyword( "AQUDIMS" , 0 ).getRecord( 0 );
                m_mxnaqn  = record.getItem("MXNAQN").get<int>(0);
                m_mxnaqc  = record.getItem("MXNAQC").get<int>(0);
                m_niftbl  = record.getItem("NIFTBL").get<int>(0);
                m_nriftb  = record.getItem("NRIFTB").get<int>(0);
                m_nanaqu  = record.getItem("NANAQU").get<int>(0);
                m_ncamax  = record.getItem("NCAMAX").get<int>(0);
                m_mxnali  = record.getItem("MXNALI").get<int>(0);
                m_mxaaql  = record.getItem("MXAAQL").get<int>(0);
            }
        }

        size_t getNumAqunum() const
        {
            return m_mxnaqn;
        }

        size_t getNumConnectionNumericalAquifer() const
        {
            return m_mxnaqc;
        }

        size_t getNumInfluenceTablesCT() const
        {
            return m_niftbl;
        }

        size_t getNumRowsInfluenceTable() const
        {
            return m_nriftb;
        }

        size_t getNumAnalyticAquifers() const
        {
            return m_nanaqu;
        }
        
        size_t getNumRowsAquancon() const
        {
            return m_ncamax;
        }

        size_t getNumAquiferLists() const
        {
            return m_mxnali;
        }

        size_t getNumAnalyticAquifersSingleList() const
        {
            return m_mxaaql;
        }        
        
    private:
        size_t m_mxnaqn , m_mxnaqc , m_niftbl , m_nriftb , m_nanaqu , m_ncamax , m_mxnali , m_mxaaql;

    };
}


#endif
