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

#include <algorithm>
#include <vector>
#include <unordered_set>

#include <boost/algorithm/string.hpp>

#include <opm/parser/eclipse/Parser/Deck.hpp>
#include <opm/parser/eclipse/Units/UnitSystem.hpp>

namespace Opm {

template< typename T >
std::vector< T >& DeckItem::value_ref() {
    return const_cast< std::vector< T >& >(
            const_cast< const DeckItem& >( *this ).value_ref< T >()
         );
}

template<>
const std::vector< int >& DeckItem::value_ref< int >() const {
    if( this->type != get_type< int >() )
        throw std::invalid_argument( "Item of wrong type." );

    return this->ival;
}

template<>
const std::vector< double >& DeckItem::value_ref< double >() const {
    if( this->type != get_type< double >() )
        throw std::invalid_argument( "Item of wrong type." );

    return this->dval;
}

template<>
const std::vector< std::string >& DeckItem::value_ref< std::string >() const {
    if( this->type != get_type< std::string >() )
        throw std::invalid_argument( "Item of wrong type." );

    return this->sval;
}

DeckItem::DeckItem( const std::string& nm ) : item_name( nm ) {}

DeckItem::DeckItem( const std::string& nm, int, size_t hint ) :
    type( get_type< int >() ),
    item_name( nm )
{
    this->ival.reserve( hint );
    this->defaulted.reserve( hint );
}

DeckItem::DeckItem( const std::string& nm, double, size_t hint ) :
    type( get_type< double >() ),
    item_name( nm )
{
    this->dval.reserve( hint );
    this->defaulted.reserve( hint );
}

DeckItem::DeckItem( const std::string& nm, std::string, size_t hint ) :
    type( get_type< std::string >() ),
    item_name( nm )
{
    this->sval.reserve( hint );
    this->defaulted.reserve( hint );
}

const std::string& DeckItem::name() const {
    return this->item_name;
}

bool DeckItem::defaultApplied( size_t index ) const {
    return this->defaulted.at( index );
}

bool DeckItem::hasValue( size_t index ) const {
    switch( this->type ) {
        case type_tag::integer: return this->ival.size() > index;
        case type_tag::fdouble: return this->dval.size() > index;
        case type_tag::string:  return this->sval.size() > index;
        default: throw std::logic_error( "Type not set." );
    }
}

size_t DeckItem::size() const {
    switch( this->type ) {
        case type_tag::integer: return this->ival.size();
        case type_tag::fdouble: return this->dval.size();
        case type_tag::string:  return this->sval.size();
        default: throw std::logic_error( "Type not set." );
    }
}

template< typename T >
const T& DeckItem::get( size_t index ) const {
    return this->value_ref< T >().at( index );
}

template< typename T >
const std::vector< T >& DeckItem::getData() const {
    return this->value_ref< T >();
}

template< typename T >
void DeckItem::push( T x ) {
    auto& val = this->value_ref< T >();

    val.push_back( std::move( x ) );
    this->defaulted.push_back( false );
}

void DeckItem::push_back( int x ) {
    this->push( x );
}

void DeckItem::push_back( double x ) {
    this->push( x );
}

void DeckItem::push_back( std::string x ) {
    this->push( std::move( x ) );
}

template< typename T >
void DeckItem::push( T x, size_t n ) {
    auto& val = this->value_ref< T >();

    val.insert( val.end(), n, x );
    this->defaulted.insert( this->defaulted.end(), n, false );
}

void DeckItem::push_back( int x, size_t n ) {
    this->push( x, n );
}

void DeckItem::push_back( double x, size_t n ) {
    this->push( x, n );
}

void DeckItem::push_back( std::string x, size_t n ) {
    this->push( std::move( x ), n );
}

template< typename T >
void DeckItem::push_default( T x ) {
    auto& val = this->value_ref< T >();
    if( this->defaulted.size() != val.size() )
        throw std::logic_error("To add a value to an item, "
                "no 'pseudo defaults' can be added before");

    val.push_back( std::move( x ) );
    this->defaulted.push_back( true );
}

void DeckItem::push_backDefault( int x ) {
    this->push_default( x );
}

void DeckItem::push_backDefault( double x ) {
    this->push_default( x );
}

void DeckItem::push_backDefault( std::string x ) {
    this->push_default( std::move( x ) );
}


void DeckItem::push_backDummyDefault() {
    if( !this->defaulted.empty() )
        throw std::logic_error("Pseudo defaults can only be specified for empty items");

    this->defaulted.push_back( true );
}

std::string DeckItem::getTrimmedString( size_t index ) const {
    return boost::algorithm::trim_copy(
               this->value_ref< std::string >().at( index )
           );
}

double DeckItem::getSIDouble( size_t index ) const {
    return this->getSIDoubleData().at( index );
}

const std::vector< double >& DeckItem::getSIDoubleData() const {
    const auto& raw = this->value_ref< double >();
    // we already converted this item to SI?
    if( !this->SIdata.empty() ) return this->SIdata;

    if( this->dimensions.empty() )
        throw std::invalid_argument("No dimension has been set for item'"
                                    + this->name()
                                    + "'; can not ask for SI data");

    /*
     * This is an unobservable state change - SIData is lazily converted to
     * SI units, so externally the object still behaves as const
     */
    const auto dim_size = dimensions.size();
    const auto sz = raw.size();
    this->SIdata.resize( sz );

    for( size_t index = 0; index < sz; index++ ) {
        const auto dimIndex = index % dim_size;
        this->SIdata[ index ] = this->dimensions[ dimIndex ]
                                .convertRawToSi( raw[ index ] );
    }

    return this->SIdata;
}

void DeckItem::push_backDimension( const Dimension& active,
                                    const Dimension& def ) {
    const auto& ds = this->value_ref< double >();
    const bool dim_inactive = ds.empty()
                            || this->defaultApplied( ds.size() - 1 );

    this->dimensions.push_back( dim_inactive ? def : active );
}

type_tag DeckItem::getType() const {
    return this->type;
}

/*
 * Explicit template instantiations. These must be manually maintained and
 * updated with changes in DeckItem so that code is emitted.
 */

template const int& DeckItem::get< int >( size_t ) const;
template const double& DeckItem::get< double >( size_t ) const;
template const std::string& DeckItem::get< std::string >( size_t ) const;

template const std::vector< int >& DeckItem::getData< int >() const;
template const std::vector< double >& DeckItem::getData< double >() const;
template const std::vector< std::string >& DeckItem::getData< std::string >() const;

DeckRecord::DeckRecord( std::vector< DeckItem >&& items ) :
    m_items( std::move( items ) ) {

    std::unordered_set< std::string > names;
    for( const auto& item : this->m_items )
        names.insert( item.name() );

    if( names.size() == this->m_items.size() )
        return;

    names.clear();
    std::string msg = "Duplicate item names in DeckRecord:";
    for( const auto& item : this->m_items ) {
        if( names.count( item.name() ) != 0 )
            msg += std::string( " " ) += item.name();

        names.insert( item.name() );
    }

    throw std::invalid_argument( msg );
}

size_t DeckRecord::size() const {
    return m_items.size();
}

void DeckRecord::addItem( DeckItem deckItem ) {
    if( this->hasItem( deckItem.name() ) )
        throw std::invalid_argument(
                "Item with name: "
                + deckItem.name()
                + " already exists in DeckRecord");

    m_items.push_back( std::move( deckItem ) );
}

DeckItem& DeckRecord::getItem( size_t index ) {
    return this->m_items.at( index );
}

DeckItem& DeckRecord::getItem(const std::string& name) {
    const auto eq = [&name]( const DeckItem& e ) {
        return e.name() == name;
    };

    auto item = std::find_if( m_items.begin(), m_items.end(), eq );

    if( item == m_items.end() )
        throw std::invalid_argument("Item: " + name + " does not exist.");

    return *item;
}

DeckItem& DeckRecord::getDataItem() {
    if (m_items.size() == 1)
        return getItem(0);
    else
        throw std::range_error("Not a data keyword ?");
}

const DeckItem& DeckRecord::getItem( size_t index ) const {
    return this->m_items.at( index );
}

const DeckItem& DeckRecord::getItem(const std::string& name) const {
    const auto eq = [&name]( const DeckItem& e ) {
        return e.name() == name;
    };

    auto item = std::find_if( this->begin(), this->end(), eq );

    if( item == m_items.end() )
        throw std::invalid_argument("Item: " + name + " does not exist.");

    return *item;
}

const DeckItem& DeckRecord::getDataItem() const {
    if (m_items.size() == 1)
        return getItem(0);
    else
        throw std::range_error("Not a data keyword ?");
}

bool DeckRecord::hasItem(const std::string& name) const {
    const auto eq = [&name]( const DeckItem& e ) {
        return e.name() == name;
    };

    return std::any_of( this->begin(), this->end(), eq );
}

DeckRecord::const_iterator DeckRecord::begin() const {
    return this->m_items.begin();
}

DeckRecord::const_iterator DeckRecord::end() const {
    return this->m_items.end();
}

DeckKeyword::DeckKeyword(const std::string& keywordName) {
    m_knownKeyword = true;
    m_keywordName = keywordName;
    m_isDataKeyword = false;
    m_fileName = "";
    m_lineNumber = -1;
}

DeckKeyword::DeckKeyword(const std::string& keywordName, bool knownKeyword) {
    m_knownKeyword = knownKeyword;
    m_keywordName = keywordName;
    m_isDataKeyword = false;
    m_fileName = "";
    m_lineNumber = -1;
}

void DeckKeyword::setLocation(const std::string& fileName, int lineNumber) {
    m_fileName = fileName;
    m_lineNumber = lineNumber;
}

const std::string& DeckKeyword::getFileName() const {
    return m_fileName;
}

int DeckKeyword::getLineNumber() const {
    return m_lineNumber;
}

void DeckKeyword::setDataKeyword(bool isDataKeyword_) {
    m_isDataKeyword = isDataKeyword_;
}

bool DeckKeyword::isDataKeyword() const {
    return m_isDataKeyword;
}


const std::string& DeckKeyword::name() const {
    return m_keywordName;
}

size_t DeckKeyword::size() const {
    return m_recordList.size();
}

bool DeckKeyword::isKnown() const {
    return m_knownKeyword;
}

void DeckKeyword::addRecord(DeckRecord&& record) {
    this->m_recordList.push_back( std::move( record ) );
}

DeckKeyword::const_iterator DeckKeyword::begin() const {
    return m_recordList.begin();
}

DeckKeyword::const_iterator DeckKeyword::end() const {
    return m_recordList.end();
}

const DeckRecord& DeckKeyword::getRecord(size_t index) const {
    return this->m_recordList.at( index );
}

DeckRecord& DeckKeyword::getRecord(size_t index) {
    return this->m_recordList.at( index );
}

const DeckRecord& DeckKeyword::getDataRecord() const {
    if (m_recordList.size() == 1)
        return getRecord(0);
    else
        throw std::range_error("Not a data keyword \"" + name() + "\"?");
}


size_t DeckKeyword::getDataSize() const {
    return this->getDataRecord().getDataItem().size();
}


const std::vector<int>& DeckKeyword::getIntData() const {
    return this->getDataRecord().getDataItem().getData< int >();
}


const std::vector<std::string>& DeckKeyword::getStringData() const {
    return this->getDataRecord().getDataItem().getData< std::string >();
}


const std::vector<double>& DeckKeyword::getRawDoubleData() const {
    return this->getDataRecord().getDataItem().getData< double >();
}

const std::vector<double>& DeckKeyword::getSIDoubleData() const {
    return this->getDataRecord().getDataItem().getSIDoubleData();
}


bool DeckView::hasKeyword( const DeckKeyword& keyword ) const {
    auto key = this->keywordMap.find( keyword.name() );

    if( key == this->keywordMap.end() ) return false;

    for( auto index : key->second )
        if( &this->getKeyword( index ) == &keyword ) return true;

    return false;
}

bool DeckView::hasKeyword( const std::string& keyword ) const {
    return this->keywordMap.find( keyword ) != this->keywordMap.end();
}

const DeckKeyword& DeckView::getKeyword( const std::string& keyword, size_t index ) const {
    if( !this->hasKeyword( keyword ) )
        throw std::invalid_argument("Keyword " + keyword + " not in deck.");

    return this->getKeyword( this->offsets( keyword ).at( index ) );
}

const DeckKeyword& DeckView::getKeyword( const std::string& keyword ) const {
    if( !this->hasKeyword( keyword ) )
        throw std::invalid_argument("Keyword " + keyword + " not in deck.");

    return this->getKeyword( this->offsets( keyword ).back() );
}

const DeckKeyword& DeckView::getKeyword( size_t index ) const {
    if( index >= this->size() )
        throw std::out_of_range("Keyword index " + std::to_string( index ) + " is out of range.");

    return *( this->begin() + index );
}

size_t DeckView::count( const std::string& keyword ) const {
    if( !this->hasKeyword( keyword ) ) return 0;

    return this->offsets( keyword ).size();
}

const std::vector< const DeckKeyword* > DeckView::getKeywordList( const std::string& keyword ) const {
    if( !hasKeyword( keyword ) ) return {};

    const auto& indices = this->offsets( keyword );

    std::vector< const DeckKeyword* > ret;
    ret.reserve( indices.size() );

    for( size_t i : indices )
        ret.push_back( &this->getKeyword( i ) );

    return ret;
}

size_t DeckView::size() const {
    return std::distance( this->begin(), this->end() );
}

DeckView::const_iterator DeckView::begin() const {
    return this->first;
}

DeckView::const_iterator DeckView::end() const {
    return this->last;
}

void DeckView::add( const DeckKeyword* kw, const_iterator f, const_iterator l ) {
    this->keywordMap[ kw->name() ].push_back( std::distance( f, l ) - 1 );
    this->first = f;
    this->last = l;
}

static const std::vector< size_t > empty_indices = {};
const std::vector< size_t >& DeckView::offsets( const std::string& keyword ) const {
    if( !hasKeyword( keyword ) ) return empty_indices;

    return this->keywordMap.find( keyword )->second;
}

DeckView::DeckView( const_iterator first_arg, const_iterator last_arg ) :
    first( first_arg ), last( last_arg )
{
    size_t index = 0;
    for( const auto& kw : *this )
        this->keywordMap[ kw.name() ].push_back( index++ );
}

DeckView::DeckView( std::pair< const_iterator, const_iterator > limits ) :
    DeckView( limits.first, limits.second )
{}

Deck::Deck() : Deck( std::vector< DeckKeyword >() ) {}

Deck::Deck( std::vector< DeckKeyword >&& x ) :
    DeckView( x.begin(), x.end() ),
    keywordList( std::move( x ) ),
    defaultUnits( UnitSystem::newMETRIC() ),
    activeUnits( UnitSystem::newMETRIC() ),
    m_dataFile("")
{
    /*
        * If multiple unit systems are requested, metric is preferred over
        * lab, and field over metric, for as long as we have no easy way of
        * figuring out which was requested last.
        */
    if( this->hasKeyword( "LAB" ) )
        this->activeUnits = UnitSystem::newLAB();
    if( this->hasKeyword( "FIELD" ) )
        this->activeUnits = UnitSystem::newFIELD();
    if( this->hasKeyword( "METRIC" ) )
        this->activeUnits = UnitSystem::newMETRIC();
}

Deck::Deck( std::initializer_list< DeckKeyword > ilist ) :
    Deck( std::vector< DeckKeyword >( ilist ) )
{}

Deck::Deck( std::initializer_list< std::string > ilist ) :
    Deck( std::vector< DeckKeyword >( ilist.begin(), ilist.end() ) )
{}

void Deck::addKeyword( DeckKeyword&& keyword ) {
    this->keywordList.push_back( std::move( keyword ) );

    auto fst = this->keywordList.begin();
    auto lst = this->keywordList.end();

    this->add( &this->keywordList.back(), fst, lst );
}

void Deck::addKeyword( const DeckKeyword& keyword ) {
    DeckKeyword kw = keyword;
    this->addKeyword( std::move( kw ) );
}


DeckKeyword& Deck::getKeyword( size_t index ) {
    return this->keywordList.at( index );
}

MessageContainer&  Deck::getMessageContainer() const {
    return this->m_messageContainer;
}

UnitSystem& Deck::getDefaultUnitSystem() {
    return this->defaultUnits;
}

const UnitSystem& Deck::getDefaultUnitSystem() const {
    return this->defaultUnits;
}

UnitSystem& Deck::getActiveUnitSystem() {
    return this->activeUnits;
}

const UnitSystem& Deck::getActiveUnitSystem() const {
    return this->activeUnits;
}

const std::string Deck::getDataFile() const {
    return m_dataFile;
}

void Deck::setDataFile(const std::string& dataFile) {
    m_dataFile = dataFile;
}

Deck::iterator Deck::begin() {
    return this->keywordList.begin();
}

Deck::iterator Deck::end() {
    return this->keywordList.end();
}

namespace {

bool isSectionDelimiter( const DeckKeyword& keyword ) {
    const auto& name = keyword.name();
    for( const auto& x : { "RUNSPEC", "GRID", "EDIT", "PROPS",
                            "REGIONS", "SOLUTION", "SUMMARY", "SCHEDULE" } )
        if( name == x ) return true;

    return false;
}

std::pair< DeckView::const_iterator, DeckView::const_iterator >
find_section( const Deck& deck, const std::string& keyword ) {

    const auto fn = [&keyword]( const DeckKeyword& kw ) {
        return kw.name() == keyword;
    };

    auto first = std::find_if( deck.begin(), deck.end(), fn );

    if( first == deck.end() )
        return { first, first };

    auto last = std::find_if( first + 1, deck.end(), isSectionDelimiter );

    if( last != deck.end() && last->name() == keyword )
        throw std::invalid_argument( std::string( "Deck contains the '" ) + keyword + "' section multiple times" );

    return { first, last };
}

}

Section::Section( const Deck& deck, const std::string& section )
    : DeckView( find_section( deck, section ) ),
        section_name( section ),
        units( deck.getActiveUnitSystem() )
{}

const std::string& Section::name() const {
    return this->section_name;
}

const UnitSystem& Section::unitSystem() const {
    return this->units;
}

bool Section::hasRUNSPEC(const Deck& deck)  { return deck.hasKeyword( "RUNSPEC" ); }
bool Section::hasGRID(const Deck& deck)     { return deck.hasKeyword( "GRID" ); }
bool Section::hasEDIT(const Deck& deck)     { return deck.hasKeyword( "EDIT" ); }
bool Section::hasPROPS(const Deck& deck)    { return deck.hasKeyword( "PROPS" ); }
bool Section::hasREGIONS(const Deck& deck)  { return deck.hasKeyword( "REGIONS" ); }
bool Section::hasSOLUTION(const Deck& deck) { return deck.hasKeyword( "SOLUTION" ); }
bool Section::hasSUMMARY(const Deck& deck)  { return deck.hasKeyword( "SUMMARY" ); }
bool Section::hasSCHEDULE(const Deck& deck) { return deck.hasKeyword( "SCHEDULE" ); }

}
