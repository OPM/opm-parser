/*
  Copyright 2013 Statoil ASA.

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
#ifndef PARSER_KEYWORD_H
#define PARSER_KEYWORD_H

#include <string>
#include <iostream>
#include <memory>
#include <set>

#ifdef HAVE_REGEX
#include <regex>
#else
#include <boost/regex.hpp>
#endif

#include <opm/json/JsonObject.hpp>

#include <opm/parser/eclipse/Parser/ParserRecord.hpp>
#include <opm/parser/eclipse/Parser/ParserDoubleItem.hpp>
#include <opm/parser/eclipse/Parser/ParserFloatItem.hpp>
#include <opm/parser/eclipse/Parser/ParserEnums.hpp>
#include <opm/parser/eclipse/RawDeck/RawKeyword.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>



namespace Opm {
    class ParserKeyword;
    typedef std::shared_ptr<ParserKeyword> ParserKeywordPtr;
    typedef std::shared_ptr<const ParserKeyword> ParserKeywordConstPtr;

    class ParserKeyword {
        ParserKeyword(ParserLogPtr parserLog,
                      const std::string& name ,
                      const std::string& sizeKeyword ,
                      const std::string& sizeItem,
                      ParserKeywordActionEnum action = INTERNALIZE ,
                      bool isTableCollection = false);
        ParserKeyword(ParserLogPtr parserLog,
                      const std::string& name ,
                      ParserKeywordSizeEnum sizeType = SLASH_TERMINATED ,
                      ParserKeywordActionEnum action = INTERNALIZE);
        ParserKeyword(ParserLogPtr parserLog,
                      const std::string& name ,
                      size_t fixedKeywordSize,
                      ParserKeywordActionEnum action = INTERNALIZE);
        ParserKeyword(ParserLogPtr parserLog,
                      const Json::JsonObject& jsonConfig);

    public:
        typedef std::set<std::string> DeckNameSet;
        typedef std::set<std::string> SectionNameSet;

        /*!
         * \brief Factory method to create a keyword where the number
         *        of items per record is defined at compile time.
         *
         * This are for example well specifcation keywords like WCONPROD...
         */
        static ParserKeywordPtr createFixedSized(ParserLogPtr parserLog,
                                                 const std::string& name,
                                                 size_t fixedKeywordSize,
                                                 ParserKeywordActionEnum action = INTERNALIZE);

        /*!
         * \brief Factory method to create a keyword with an per-se
         *        unspecified number of items per record.
         *
         * This are for example grid properties like PERM?...
         */
        static ParserKeywordPtr createDynamicSized(ParserLogPtr parserLog,
                                                   const std::string& name,
                                                   ParserKeywordSizeEnum sizeType = SLASH_TERMINATED ,
                                                   ParserKeywordActionEnum action = INTERNALIZE);

        /*!
         * \brief Factory method to create a keyword which has a
         *        dynamic number of items per record.
         *
         * But with the number of items are specified via an item of a
         * different keyword, e.g. for tables.
         */
        static ParserKeywordPtr createTable(ParserLogPtr parserLog,
                                            const std::string& name,
                                            const std::string& sizeKeyword,
                                            const std::string& sizeItem,
                                            ParserKeywordActionEnum action = INTERNALIZE,
                                            bool isTableCollection = false);

        /*!
         * \brief Factory method to create a keyword from a JSON
         *        configuration object.
         */
        static ParserKeywordPtr createFromJson(ParserLogPtr parserLog, const Json::JsonObject& jsonConfig);

        static std::string getDeckName(const std::string& rawString);
        static bool validInternalName(const std::string& name);
        static bool validDeckName(const std::string& name, bool acceptLowerCaseLetters = false);
        bool hasMatchRegex() const;
        void setMatchRegex(const std::string& deckNameRegexp);
        bool matches(const std::string& deckKeywordName) const;
        bool hasDimension() const;
        ParserRecordPtr getRecord() const;
        const std::string& getName() const;
        ParserKeywordActionEnum getAction() const;
        size_t getFixedSize() const;
        bool hasFixedSize() const;
        bool isTableCollection() const;
        std::string getDescription() const;
        void setDescription(const std::string &description);

        size_t numItems() const;

        bool hasMultipleDeckNames() const;
        void clearDeckNames();
        void addDeckName( const std::string& deckName );
        DeckNameSet::const_iterator deckNamesBegin() const;
        DeckNameSet::const_iterator deckNamesEnd() const;

        void clearValidSectionNames();
        void addValidSectionName(const std::string& sectionName);
        bool isValidSection(const std::string& sectionName) const;
        SectionNameSet::const_iterator validSectionNamesBegin() const;
        SectionNameSet::const_iterator validSectionNamesEnd() const;

        DeckKeywordPtr parse(RawKeywordConstPtr rawKeyword) const;
        enum ParserKeywordSizeEnum getSizeType() const;
        const std::pair<std::string,std::string>& getSizeDefinitionPair() const;
        void addItem( ParserItemConstPtr item );
        void addDataItem( ParserItemConstPtr item );
        bool isDataKeyword() const;
        bool equal(const ParserKeyword& other) const;
        void inlineNew(std::ostream& os, const std::string& parserLogString, const std::string& lhs, const std::string& indent) const;
        void applyUnitsToDeck(std::shared_ptr<const Deck> deck , std::shared_ptr<DeckKeyword> deckKeyword) const;
    private:
        ParserLogPtr m_parserLog;
        std::pair<std::string,std::string> m_sizeDefinitionPair;
        std::string m_name;
        DeckNameSet m_deckNames;
        DeckNameSet m_validSectionNames;
        std::string m_matchRegexString;
#ifdef HAVE_REGEX
        std::regex m_matchRegex;
#else
        boost::regex m_matchRegex;
#endif
        ParserRecordPtr m_record;
        enum ParserKeywordSizeEnum m_keywordSizeType;
        size_t m_fixedSize;
        bool m_isDataKeyword;
        bool m_isTableCollection;
        ParserKeywordActionEnum m_action;
        std::string m_Description;

        static bool validNameStart(const std::string& name, bool acceptLowerCaseLetters);
        void initDeckNames( const Json::JsonObject& jsonConfig );
        void initSectionNames( const Json::JsonObject& jsonConfig );
        void initMatchRegex( const Json::JsonObject& jsonObject );
        void initData( const Json::JsonObject& jsonConfig );
        void initSize( const Json::JsonObject& jsonConfig );
        void initSizeKeyword( const std::string& sizeKeyword, const std::string& sizeItem);
        void initSizeKeyword(const Json::JsonObject& sizeObject);
        void commonInit(ParserLogPtr parserLog, const std::string& name, ParserKeywordSizeEnum sizeType, ParserKeywordActionEnum action);
        void addItems( const Json::JsonObject& jsonConfig);
        void initDoubleItemDimension( ParserDoubleItemPtr item, const Json::JsonObject itemConfig);
        void initFloatItemDimension( ParserFloatItemPtr item, const Json::JsonObject itemConfig);
    };
}

#endif
