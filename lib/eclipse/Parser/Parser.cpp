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

#include <cctype>
#include <fstream>
#include <memory>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <opm/json/JsonObject.hpp>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParserItem.hpp>
#include <opm/parser/eclipse/Parser/ParserKeyword.hpp>
#include <opm/parser/eclipse/Parser/ParserRecord.hpp>
#include <opm/parser/eclipse/RawDeck/RawConsts.hpp>
#include <opm/parser/eclipse/RawDeck/RawEnums.hpp>
#include <opm/parser/eclipse/RawDeck/RawRecord.hpp>
#include <opm/parser/eclipse/RawDeck/RawKeyword.hpp>
#include <opm/parser/eclipse/RawDeck/StarToken.hpp>
#include <opm/parser/eclipse/Utility/Stringview.hpp>

namespace Opm {

namespace {

struct find_comment {
    /*
     * A note on performance: using a function to plug functionality into
     * find_terminator rather than plain functions because it almost ensures
     * inlining, where the plain function can reduce to a function pointer.
     */
    template< typename Itr >
    Itr operator()( Itr begin, Itr end ) const {
        auto itr = std::find( begin, end, '-' );
        for( ; itr != end; itr = std::find( itr + 1, end, '-' ) )
            if( (itr + 1) != end &&  *( itr + 1 ) == '-' ) return itr;

        return end;
    }
};

template< typename Itr, typename Term >
inline Itr find_terminator( Itr begin, Itr end, Term terminator ) {

    auto pos = terminator( begin, end );

    if( pos == begin || pos == end) return pos;

    auto qbegin = std::find_if( begin, end, RawConsts::is_quote() );

    if( qbegin == end || qbegin > pos )
        return pos;

    auto qend = std::find( qbegin + 1, end, *qbegin );

    // Quotes are not balanced - probably an error?!
    if( qend == end ) return end;

    return find_terminator( qend + 1, end, terminator );
}

/**
    This function will return a copy of the input string where all
    characters following '--' are removed. The copy is a view and relies on
    the source string to remain alive. The function handles quoting with
    single quotes and double quotes:

    ABC --Comment                =>  ABC
    ABC '--Comment1' --Comment2  =>  ABC '--Comment1'
    ABC "-- Not balanced quote?  =>  ABC "-- Not balanced quote?
*/
static inline string_view strip_comments( string_view str ) {
    return { str.begin(),
             find_terminator( str.begin(), str.end(), find_comment() ) };
}

template< typename Itr >
inline Itr trim_left( Itr begin, Itr end ) {
    return std::find_if_not( begin, end, RawConsts::is_separator() );
}

template< typename Itr >
inline Itr trim_right( Itr begin, Itr end ) {

    std::reverse_iterator< Itr > rbegin( end );
    std::reverse_iterator< Itr > rend( begin );

    return std::find_if_not( rbegin, rend, RawConsts::is_separator() ).base();
}

inline string_view trim( string_view str ) {
    auto fst = trim_left( str.begin(), str.end() );
    auto lst = trim_right( fst, str.end() );
    return { fst, lst };
}

inline string_view strip_slash( string_view view ) {
    using itr = string_view::const_iterator;
    const auto term = []( itr begin, itr end ) {
        return std::find( begin, end, '/' );
    };

    auto begin = view.begin();
    auto end = view.end();
    auto slash = find_terminator( begin, end, term );

    /* we want to preserve terminating slashes */
    if( slash != end ) ++slash;

    return { begin, slash };
}

inline bool getline( string_view& input, string_view& line ) {
    if( input.empty() ) return false;

    auto end = std::find( input.begin(), input.end(), '\n' );

    line = string_view( input.begin(), end );
    input = string_view( end + 1, input.end() );
    return true;

    /* we know that we always append a newline onto the input string, so we can
     * safely assume that end+1 will either be end-of-input (i.e. empty range)
     * or the start of the next line
     */
}

/*
 * Read the input file and remove everything that isn't interesting data,
 * including stripping comments, removing leading/trailing whitespaces and
 * everything after (terminating) slashes. Manually copying into the string for
 * performance.
 */
inline std::string clean( const std::string& str ) {
    std::string dst;
    dst.resize( str.size() );

    string_view input( str ), line;
    auto dsti = dst.begin();
    while( getline( input, line ) ) {
        line = trim( strip_slash( strip_comments( line ) ) );

        std::copy( line.begin(), line.end(), dsti );
        dsti += std::distance( line.begin(), line.end() );
        *dsti++ = '\n';
    }

    dst.resize( std::distance( dst.begin(), dsti ) );
    return dst;
}

const std::string emptystr = "";

struct file {
    file( boost::filesystem::path p, const std::string& in ) :
        input( in ), path( p )
    {}

    string_view input;
    size_t lineNR = 0;
    boost::filesystem::path path;
};

class InputStack : public std::stack< file, std::vector< file > > {
    public:
        void push( std::string&& input, boost::filesystem::path p = "" );

    private:
        std::list< std::string > string_storage;
        using base = std::stack< file, std::vector< file > >;
};

void InputStack::push( std::string&& input, boost::filesystem::path p ) {
    this->string_storage.push_back( std::move( input ) );
    this->emplace( p, this->string_storage.back() );
}

class ParserState {
    public:
        ParserState( const ParseContext& );
        ParserState( const ParseContext&, boost::filesystem::path );

        void loadString( const std::string& );
        void loadFile( const boost::filesystem::path& );
        void openRootFile( const boost::filesystem::path& );

        void handleRandomText(const string_view& ) const;
        boost::filesystem::path getIncludeFilePath( std::string ) const;
        void addPathAlias( const std::string& alias, const std::string& path );

        const boost::filesystem::path& current_path() const;
        size_t line() const;

        bool done() const;
        string_view getline();
        void closeFile();

    private:
        InputStack input_stack;

        std::map< std::string, std::string > pathMap;
        boost::filesystem::path rootPath;

    public:
        std::shared_ptr< RawKeyword > rawKeyword;
        string_view nextKeyword = emptystr;
        Deck deck;
        const ParseContext& parseContext;
        bool unknown_keyword = false;
};


const boost::filesystem::path& ParserState::current_path() const {
    return this->input_stack.top().path;
}

size_t ParserState::line() const {
    return this->input_stack.top().lineNR;
}

bool ParserState::done() const {

    while( !this->input_stack.empty() &&
            this->input_stack.top().input.empty() )
        const_cast< ParserState* >( this )->input_stack.pop();

    return this->input_stack.empty();
}

string_view ParserState::getline() {
    string_view ln;

    Opm::getline( this->input_stack.top().input, ln );
    this->input_stack.top().lineNR++;

    return ln;
}

void ParserState::closeFile() {
    this->input_stack.pop();
}

ParserState::ParserState(const ParseContext& __parseContext) :
    parseContext( __parseContext )
{}

ParserState::ParserState( const ParseContext& context,
                          boost::filesystem::path p ) :
    rootPath( boost::filesystem::canonical( p ).parent_path() ),
    parseContext( context )
{
    openRootFile( p );
}

void ParserState::loadString(const std::string& input) {
    this->input_stack.push( clean( input + "\n" ) );
}

void ParserState::loadFile(const boost::filesystem::path& inputFile) {

    boost::filesystem::path inputFileCanonical;
    try {
        inputFileCanonical = boost::filesystem::canonical(inputFile);
    } catch (boost::filesystem::filesystem_error fs_error) {
        std::string msg = "Could not open file: " + inputFile.string();
        parseContext.handleError( ParseContext::PARSE_MISSING_INCLUDE , deck.getMessageContainer() , msg);
        return;
    }

    const auto closer = []( std::FILE* f ) { std::fclose( f ); };
    std::unique_ptr< std::FILE, decltype( closer ) > ufp(
            std::fopen( inputFileCanonical.string().c_str(), "rb" ),
            closer
            );

    // make sure the file we'd like to parse is readable
    if( !ufp ) {
        std::string msg = "Could not read from file: " + inputFile.string();
        parseContext.handleError( ParseContext::PARSE_MISSING_INCLUDE , deck.getMessageContainer() , msg);
        return;
    }

    /*
     * read the input file C-style. This is done for performance
     * reasons, as streams are slow
     */

    auto* fp = ufp.get();
    std::string buffer;
    std::fseek( fp, 0, SEEK_END );
    buffer.resize( std::ftell( fp ) + 1 );
    std::rewind( fp );
    const auto readc = std::fread( &buffer[ 0 ], 1, buffer.size() - 1, fp );
    buffer.back() = '\n';

    if( std::ferror( fp ) || readc != buffer.size() - 1 )
        throw std::runtime_error( "Error when reading input file '"
                                + inputFileCanonical.string() + "'" );

    this->input_stack.push( clean( buffer ), inputFileCanonical );
}

/*
 * We have encountered 'random' characters in the input file which
 * are not correctly formatted as a keyword heading, and not part
 * of the data section of any keyword.
 */

void ParserState::handleRandomText(const string_view& keywordString ) const {
    std::string errorKey;
    std::stringstream msg;
    std::string trimmedCopy = keywordString.string();

    if (trimmedCopy == "/") {
        errorKey = ParseContext::PARSE_RANDOM_SLASH;
        msg << "Extra '/' detected at: "
            << this->current_path()
            << ":" << this->line();
    } else {
        errorKey = ParseContext::PARSE_RANDOM_TEXT;
        msg << "String \'" << keywordString
            << "\' not formatted/recognized as valid keyword at: "
            << this->current_path()
            << ":" << this->line();
    }
    parseContext.handleError( errorKey , deck.getMessageContainer() , msg.str() );
}

void ParserState::openRootFile( const boost::filesystem::path& inputFile) {
    this->loadFile( inputFile );
    this->deck.setDataFile( inputFile.string() );
    const boost::filesystem::path& inputFileCanonical = boost::filesystem::canonical(inputFile);
    rootPath = inputFileCanonical.parent_path();
}

boost::filesystem::path ParserState::getIncludeFilePath( std::string path ) const {
    static const std::string pathKeywordPrefix("$");
    static const std::string validPathNameCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_");

    size_t positionOfPathName = path.find(pathKeywordPrefix);

    if ( positionOfPathName != std::string::npos) {
        std::string stringStartingAtPathName = path.substr(positionOfPathName+1);
        size_t cutOffPosition = stringStartingAtPathName.find_first_not_of(validPathNameCharacters);
        std::string stringToFind = stringStartingAtPathName.substr(0, cutOffPosition);
        std::string stringToReplace = this->pathMap.at( stringToFind );
        boost::replace_all(path, pathKeywordPrefix + stringToFind, stringToReplace);
    }

    // Check if there are any backslashes in the path...
    if (path.find('\\') != std::string::npos) {
        // ... if so, replace with slashes and create a warning.
        std::replace(path.begin(), path.end(), '\\', '/');
        deck.getMessageContainer().warning("Replaced one or more backslash with a slash in an INCLUDE path.");
    }

    boost::filesystem::path includeFilePath(path);

    if (includeFilePath.is_relative())
        return this->rootPath / includeFilePath;

    return includeFilePath;
}

void ParserState::addPathAlias( const std::string& alias, const std::string& path ) {
    this->pathMap.emplace( alias, path );
}

std::shared_ptr< RawKeyword > createRawKeyword( const string_view& kw, ParserState& parserState, const Parser& parser ) {
    auto keywordString = ParserKeyword::getDeckName( kw );

    if( !parser.isRecognizedKeyword( keywordString ) ) {
        if( ParserKeyword::validDeckName( keywordString ) ) {
            std::string msg = "Keyword " + keywordString + " not recognized.";
            auto& msgContainer = parserState.deck.getMessageContainer();
            parserState.parseContext.handleError( ParseContext::PARSE_UNKNOWN_KEYWORD, msgContainer, msg );
            parserState.unknown_keyword = true;
            return {};
        }

        if (!parserState.unknown_keyword)
            parserState.handleRandomText( keywordString );

        return {};
    }
    parserState.unknown_keyword = false;


    const auto* parserKeyword = parser.getParserKeywordFromDeckName( keywordString );

    if( parserKeyword->getSizeType() == SLASH_TERMINATED || parserKeyword->getSizeType() == UNKNOWN) {

        const auto rawSizeType = parserKeyword->getSizeType() == SLASH_TERMINATED
                                ? Raw::SLASH_TERMINATED
                                : Raw::UNKNOWN;

        return std::make_shared< RawKeyword >( keywordString, rawSizeType,
                                                parserState.current_path().string(),
                                                parserState.line() );
    }

    if( parserKeyword->hasFixedSize() ) {
        return std::make_shared< RawKeyword >( keywordString,
                                                parserState.current_path().string(),
                                                parserState.line(),
                                                parserKeyword->getFixedSize(),
                                                parserKeyword->isTableCollection() );
    }

    const auto& sizeKeyword = parserKeyword->getSizeDefinitionPair();
    const auto& deck = parserState.deck;
    size_t targetSize;

    if( deck.hasKeyword(sizeKeyword.first ) ) {
        const auto& sizeDefinitionKeyword = deck.getKeyword(sizeKeyword.first);
        const auto& record = sizeDefinitionKeyword.getRecord(0);

        targetSize = record.getItem( sizeKeyword.second ).get< int >( 0 );
    } else {
        const auto* keyword = parser.getKeyword( sizeKeyword.first );
        const auto& record = keyword->getRecord(0);
        const auto& int_item = record.get( sizeKeyword.second );

        targetSize = int_item.getDefault< int >( );
    }

    return std::make_shared< RawKeyword >( keywordString,
                                           parserState.current_path().string(),
                                           parserState.line(),
                                           targetSize,
                                           parserKeyword->isTableCollection() );
}

bool tryParseKeyword( ParserState& parserState, const Parser& parser ) {
    if (parserState.nextKeyword.length() > 0) {
        parserState.rawKeyword = createRawKeyword( parserState.nextKeyword, parserState, parser );
        parserState.nextKeyword = emptystr;
    }

    if (parserState.rawKeyword && parserState.rawKeyword->isFinished())
        return true;

    while( !parserState.done() ) {
        auto line = parserState.getline();

        if( line.empty() && !parserState.rawKeyword ) continue;
        if( line.empty() && !parserState.rawKeyword->is_title() ) continue;

        std::string keywordString;

        if( parserState.rawKeyword == NULL ) {
            if( RawKeyword::isKeywordPrefix( line, keywordString ) ) {
                parserState.rawKeyword = createRawKeyword( keywordString, parserState, parser );
            } else {
                /* We are looking at some random gibberish?! */
                if (!parserState.unknown_keyword)
                    parserState.handleRandomText( line );
            }
        } else {
            if (parserState.rawKeyword->getSizeType() == Raw::UNKNOWN) {
                if( parser.isRecognizedKeyword( line ) ) {
                    parserState.rawKeyword->finalizeUnknownSize();
                    parserState.nextKeyword = line;
                    return true;
                }
            }
            parserState.rawKeyword->addRawRecordString(line);
        }

        if (parserState.rawKeyword
            && parserState.rawKeyword->isFinished()
            && parserState.rawKeyword->getSizeType() != Raw::UNKNOWN)
        {
            return true;
        }
    }

    if (parserState.rawKeyword
        && parserState.rawKeyword->getSizeType() == Raw::UNKNOWN)
    {
        parserState.rawKeyword->finalizeUnknownSize();
    }

    return false;
}

bool parseState( ParserState& parserState, const Parser& parser ) {

    while( !parserState.done() ) {

        parserState.rawKeyword.reset();

        const bool streamOK = tryParseKeyword( parserState, parser );
        if( !parserState.rawKeyword && !streamOK )
            continue;

        if (parserState.rawKeyword->getKeywordName() == Opm::RawConsts::end)
            return true;

        if (parserState.rawKeyword->getKeywordName() == Opm::RawConsts::endinclude) {
            parserState.closeFile();
            continue;
        }

        if (parserState.rawKeyword->getKeywordName() == Opm::RawConsts::paths) {
            for( const auto& record : *parserState.rawKeyword ) {
                std::string pathName = readValueToken<std::string>(record.getItem(0));
                std::string pathValue = readValueToken<std::string>(record.getItem(1));
                parserState.addPathAlias( pathName, pathValue );
            }

            continue;
        }

        if (parserState.rawKeyword->getKeywordName() == Opm::RawConsts::include) {
            auto& firstRecord = parserState.rawKeyword->getFirstRecord( );
            std::string includeFileAsString = readValueToken<std::string>(firstRecord.getItem(0));
            boost::filesystem::path includeFile = parserState.getIncludeFilePath( includeFileAsString );

            parserState.loadFile( includeFile );
            continue;
        }

        if( parser.isRecognizedKeyword( parserState.rawKeyword->getKeywordName() ) ) {
            const auto& kwname = parserState.rawKeyword->getKeywordName();
            const auto* parserKeyword = parser.getParserKeywordFromDeckName( kwname );
            parserState.deck.addKeyword( parserKeyword->parse( parserState.parseContext, parserState.deck.getMessageContainer(), parserState.rawKeyword ) );
        } else {
            DeckKeyword deckKeyword( parserState.rawKeyword->getKeywordName(), false );
            const std::string msg = "The keyword " + parserState.rawKeyword->getKeywordName() + " is not recognized";
            deckKeyword.setLocation( parserState.rawKeyword->getFilename(),
                    parserState.rawKeyword->getLineNR());
            parserState.deck.addKeyword( std::move( deckKeyword ) );
            parserState.deck.getMessageContainer().warning(
                parserState.current_path().string(), msg, parserState.line() );
        }
    }

    return true;
}

}


    /* stripComments only exists so that the unit tests can verify it.
     * strip_comment is the actual (internal) implementation
     */
    std::string Parser::stripComments( const std::string& str ) {
        return { str.begin(),
                 find_terminator( str.begin(), str.end(), find_comment() ) };
    }

    Parser::Parser(bool addDefault) {
        if (addDefault)
            addDefaultKeywords();
    }


    /*
     About INCLUDE: Observe that the ECLIPSE parser is slightly unlogical
     when it comes to nested includes; the path to an included file is always
     interpreted relative to the filesystem location of the DATA file, and
     not the location of the file issuing the INCLUDE command. That behaviour
     is retained in the current implementation.
     */

    inline void assertFullDeck(const ParseContext& context) {
        if (context.hasKey(ParseContext::PARSE_MISSING_SECTIONS))
            throw new std::logic_error("Cannot construct a state in partial deck context");
    }

    EclipseState Parser::parse(const std::string &filename, const ParseContext& context) {
        assertFullDeck(context);
        return EclipseState( Parser{}.parseFile( filename, context ), context );
    }

    EclipseState Parser::parse(const Deck& deck, const ParseContext& context) {
        assertFullDeck(context);
        return EclipseState(deck, context);
    }

    EclipseState Parser::parseData(const std::string &data, const ParseContext& context) {
        assertFullDeck(context);
        Parser p;
        auto deck = p.parseString(data, context);
        return parse(deck, context);
    }

    EclipseGrid Parser::parseGrid(const std::string &filename, const ParseContext& context) {
        if (context.hasKey(ParseContext::PARSE_MISSING_SECTIONS))
            return EclipseGrid{ filename };
        return parse(filename, context).getInputGrid();
    }

    EclipseGrid Parser::parseGrid(const Deck& deck, const ParseContext& context)
    {
        if (context.hasKey(ParseContext::PARSE_MISSING_SECTIONS))
            return EclipseGrid{ deck };
        return parse(deck, context).getInputGrid();
    }

    EclipseGrid Parser::parseGridData(const std::string &data, const ParseContext& context) {
        Parser parser;
        auto deck = parser.parseString(data, context);
        if (context.hasKey(ParseContext::PARSE_MISSING_SECTIONS)) {
            return EclipseGrid{ deck };
        }
        return parse(deck, context).getInputGrid();
    }

    Deck Parser::parseFile(const std::string &dataFileName, const ParseContext& parseContext) const {
        ParserState parserState( parseContext, dataFileName );
        parseState( parserState, *this );
        applyUnitsToDeck( parserState.deck );

        return std::move( parserState.deck );
    }

    Deck Parser::parseString(const std::string &data, const ParseContext& parseContext) const {
        ParserState parserState( parseContext );
        parserState.loadString( data );

        parseState( parserState, *this );
        applyUnitsToDeck( parserState.deck );

        return std::move( parserState.deck );
    }

    size_t Parser::size() const {
        return m_deckParserKeywords.size();
    }

    const ParserKeyword* Parser::matchingKeyword(const string_view& name) const {
        for (auto iter = m_wildCardKeywords.begin(); iter != m_wildCardKeywords.end(); ++iter) {
            if (iter->second->matches(name))
                return iter->second;
        }
        return nullptr;
    }

    bool Parser::hasWildCardKeyword(const std::string& internalKeywordName) const {
        return (m_wildCardKeywords.count(internalKeywordName) > 0);
    }

    bool Parser::isRecognizedKeyword(const string_view& name ) const {
        if( !ParserKeyword::validDeckName( name ) )
            return false;

        if( m_deckParserKeywords.count( name ) )
            return true;

        return bool( matchingKeyword( name ) );
    }

void Parser::addParserKeyword( std::unique_ptr< const ParserKeyword >&& parserKeyword) {
    string_view name( parserKeyword->getName() );
    auto* ptr = parserKeyword.get();

    /* Store the keywords in the keyword storage. They aren't free'd until the
     * parser gets destroyed, even if there is no reasonable way to reach them
     * (effectively making them leak). This is not a big problem because:
     *
     * * A keyword can be added that overwrites some *but not all* deckname ->
     *   keyword mappings. Keeping track of this is more hassle than worth for
     *   what is essentially edge case usage.
     * * We can store (and search) via string_view's from the keyword added
     *   first because we know that it will be kept around, i.e. we don't have to
     *   deal with subtle lifetime issues.
     * * It means we aren't reliant on some internal name mapping, and can only
     * be concerned with interesting behaviour.
     * * Finally, these releases would in practice never happen anyway until
     *   the parser went out of scope, and now they'll also be cleaned up in the
     *   same sweep.
     */

    this->keyword_storage.push_back( std::move( parserKeyword ) );

    for (auto nameIt = ptr->deckNamesBegin();
            nameIt != ptr->deckNamesEnd();
            ++nameIt)
    {
        m_deckParserKeywords[ *nameIt ] = ptr;
    }

    if (ptr->hasMatchRegex()) {
        m_wildCardKeywords[ name ] = ptr;
    }

}


void Parser::addParserKeyword(const Json::JsonObject& jsonKeyword) {
    addParserKeyword( std::unique_ptr< ParserKeyword >( new ParserKeyword( jsonKeyword ) ) );
}

bool Parser::hasKeyword( const std::string& name ) const {
    return this->m_deckParserKeywords.find( string_view( name ) )
        != this->m_deckParserKeywords.end();
}

const ParserKeyword* Parser::getKeyword( const std::string& name ) const {
    return getParserKeywordFromDeckName( string_view( name ) );
}

const ParserKeyword* Parser::getParserKeywordFromDeckName(const string_view& name ) const {
    auto candidate = m_deckParserKeywords.find( name );

    if( candidate != m_deckParserKeywords.end() ) return candidate->second;

    const auto* wildCardKeyword = matchingKeyword( name );

    if ( !wildCardKeyword )
        throw std::invalid_argument( "Do not have parser keyword for parsing: " + name );

    return wildCardKeyword;
}

std::vector<std::string> Parser::getAllDeckNames () const {
    std::vector<std::string> keywords;
    for (auto iterator = m_deckParserKeywords.begin(); iterator != m_deckParserKeywords.end(); iterator++) {
        keywords.push_back(iterator->first.string());
    }
    for (auto iterator = m_wildCardKeywords.begin(); iterator != m_wildCardKeywords.end(); iterator++) {
        keywords.push_back(iterator->first.string());
    }
    return keywords;
}


    void Parser::loadKeywords(const Json::JsonObject& jsonKeywords) {
        if (jsonKeywords.is_array()) {
            for (size_t index = 0; index < jsonKeywords.size(); index++) {
                Json::JsonObject jsonKeyword = jsonKeywords.get_array_item(index);
                addParserKeyword( std::unique_ptr< ParserKeyword >( new ParserKeyword( jsonKeyword ) ) );
            }
        } else
            throw std::invalid_argument("Input JSON object is not an array");
    }

    bool Parser::loadKeywordFromFile(const boost::filesystem::path& configFile) {

        try {
            Json::JsonObject jsonKeyword(configFile);
            addParserKeyword( std::unique_ptr< ParserKeyword >( new ParserKeyword( jsonKeyword ) ) );
            return true;
        }
        catch (...) {
            return false;
        }
    }


    void Parser::loadKeywordsFromDirectory(const boost::filesystem::path& directory, bool recursive) {
        if (!boost::filesystem::exists(directory))
            throw std::invalid_argument("Directory: " + directory.string() + " does not exist.");
        else {
            boost::filesystem::directory_iterator end;
            for (boost::filesystem::directory_iterator iter(directory); iter != end; iter++) {
                if (boost::filesystem::is_directory(*iter)) {
                    if (recursive)
                        loadKeywordsFromDirectory(*iter, recursive);
                } else {
                    if (ParserKeyword::validInternalName(iter->path().filename().string())) {
                        if (!loadKeywordFromFile(*iter))
                            std::cerr << "** Warning: failed to load keyword from file:" << iter->path() << std::endl;
                    }
                }
            }
        }
    }


    void Parser::applyUnitsToDeck(Deck& deck) const {
        /*
         * If multiple unit systems are requested, metric is preferred over
         * lab, and field over metric, for as long as we have no easy way of
         * figuring out which was requested last.
         */
        if( deck.hasKeyword( "LAB" ) )
            deck.getActiveUnitSystem() = UnitSystem::newLAB();
        if( deck.hasKeyword( "FIELD" ) )
            deck.getActiveUnitSystem() = UnitSystem::newFIELD();
        if( deck.hasKeyword( "METRIC" ) )
            deck.getActiveUnitSystem() = UnitSystem::newMETRIC();

        for( auto& deckKeyword : deck ) {

            if( !isRecognizedKeyword( deckKeyword.name() ) ) continue;

            const auto* parserKeyword = getParserKeywordFromDeckName( deckKeyword.name() );
            if( !parserKeyword->hasDimension() ) continue;

            parserKeyword->applyUnitsToDeck(deck , deckKeyword);
        }
    }

    static bool isSectionDelimiter( const DeckKeyword& keyword ) {
        const auto& name = keyword.name();
        for( const auto& x : { "RUNSPEC", "GRID", "EDIT", "PROPS",
                               "REGIONS", "SOLUTION", "SUMMARY", "SCHEDULE" } )
            if( name == x ) return true;

        return false;
    }

    bool Section::checkSectionTopology(const Deck& deck,
                                       const Parser& parser,
                                       bool ensureKeywordSectionAffiliation)
    {
        if( deck.size() == 0 ) {
            std::string msg = "empty decks are invalid\n";
            deck.getMessageContainer().warning(msg);
            return false;
        }

        bool deckValid = true;

        if( deck.getKeyword(0).name() != "RUNSPEC" ) {
            std::string msg = "The first keyword of a valid deck must be RUNSPEC\n";
            auto curKeyword = deck.getKeyword(0);
            deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
            deckValid = false;
        }

        std::string curSectionName = deck.getKeyword(0).name();
        size_t curKwIdx = 1;
        for (; curKwIdx < deck.size(); ++curKwIdx) {
            const auto& curKeyword = deck.getKeyword(curKwIdx);
            const std::string& curKeywordName = curKeyword.name();

            if (!isSectionDelimiter( curKeyword )) {
                if( !parser.isRecognizedKeyword( curKeywordName ) )
                    // ignore unknown keywords for now (i.e. they can appear in any section)
                    continue;

                const auto& parserKeyword = parser.getParserKeywordFromDeckName( curKeywordName );
                if (ensureKeywordSectionAffiliation && !parserKeyword->isValidSection(curSectionName)) {
                    std::string msg =
                        "The keyword '"+curKeywordName+"' is located in the '"+curSectionName
                        +"' section where it is invalid";
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                continue;
            }

            if (curSectionName == "RUNSPEC") {
                if (curKeywordName != "GRID") {
                    std::string msg =
                        "The RUNSPEC section must be followed by GRID instead of "+curKeywordName;
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "GRID") {
                if (curKeywordName != "EDIT" && curKeywordName != "PROPS") {
                    std::string msg =
                        "The GRID section must be followed by EDIT or PROPS instead of "+curKeywordName;
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "EDIT") {
                if (curKeywordName != "PROPS") {
                    std::string msg =
                        "The EDIT section must be followed by PROPS instead of "+curKeywordName;
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "PROPS") {
                if (curKeywordName != "REGIONS" && curKeywordName != "SOLUTION") {
                    std::string msg =
                        "The PROPS section must be followed by REGIONS or SOLUTION instead of "+curKeywordName;
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "REGIONS") {
                if (curKeywordName != "SOLUTION") {
                    std::string msg =
                        "The REGIONS section must be followed by SOLUTION instead of "+curKeywordName;
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "SOLUTION") {
                if (curKeywordName != "SUMMARY" && curKeywordName != "SCHEDULE") {
                    std::string msg =
                        "The SOLUTION section must be followed by SUMMARY or SCHEDULE instead of "+curKeywordName;
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "SUMMARY") {
                if (curKeywordName != "SCHEDULE") {
                    std::string msg =
                        "The SUMMARY section must be followed by SCHEDULE instead of "+curKeywordName;
                    deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                    deckValid = false;
                }

                curSectionName = curKeywordName;
            }
            else if (curSectionName == "SCHEDULE") {
                // schedule is the last section, so every section delimiter after it is wrong...
                std::string msg =
                    "The SCHEDULE section must be the last one ("
                    +curKeywordName+" specified after SCHEDULE)";
                deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
                deckValid = false;
            }
        }

        // SCHEDULE is the last section and it is mandatory, so make sure it is there
        if (curSectionName != "SCHEDULE") {
            const auto& curKeyword = deck.getKeyword(deck.size() - 1);
            std::string msg =
                "The last section of a valid deck must be SCHEDULE (is "+curSectionName+")";
            deck.getMessageContainer().warning(msg, curKeyword.getFileName(), curKeyword.getLineNumber());
            deckValid = false;
        }

        return deckValid;
    }

} // namespace Opm
