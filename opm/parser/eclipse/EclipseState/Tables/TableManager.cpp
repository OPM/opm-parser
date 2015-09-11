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
#include <opm/parser/eclipse/EclipseState/Tables/TableManager.hpp>
#include <opm/parser/eclipse/OpmLog/OpmLog.hpp>

namespace Opm {

    TableManager::TableManager( const Deck& deck ) {
        initTabdims( deck );
        initSimpleTables(deck, "SWOF", m_swofTables);
        initSimpleTables(deck, "SGOF", m_sgofTables);
        initSimpleTables(deck, "SLGOF", m_slgofTables);
        initSimpleTables(deck, "SOF2", m_sof2Tables);
        initSimpleTables(deck, "SOF3", m_sof3Tables);
        initSimpleTables(deck, "PVDG", m_pvdgTables);
        initSimpleTables(deck, "PVDO", m_pvdoTables);
        initSimpleTables(deck, "SWFN", m_swfnTables);
        initSimpleTables(deck, "SGFN", m_sgfnTables);
        initSimpleTables(deck, "SSFN", m_ssfnTables);
        initSimpleTables(deck, "PVDS", m_pvdsTables);
        initSimpleTables(deck, "PLYADS", m_plyadsTables);
        initSimpleTables(deck, "PLYMAX", m_plymaxTables);
        initSimpleTables(deck, "PLYROCK", m_plyrockTables);
        initSimpleTables(deck, "PLYVISC", m_plyviscTables);
        initSimpleTables(deck, "PLYDHFLF", m_plydhflfTables);
        initSimpleTables(deck, "OILVISCT", m_oilvisctTables);
        initSimpleTables(deck, "WATVISCT", m_watvisctTables);
        initSimpleTables(deck, "ENKRVD", m_enkrvdTables);
        initSimpleTables(deck, "ENPTVD", m_enptvdTables);
        initSimpleTables(deck, "IMKRVD", m_imkrvdTables);
        initSimpleTables(deck, "IMPTVD", m_imptvdTables);
        initSimpleTables(deck, "RSVD", m_rsvdTables);
        initSimpleTables(deck, "RVVD", m_rvvdTables);

        initPlyshlogTables(deck, "PLYSHLOG", m_plyshlogTables);
        initRocktabTables(deck);
        initRTempTables(deck);
        initGasvisctTables(deck, "GASVISCT", m_gasvisctTables);
        initFullTables(deck, "PVTG", m_pvtgTables);
        initFullTables(deck, "PVTO", m_pvtoTables);

        initVFPProdTables(deck, m_vfpprodTables);
        initVFPInjTables(deck,  m_vfpinjTables);
    }


    void TableManager::initTabdims(const Deck& deck) {
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


    void TableManager::initRTempTables(const Deck& deck) {
        // the temperature vs depth table. the problem here is that
        // the TEMPVD (E300) and RTEMPVD (E300 + E100) keywords are
        // synonymous, but we want to provide only a single cannonical
        // API here, so we jump through some small hoops...
        if (deck.hasKeyword("TEMPVD") && deck.hasKeyword("RTEMPVD"))
            throw std::invalid_argument("The TEMPVD and RTEMPVD tables are mutually exclusive!");
        else if (deck.hasKeyword("TEMPVD"))
            initSimpleTables(deck, "TEMPVD", m_rtempvdTables);
        else if (deck.hasKeyword("RTEMPVD"))
            initSimpleTables(deck, "RTEMPVD", m_rtempvdTables);

    }


    void TableManager::initGasvisctTables(const Deck& deck,
                                    const std::string& keywordName,
                                    std::vector<GasvisctTable>& tableVector) {
        if (!deck.hasKeyword(keywordName))
            return; // the table is not featured by the deck...

        if (deck.numKeywords(keywordName) > 1) {
            complainAboutAmbiguousKeyword(deck, keywordName);
            return;
        }

        const auto& tableKeyword = deck.getKeyword(keywordName);
        for (size_t tableIdx = 0; tableIdx < tableKeyword->size(); ++tableIdx) {
            if (tableKeyword->getRecord(tableIdx)->getItem(0)->size() == 0) {
                // for simple tables, an empty record indicates that the previous table
                // should be copied...
                if (tableIdx == 0) {
                    std::string msg = "The first table for keyword " + keywordName + " must be explicitly defined! Ignoring keyword";
                    OpmLog::addMessage(Log::MessageType::Error , Log::fileMessage(tableKeyword->getFileName(),tableKeyword->getLineNumber(),msg));
                    return;
                }
                tableVector.push_back(tableVector.back());
                continue;
            }

            tableVector.push_back(GasvisctTable());
            tableVector[tableIdx].init(deck, tableKeyword->getRecord(tableIdx));
        }
    }

    void TableManager::initPlyshlogTables(const Deck& deck,
                                    const std::string& keywordName,
                                    std::vector<PlyshlogTable>& tableVector){

        if (!deck.hasKeyword(keywordName)) {
            return;
        }

        if (!deck.numKeywords(keywordName)) {
            complainAboutAmbiguousKeyword(deck, keywordName);
            return;
        }

        const auto& keyword = deck.getKeyword(keywordName);

        tableVector.push_back(PlyshlogTable());

        tableVector[0].init(keyword);

    }




    void TableManager::initRocktabTables(const Deck& deck) {
        if (!deck.hasKeyword("ROCKTAB"))
            return; // ROCKTAB is not featured by the deck...

        if (deck.numKeywords("ROCKTAB") > 1) {
            complainAboutAmbiguousKeyword(deck, "ROCKTAB");
            return;
        }

        const auto rocktabKeyword = deck.getKeyword("ROCKTAB");

        bool isDirectional = deck.hasKeyword("RKTRMDIR");
        bool useStressOption = false;
        if (deck.hasKeyword("ROCKOPTS")) {
            const auto rockoptsKeyword = deck.getKeyword("ROCKOPTS");
            useStressOption = (rockoptsKeyword->getRecord(0)->getItem("METHOD")->getTrimmedString(0) == "STRESS");
        }

        for (size_t tableIdx = 0; tableIdx < rocktabKeyword->size(); ++tableIdx) {
            if (rocktabKeyword->getRecord(tableIdx)->getItem(0)->size() == 0) {
                // for ROCKTAB tables, an empty record indicates that the previous table
                // should be copied...
                if (tableIdx == 0)
                    throw std::invalid_argument("The first table for keyword ROCKTAB"
                                                " must be explicitly defined!");
                m_rocktabTables[tableIdx] = m_rocktabTables[tableIdx - 1];
                continue;
            }

            m_rocktabTables.push_back(RocktabTable());
            m_rocktabTables[tableIdx].init(rocktabKeyword->getRecord( tableIdx ),
                                           isDirectional,
                                           useStressOption);
        }
    }



    void TableManager::initVFPProdTables(const Deck& deck,
                                          std::map<int, VFPProdTable>& tableMap) {
        if (!deck.hasKeyword(ParserKeywords::VFPPROD::keywordName)) {
            return;
        }

        int num_tables = deck.numKeywords(ParserKeywords::VFPPROD::keywordName);
        const auto& keywords = deck.getKeywordList<ParserKeywords::VFPPROD>();
        const auto unit_system = deck.getActiveUnitSystem();
        for (int i=0; i<num_tables; ++i) {
            const auto& keyword = keywords[i];

            VFPProdTable table;
            table.init(keyword, unit_system);

            //Check that the table in question has a unique ID
            int table_id = table.getTableNum();
            if (tableMap.find(table_id) == tableMap.end()) {
                tableMap.insert(std::make_pair(table_id, std::move(table)));
            }
            else {
                throw std::invalid_argument("Duplicate table numbers for VFPPROD found");
            }
        }
    }

    void TableManager::initVFPInjTables(const Deck& deck,
                                        std::map<int, VFPInjTable>& tableMap) {
        if (!deck.hasKeyword(ParserKeywords::VFPINJ::keywordName)) {
            return;
        }

        int num_tables = deck.numKeywords(ParserKeywords::VFPINJ::keywordName);
        const auto& keywords = deck.getKeywordList<ParserKeywords::VFPINJ>();
        const auto unit_system = deck.getActiveUnitSystem();
        for (int i=0; i<num_tables; ++i) {
            const auto& keyword = keywords[i];

            VFPInjTable table;
            table.init(keyword, unit_system);

            //Check that the table in question has a unique ID
            int table_id = table.getTableNum();
            if (tableMap.find(table_id) == tableMap.end()) {
                tableMap.insert(std::make_pair(table_id, std::move(table)));
            }
            else {
                throw std::invalid_argument("Duplicate table numbers for VFPINJ found");
            }
        }
    }

    std::shared_ptr<const Tabdims> TableManager::getTabdims() const {
        return m_tabdims;
    }


    const std::vector<SwofTable>& TableManager::getSwofTables() const {
        return m_swofTables;
    }


    const std::vector<SlgofTable>& TableManager::getSlgofTables() const {
        return m_slgofTables;
    }


    const std::vector<SgofTable>& TableManager::getSgofTables() const {
        return m_sgofTables;
    }

    const std::vector<Sof2Table>& TableManager::getSof2Tables() const {
        return m_sof2Tables;
    }

    const std::vector<Sof3Table>& TableManager::getSof3Tables() const {
        return m_sof3Tables;
    }

    const std::vector<PvdgTable>& TableManager::getPvdgTables() const {
        return m_pvdgTables;
    }

    const std::vector<PvdoTable>& TableManager::getPvdoTables() const {
        return m_pvdoTables;
    }

    const std::vector<SwfnTable>& TableManager::getSwfnTables() const {
        return m_swfnTables;
    }

    const std::vector<SgfnTable>& TableManager::getSgfnTables() const {
        return m_sgfnTables;
    }

    const std::vector<SsfnTable>& TableManager::getSsfnTables() const {
        return m_ssfnTables;
    }

    const std::vector<PvdsTable>& TableManager::getPvdsTables() const {
        return m_pvdsTables;
    }

    const std::vector<OilvisctTable>& TableManager::getOilvisctTables() const {
        return m_oilvisctTables;
    }

    const std::vector<WatvisctTable>& TableManager::getWatvisctTables() const {
        return m_watvisctTables;
    }

    const std::vector<GasvisctTable>& TableManager::getGasvisctTables() const {
        return m_gasvisctTables;
    }

    const std::vector<PlyadsTable>& TableManager::getPlyadsTables() const {
        return m_plyadsTables;
    }

    const std::vector<PlymaxTable>& TableManager::getPlymaxTables() const {
        return m_plymaxTables;
    }

    const std::vector<PlyrockTable>& TableManager::getPlyrockTables() const {
        return m_plyrockTables;
    }

    const std::vector<PlyviscTable>& TableManager::getPlyviscTables() const {
        return m_plyviscTables;
    }

    const std::vector<PlydhflfTable>& TableManager::getPlydhflfTables() const {
        return m_plydhflfTables;
    }

    const std::vector<PlyshlogTable>& TableManager::getPlyshlogTables() const {
        return m_plyshlogTables;
    }

    const std::vector<RocktabTable>& TableManager::getRocktabTables() const {
        return m_rocktabTables;
    }

    const std::vector<RtempvdTable>& TableManager::getRtempvdTables() const {
        return m_rtempvdTables;
    }


    const std::vector<EnkrvdTable>& TableManager::getEnkrvdTables() const {
        return m_enkrvdTables;
    }

    const std::vector<EnptvdTable>& TableManager::getEnptvdTables() const {
        return m_enptvdTables;
    }


    const std::vector<ImkrvdTable>& TableManager::getImkrvdTables() const {
        return m_imkrvdTables;
    }

    const std::vector<ImptvdTable>& TableManager::getImptvdTables() const {
        return m_imptvdTables;
    }


    const std::vector<RsvdTable>& TableManager::getRsvdTables() const {
        return m_rsvdTables;
    }

    const std::vector<RvvdTable>& TableManager::getRvvdTables() const {
        return m_rvvdTables;
    }

    const std::vector<PvtgTable>& TableManager::getPvtgTables() const {
        return m_pvtgTables;
    }


    const std::vector<PvtoTable>& TableManager::getPvtoTables() const {
        return m_pvtoTables;
    }


    const std::map<int, VFPProdTable>& TableManager::getVFPProdTables() const {
        return m_vfpprodTables;
    }

    const std::map<int, VFPInjTable>& TableManager::getVFPInjTables() const {
        return m_vfpinjTables;
    }


    void TableManager::complainAboutAmbiguousKeyword(const Deck& deck, const std::string& keywordName) const {
        OpmLog::addMessage(Log::MessageType::Error, "The " + keywordName + " keyword must be unique in the deck. Ignoring all!");
        auto keywords = deck.getKeywordList(keywordName);
        for (size_t i = 0; i < keywords.size(); ++i) {
            std::string msg = "Ambiguous keyword "+keywordName+" defined here";
            OpmLog::addMessage(Log::MessageType::Error , Log::fileMessage( keywords[i]->getFileName(), keywords[i]->getLineNumber(),msg));
        }
    }
}

