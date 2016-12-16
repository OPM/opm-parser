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

#ifndef DECK_HPP
#define DECK_HPP

#include <map>
#include <string>
#include <vector>

#include <opm/parser/eclipse/Parser/MessageContainer.hpp>
#include <opm/parser/eclipse/Units/Dimension.hpp>
#include <opm/parser/eclipse/Units/UnitSystem.hpp>
#include <opm/parser/eclipse/Utility/Typetools.hpp>

#ifdef OPM_PARSER_DECK_API_WARNING
#ifndef OPM_PARSER_DECK_API
#pragma message "\n\n" \
"   ----------------------------------------------------------------------------------\n" \
"   The current compilation unit includes the header Deck.hpp. Outside of opm-parser  \n" \
"   you are encouraged to use the EclipseState API instead of the low level Deck API. \n" \
"   If use of the Deck API is absolutely necessary you can silence this warning with  \n" \
"   #define OPM_PARSER_DECK_API before including the Deck.hpp header.                 \n" \
"   ----------------------------------------------------------------------------------\n" \
""
#endif
#endif



namespace Opm {

class Parser;
class ParserKeyword;

class DeckItem {
    public:
        DeckItem() = default;
        DeckItem( const std::string& );

        DeckItem( const std::string&, int, size_t size_hint = 8 );
        DeckItem( const std::string&, double, size_t size_hint = 8 );
        DeckItem( const std::string&, std::string, size_t size_hint = 8 );

        const std::string& name() const;

        // return true if the default value was used for a given data point
        bool defaultApplied( size_t ) const;

        // Return true if the item has a value for the current index;
        // does not differentiate between default values from the
        // config and values which have been set in the deck.
        bool hasValue( size_t ) const;

        // if the number returned by this method is less than what is semantically
        // expected (e.g. size() is less than the number of cells in the grid for
        // keywords like e.g. SGL), then the remaining values are defaulted. The deck
        // creates the defaulted items if all their sizes are fully specified by the
        // keyword, though...
        size_t size() const;

        template< typename T > const T& get( size_t ) const;
        double getSIDouble( size_t ) const;
        std::string getTrimmedString( size_t ) const;

        template< typename T > const std::vector< T >& getData() const;
        const std::vector< double >& getSIDoubleData() const;

        void push_back( int );
        void push_back( double );
        void push_back( std::string );
        void push_back( int, size_t );
        void push_back( double, size_t );
        void push_back( std::string, size_t );
        void push_backDefault( int );
        void push_backDefault( double );
        void push_backDefault( std::string );
        // trying to access the data of a "dummy default item" will raise an exception
        void push_backDummyDefault();

        void push_backDimension( const Dimension& /* activeDimension */,
                const Dimension& /* defaultDimension */);

        type_tag getType() const;

    private:
        std::vector< double > dval;
        std::vector< int > ival;
        std::vector< std::string > sval;

        type_tag type = type_tag::unknown;

        std::string item_name;
        std::vector< bool > defaulted;
        std::vector< Dimension > dimensions;
        mutable std::vector< double > SIdata;

        template< typename T > std::vector< T >& value_ref();
        template< typename T > const std::vector< T >& value_ref() const;
        template< typename T > void push( T );
        template< typename T > void push( T, size_t );
        template< typename T > void push_default( T );
};

class DeckRecord {
    public:
        typedef std::vector< DeckItem >::const_iterator const_iterator;

        DeckRecord() = default;
        DeckRecord( std::vector< DeckItem >&& );

        size_t size() const;
        void addItem( DeckItem deckItem );

        DeckItem& getItem( size_t index );
        DeckItem& getItem( const std::string& name );
        DeckItem& getDataItem();

        const DeckItem& getItem( size_t index ) const;
        const DeckItem& getItem( const std::string& name ) const;
        const DeckItem& getDataItem() const;

        bool hasItem(const std::string& name) const;

        template <class Item>
            DeckItem& getItem() {
                return getItem( Item::itemName );
            }

        template <class Item>
            const DeckItem& getItem() const {
                return getItem( Item::itemName );
            }

        const_iterator begin() const;
        const_iterator end() const;

    private:
        std::vector< DeckItem > m_items;
};

class DeckKeyword {
    public:
        typedef std::vector< DeckRecord >::const_iterator const_iterator;

        DeckKeyword(const std::string& name );
        DeckKeyword(const std::string& name, bool knownKeyword);

        const std::string& name() const;
        void setLocation(const std::string& fileName, int lineNumber);
        const std::string& getFileName() const;
        int getLineNumber() const;

        size_t size() const;
        void addRecord(DeckRecord&& record);
        const DeckRecord& getRecord(size_t index) const;
        DeckRecord& getRecord(size_t index);
        const DeckRecord& getDataRecord() const;
        void setDataKeyword(bool isDataKeyword = true);
        bool isKnown() const;
        bool isDataKeyword() const;

        const std::vector<int>& getIntData() const;
        const std::vector<double>& getRawDoubleData() const;
        const std::vector<double>& getSIDoubleData() const;
        const std::vector<std::string>& getStringData() const;
        size_t getDataSize() const;

        template <class Keyword>
            bool isKeyword() const {
                if (Keyword::keywordName == m_keywordName)
                    return true;
                else
                    return false;
            }

        const_iterator begin() const;
        const_iterator end() const;

    private:
        std::string m_keywordName;
        std::string m_fileName;
        int m_lineNumber;

        std::vector< DeckRecord > m_recordList;
        bool m_knownKeyword;
        bool m_isDataKeyword;
};

/*
 * The Deck (container) class owns all memory given to it via .addX(), as
 * do all inner objects. This means that the Deck object itself must stay
 * alive as long as DeckItem (and friends) are needed, to avoid
 * use-after-free.
 */

class DeckView {
    public:
        typedef std::vector< DeckKeyword >::const_iterator const_iterator;

        bool hasKeyword( const DeckKeyword& keyword ) const;
        bool hasKeyword( const std::string& keyword ) const;
        template< class Keyword >
        bool hasKeyword() const {
            return hasKeyword( Keyword::keywordName );
        }

        const DeckKeyword& getKeyword( const std::string& keyword, size_t index ) const;
        const DeckKeyword& getKeyword( const std::string& keyword ) const;
        const DeckKeyword& getKeyword( size_t index ) const;
        DeckKeyword& getKeyword( size_t index );
        template< class Keyword >
        const DeckKeyword& getKeyword() const {
            return getKeyword( Keyword::keywordName );
        }
        template< class Keyword >
        const DeckKeyword& getKeyword( size_t index ) const {
            return getKeyword( Keyword::keywordName, index );
        }

        const std::vector< const DeckKeyword* > getKeywordList( const std::string& keyword ) const;
        template< class Keyword >
        const std::vector< const DeckKeyword* > getKeywordList() const {
            return getKeywordList( Keyword::keywordName );
        }

        size_t count(const std::string& keyword) const;
        size_t size() const;

        const_iterator begin() const;
        const_iterator end() const;

    protected:
        void add( const DeckKeyword*, const_iterator, const_iterator );

        const std::vector< size_t >& offsets( const std::string& ) const;

        DeckView( const_iterator first, const_iterator last );
        DeckView( std::pair< const_iterator, const_iterator > );

    private:
        const_iterator first;
        const_iterator last;
        std::map< std::string, std::vector< size_t > > keywordMap;

};

class Deck : private DeckView {
    public:
        using DeckView::const_iterator;
        using DeckView::hasKeyword;
        using DeckView::getKeyword;
        using DeckView::getKeywordList;
        using DeckView::count;
        using DeckView::size;
        using DeckView::begin;
        using DeckView::end;

        using iterator = std::vector< DeckKeyword >::iterator;

        Deck();
        Deck( std::initializer_list< DeckKeyword > );
        Deck( std::initializer_list< std::string > );
        void addKeyword( DeckKeyword&& keyword );
        void addKeyword( const DeckKeyword& keyword );

        DeckKeyword& getKeyword( size_t );
        MessageContainer& getMessageContainer() const;

        const UnitSystem& getDefaultUnitSystem() const;
        const UnitSystem& getActiveUnitSystem() const;
        UnitSystem& getActiveUnitSystem();
        UnitSystem& getDefaultUnitSystem();

        const std::string getDataFile() const;
        void setDataFile(const std::string& dataFile);

        iterator begin();
        iterator end();

    private:
        Deck( std::vector< DeckKeyword >&& );

        std::vector< DeckKeyword > keywordList;
        mutable MessageContainer m_messageContainer;
        UnitSystem defaultUnits;
        UnitSystem activeUnits;

        std::string m_dataFile;
};

class Section : public DeckView {
    public:
        using DeckView::const_iterator;

        Section( const Deck& deck, const std::string& startKeyword );
        const std::string& name() const;
        const UnitSystem& unitSystem() const;

        static bool hasRUNSPEC( const Deck& );
        static bool hasGRID( const Deck& );
        static bool hasEDIT( const Deck& );
        static bool hasPROPS( const Deck& );
        static bool hasREGIONS( const Deck& );
        static bool hasSOLUTION( const Deck& );
        static bool hasSUMMARY( const Deck& );
        static bool hasSCHEDULE( const Deck& );

        // returns whether the deck has all mandatory sections and if all sections are in
        // the right order
        static bool checkSectionTopology(const Deck& deck,
                                         const Parser&,
                                         bool ensureKeywordSectionAffiliation = false);

    private:
        std::string section_name;
        const UnitSystem& units;
};

class RUNSPECSection : public Section {
    public:
        using Section::const_iterator;
        RUNSPECSection(const Deck& deck) : Section(deck, "RUNSPEC") {}
};

class GRIDSection : public Section {
    public:
        using Section::const_iterator;
        GRIDSection(const Deck& deck) : Section(deck, "GRID") {}
};

class EDITSection : public Section {
    public:
        using Section::const_iterator;
        EDITSection(const Deck& deck) : Section(deck, "EDIT") {}
};

class PROPSSection : public Section {
    public:
        using Section::const_iterator;
        PROPSSection(const Deck& deck) : Section(deck, "PROPS") {}
};

class REGIONSSection : public Section {
    public:
        using Section::const_iterator;
        REGIONSSection(const Deck& deck) : Section(deck, "REGIONS") {}
};

class SOLUTIONSection : public Section {
    public:
        using Section::const_iterator;
        SOLUTIONSection(const Deck& deck) : Section(deck, "SOLUTION") {}
};

class SUMMARYSection : public Section {
    public:
        using Section::const_iterator;
        SUMMARYSection(const Deck& deck) : Section(deck, "SUMMARY") {}
};

class SCHEDULESection : public Section {
    public:
        using Section::const_iterator;
        SCHEDULESection(const Deck& deck) : Section(deck, "SCHEDULE") {}
};

}
#endif  /* DECK_HPP */

