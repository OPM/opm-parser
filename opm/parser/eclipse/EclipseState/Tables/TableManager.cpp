/*
  Copyright 2015 Statoil ASA.

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

#include <opm/parser/eclipse/Parser/ParserKeywords.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/Tables.hpp>
#include <opm/parser/eclipse/OpmLog/OpmLog.hpp>

namespace Opm {

    Tables::Tables( const Deck& deck ) {
        initTabdims( deck );
        initSimpleTables(deck, "SWOF", m_swofTables);
    }



    void Tables::initTabdims(const Deck& deck) {
        /*
          The default values for the various number of tables is
          embedded in the ParserKeyword("TABDIMS") instance; however
          the EclipseState object does not have a dependency on the
          Parser classes, have therefor decided not to add an explicit
          dependency here, and instead duplicated all the default
          values.
        */
        size_t ntsfun = 1;
        size_t ntpvt = 1;
        size_t nssfun = 1;
        size_t nppvt = 1;
        size_t ntfip = 1;
        size_t nrpvt = 1;

        if (deck.hasKeyword("TABDIMS")) {
            auto keyword = deck.getKeyword("TABDIMS");
            auto record = keyword->getRecord(0);
            ntsfun = record->getItem("NTSFUN")->getInt(0);
            ntpvt  = record->getItem("NTPVT")->getInt(0);
            nssfun = record->getItem("NSSFUN")->getInt(0);
            nppvt  = record->getItem("NPPVT")->getInt(0);
            ntfip  = record->getItem("NTFIP")->getInt(0);
            nrpvt  = record->getItem("NRPVT")->getInt(0);
        }
        m_tabdims = std::make_shared<Tabdims>(ntsfun , ntpvt , nssfun , nppvt , ntfip , nrpvt);
    }


    std::shared_ptr<const Tabdims> Tables::getTabdims() const {
        return m_tabdims;
    }


    const std::vector<SwofTable>& Tables::getSwofTables() const {
        return m_swofTables;
    }


    void Tables::complainAboutAmbiguousKeyword(const Deck& deck, const std::string& keywordName) const {
        OpmLog::addMessage(Log::MessageType::Error, "The " + keywordName + " keyword must be unique in the deck. Ignoring all!");
        auto keywords = deck.getKeywordList(keywordName);
        for (size_t i = 0; i < keywords.size(); ++i) {
            std::string msg = "Ambiguous keyword "+keywordName+" defined here";
            OpmLog::addMessage(Log::MessageType::Error , Log::fileMessage( keywords[i]->getFileName(), keywords[i]->getLineNumber(),msg));
        }
    }
}


