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

#include <memory>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParserLog.hpp>
#include <opm/parser/eclipse/Parser/ParserKeyword.hpp>
#include <opm/parser/eclipse/RawDeck/RawConsts.hpp>
#include <opm/parser/eclipse/RawDeck/RawEnums.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckIntItem.hpp>

namespace Opm {

    struct ParserState {
        DeckPtr deck;
        ParserLog parserLog;
        boost::filesystem::path dataFile;
        boost::filesystem::path rootPath;
        std::map<std::string, std::string> pathMap;
        size_t lineNR;
        std::shared_ptr<std::istream> inputstream;
        RawKeywordPtr rawKeyword;
        std::string nextKeyword;
        ParserState(const boost::filesystem::path &inputDataFile, DeckPtr deckToFill, const boost::filesystem::path &commonRootPath) {
            lineNR = 0;
            dataFile = inputDataFile;
            deck = deckToFill;
            rootPath = commonRootPath;

            std::ifstream *ifs = new std::ifstream(inputDataFile.string().c_str());
            inputstream.reset(ifs);

            // make sure the file we'd like to parse exists and is
            // readable
            if (!ifs->is_open()) {
                throw std::runtime_error(std::string("Input file '") +
                                         inputDataFile.string() +
                                         std::string("' does not exist or is not readable"));
            }
        }

        ParserState(const std::string &inputData, DeckPtr deckToFill) {
            lineNR = 0;
            dataFile = "";
            deck = deckToFill;
            inputstream.reset(new std::istringstream(inputData));
        }

        ParserState(std::shared_ptr<std::istream> inputStream, DeckPtr deckToFill) {
            lineNR = 0;
            dataFile = "";
            deck = deckToFill;
            inputstream = inputStream;
        }
    };

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

    DeckPtr Parser::parseFile(const std::string &dataFileName, ParserLogPtr parserLog) const {

        std::shared_ptr<ParserState> parserState(new ParserState(dataFileName, DeckPtr(new Deck()), getRootPathFromFile(dataFileName)));

        parseStream(parserState);
        applyUnitsToDeck(parserState->deck);

        if (parserLog)
            *parserLog = parserState->parserLog;

        return parserState->deck;
    }

    DeckPtr Parser::parseString(const std::string &data, ParserLogPtr parserLog) const {

        std::shared_ptr<ParserState> parserState(new ParserState(data, DeckPtr(new Deck())));

        parseStream(parserState);
        applyUnitsToDeck(parserState->deck);

        if (parserLog)
            *parserLog = parserState->parserLog;

        return parserState->deck;
    }

    DeckPtr Parser::parseStream(std::shared_ptr<std::istream> inputStream, ParserLogPtr parserLog) const {

        std::shared_ptr<ParserState> parserState(new ParserState(inputStream, DeckPtr(new Deck())));

        parseStream(parserState);
        applyUnitsToDeck(parserState->deck);

        if (parserLog)
            *parserLog = parserState->parserLog;

        return parserState->deck;
    }

    size_t Parser::size() const {
        return m_deckParserKeywords.size();
    }

    bool Parser::hasInternalKeyword(const std::string& internalKeywordName) const {
        return (m_internalParserKeywords.count(internalKeywordName) > 0);
    }

    ParserKeywordConstPtr Parser::getParserKeywordFromInternalName(const std::string& internalKeywordName) const {
        return m_internalParserKeywords.at(internalKeywordName);
    }

    ParserKeywordConstPtr Parser::matchingKeyword(const std::string& name) const {
        for (auto iter = m_wildCardKeywords.begin(); iter != m_wildCardKeywords.end(); ++iter) {
            if (iter->second->matches(name))
                return iter->second;
        }
        return ParserKeywordConstPtr();
    }

    bool Parser::hasWildCardKeyword(const std::string& internalKeywordName) const {
        return (m_wildCardKeywords.count(internalKeywordName) > 0);
    }

    bool Parser::isRecognizedKeyword(const std::string& deckKeywordName) const {
        if (!ParserKeyword::validDeckName(deckKeywordName, /*acceptLowerCase=*/true))
            return false;

        if (m_deckParserKeywords.count(deckKeywordName) > 0)
            return true;

        ParserKeywordConstPtr wildCardKeyword = matchingKeyword( deckKeywordName );
        return wildCardKeyword?true:false;
    }

    void Parser::addParserKeyword(ParserKeywordConstPtr parserKeyword) {
        m_internalParserKeywords[parserKeyword->getName()] = parserKeyword;

        for (auto nameIt = parserKeyword->deckNamesBegin();
             nameIt != parserKeyword->deckNamesEnd();
             ++nameIt)
        {
            m_deckParserKeywords[*nameIt] = parserKeyword;
        }

        if (parserKeyword->hasMatchRegex())
            m_wildCardKeywords[parserKeyword->getName()] = parserKeyword;
    }

    bool Parser::dropParserKeyword(const std::string& parserKeywordName) {
        // remove from the internal from the internal names
        bool erase = (m_internalParserKeywords.erase( parserKeywordName ) > 0);

        // remove keyword from the deck names map
        auto deckParserKeywordIt = m_deckParserKeywords.begin();
        while (deckParserKeywordIt != m_deckParserKeywords.end()) {
            if (deckParserKeywordIt->second->getName() == parserKeywordName)
                // note the post-increment of the iterator. this is required to keep the
                // iterator valid for while at the same time erasing it...
                m_deckParserKeywords.erase(deckParserKeywordIt++);
            else
                ++ deckParserKeywordIt;
        }

        // remove the keyword from the wildcard list
        m_wildCardKeywords.erase( parserKeywordName );

        return erase;
    }



    ParserKeywordConstPtr Parser::getParserKeywordFromDeckName(const std::string& deckKeywordName) const {
        if (m_deckParserKeywords.count(deckKeywordName)) {
            return m_deckParserKeywords.at(deckKeywordName);
        } else {
            ParserKeywordConstPtr wildCardKeyword = matchingKeyword( deckKeywordName );

            if (wildCardKeyword)
                return wildCardKeyword;
            else
                throw std::invalid_argument("Do not have parser keyword for parsing: " + deckKeywordName);
        }
    }

    std::vector<std::string> Parser::getAllDeckNames () const {
        std::vector<std::string> keywords;
        for (auto iterator = m_deckParserKeywords.begin(); iterator != m_deckParserKeywords.end(); iterator++) {
            keywords.push_back(iterator->first);
        }
        for (auto iterator = m_wildCardKeywords.begin(); iterator != m_wildCardKeywords.end(); iterator++) {
            keywords.push_back(iterator->first);
        }
        return keywords;
    }


    boost::filesystem::path Parser::getIncludeFilePath(std::shared_ptr<ParserState> parserState, std::string path) const
    {
        const std::string pathKeywordPrefix("$");
        const std::string validPathNameCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_");

        size_t positionOfPathName = path.find(pathKeywordPrefix);

        if ( positionOfPathName != std::string::npos) {
            std::string stringStartingAtPathName = path.substr(positionOfPathName+1);
            size_t cutOffPosition = stringStartingAtPathName.find_first_not_of(validPathNameCharacters);
            std::string stringToFind = stringStartingAtPathName.substr(0, cutOffPosition);
            std::string stringToReplace = parserState->pathMap[stringToFind];
            boost::replace_all(path, pathKeywordPrefix + stringToFind, stringToReplace);
        }

        boost::filesystem::path includeFilePath(path);

        if (includeFilePath.is_relative())
            includeFilePath = parserState->rootPath / includeFilePath;

        return includeFilePath;
    }

    bool Parser::parseStream(std::shared_ptr<ParserState> parserState) const {
        bool verbose = false;
        bool stopParsing = false;

        if (parserState->inputstream) {
            while (true) {
                bool streamOK = tryParseKeyword(parserState);
                if (parserState->rawKeyword) {
                    if (parserState->rawKeyword->getKeywordName() == Opm::RawConsts::end) {
                        stopParsing = true;
                        break;
                    }
                    else if (parserState->rawKeyword->getKeywordName() == Opm::RawConsts::endinclude) {
                        break;
                    }
                    else if (parserState->rawKeyword->getKeywordName() == Opm::RawConsts::paths) {
                        for (size_t i = 0; i < parserState->rawKeyword->size(); i++) {
                             RawRecordConstPtr record = parserState->rawKeyword->getRecord(i);
                             std::string pathName = readValueToken<std::string>(record->getItem(0));
                             std::string pathValue = readValueToken<std::string>(record->getItem(1));
                             parserState->pathMap.insert(std::pair<std::string, std::string>(pathName, pathValue));
                        }
                    }
                    else if (parserState->rawKeyword->getKeywordName() == Opm::RawConsts::include) {
                        RawRecordConstPtr firstRecord = parserState->rawKeyword->getRecord(0);
                        std::string includeFileAsString = readValueToken<std::string>(firstRecord->getItem(0));
                        boost::filesystem::path includeFile = getIncludeFilePath(parserState, includeFileAsString);

                        if (verbose)
                            std::cout << parserState->rawKeyword->getKeywordName() << "  " << includeFile << std::endl;

                        std::shared_ptr<ParserState> newParserState (new ParserState(includeFile.string(), parserState->deck, parserState->rootPath));
                        stopParsing = parseStream(newParserState);
                        if (stopParsing) break;
                    } else {
                        if (verbose)
                            std::cout << parserState->rawKeyword->getKeywordName() << std::endl;

                        if (isRecognizedKeyword(parserState->rawKeyword->getKeywordName())) {
                            ParserKeywordConstPtr parserKeyword = getParserKeywordFromDeckName(parserState->rawKeyword->getKeywordName());
                            ParserKeywordActionEnum action = parserKeyword->getAction();
                            if (action == INTERNALIZE) {
                                DeckKeywordPtr deckKeyword = parserKeyword->parse(parserState->rawKeyword);
                                deckKeyword->setParserKeyword(parserKeyword);
                                parserState->deck->addKeyword(deckKeyword);
                            } else if (action == IGNORE_WARNING) 
                                parserState->parserLog.addWarning(parserState->dataFile.string(),
                                                                  parserState->rawKeyword->getLineNR(),
                                                                  "The keyword " + parserState->rawKeyword->getKeywordName() + " is ignored - this might potentially affect the results");
                        } else {
                            DeckKeywordPtr deckKeyword(new DeckKeyword(parserState->rawKeyword->getKeywordName(), false));
                            deckKeyword->setLocation(parserState->rawKeyword->getFilename(),
                                                     parserState->rawKeyword->getLineNR());
                            parserState->deck->addKeyword(deckKeyword);
                            parserState->parserLog.addWarning(parserState->dataFile.string(),
                                                              parserState->rawKeyword->getLineNR(),
                                                              "The keyword " + parserState->rawKeyword->getKeywordName() + " is not recognized");
                        }
                    }
                    parserState->rawKeyword.reset();
                }
                if (!streamOK)
                    break;
            }
        } else
            throw std::invalid_argument("Failed to open file: " + parserState->dataFile.string());
        return stopParsing;
    }


    void Parser::loadKeywords(const Json::JsonObject& jsonKeywords) {
        if (jsonKeywords.is_array()) {
            for (size_t index = 0; index < jsonKeywords.size(); index++) {
                Json::JsonObject jsonKeyword = jsonKeywords.get_array_item(index);
                ParserKeywordConstPtr parserKeyword = ParserKeyword::createFromJson(jsonKeyword);

                addParserKeyword(parserKeyword);
            }
        } else
            throw std::invalid_argument("Input JSON object is not an array");
    }

    RawKeywordPtr Parser::createRawKeyword(const std::string& initialLine, std::shared_ptr<ParserState> parserState) const {
        std::string keywordString = ParserKeyword::getDeckName(initialLine);

        if (isRecognizedKeyword(keywordString)) {
            ParserKeywordConstPtr parserKeyword = getParserKeywordFromDeckName( keywordString );
            ParserKeywordActionEnum action = parserKeyword->getAction();
            
            if (action == THROW_EXCEPTION)
                throw std::invalid_argument("Parsing terminated by fatal keyword: " + keywordString);
            
            if (parserKeyword->getSizeType() == SLASH_TERMINATED || parserKeyword->getSizeType() == UNKNOWN) {
                Raw::KeywordSizeEnum rawSizeType;
                if (parserKeyword->getSizeType() == SLASH_TERMINATED)
                    rawSizeType = Raw::SLASH_TERMINATED;
                else
                    rawSizeType = Raw::UNKNOWN;

                return RawKeywordPtr(new RawKeyword(keywordString , rawSizeType , parserState->dataFile.string(), parserState->lineNR));
            } else {
                if (parserKeyword->hasFixedSize())
                    return RawKeywordPtr(new RawKeyword(keywordString, parserState->dataFile.string() , parserState->lineNR , parserKeyword->getFixedSize(), parserKeyword->isTableCollection()));
                else {
                    // we do not require the presence and correctness of the
                    // size-definition keyword here (e.g. TABDIMS for the saturation
                    // tables). Instead this is ensured by Eclipse::checkDeck()
                    return RawKeywordPtr(new RawKeyword(keywordString, Raw::UNKNOWN, parserState->dataFile.string(), parserState->lineNR));
                }
            }
        } else {
            // create a keyword with an unknown name
            return RawKeywordPtr(new RawKeyword(keywordString, Raw::UNKNOWN, parserState->dataFile.string(), parserState->lineNR));
        }
    }


    std::string Parser::doSpecialHandlingForTitleKeyword(std::string line, std::shared_ptr<ParserState> parserState) const {
        if ((parserState->rawKeyword != NULL) && (parserState->rawKeyword->getKeywordName() == "TITLE"))
                line = line.append("/");
        return line;
    }

    bool Parser::tryParseKeyword(std::shared_ptr<ParserState> parserState) const {
        std::string line;

        if (parserState->nextKeyword.length() > 0) {
            parserState->rawKeyword = createRawKeyword(parserState->nextKeyword, parserState);
            parserState->nextKeyword = "";
        }

        if (parserState->rawKeyword && parserState->rawKeyword->isFinished())
            return true;

        while (std::getline(*parserState->inputstream, line)) {
            // remove comments. note that this is a bit too simplistic as it fails when
            //  having '--' in strings. _nobody_ does this, though ;)...
            int commentPos = line.find("--");
            line = line.substr(0, commentPos);

            boost::algorithm::trim_right(line); // Removing garbage (eg. \r)
            line = doSpecialHandlingForTitleKeyword(line, parserState);
            parserState->lineNR++;

            // skip empty lines
            if (line.size() == 0)
                continue;

            if (parserState->rawKeyword == NULL) {
                // else try to create a new keyword
                std::string keywordName;
                if (RawKeyword::isKeywordPrefix(line, keywordName))
                    parserState->rawKeyword = createRawKeyword(keywordName, parserState);
                else {
                    // to prevent the user from being flooded by warnings, only add a
                    // warning for the first unparsable line if the unparsable lines are
                    // consecutive...
                    static std::string lastWarningFile = parserState->dataFile.string();
                    static int lastWarningLine = -100;

                    int curLine = parserState->lineNR;
                    if (parserState->rawKeyword)
                        curLine = parserState->rawKeyword->getLineNR();

                    if (lastWarningFile == parserState->dataFile.string()
                        && lastWarningLine < curLine - 1)
                    {
                        parserState->parserLog.addWarning(parserState->dataFile.string(),
                                                          parserState->lineNR,
                                                          "Can't parse line: Expected beginning of keyword");
                    }

                    lastWarningFile = parserState->dataFile.string();
                    lastWarningLine = curLine;
                }
            } else {
                if (parserState->rawKeyword->getSizeType() == Raw::UNKNOWN) {
                    // for unknown keywords we can't know if the first item of the next
                    // line is a string or a keyword name which we don't know. We thus
                    // assume that all records of keywords with an unknown number of
                    // records start with a double item...
                    std::string keywordName = ParserKeyword::getDeckName(line);
                    if (ParserKeyword::validDeckName(keywordName, /*acceptLowerCase=*/true))
                    {
                        parserState->rawKeyword->finalizeUnknownSize();
                        parserState->nextKeyword = line;
                        return true;
                    }
                }
                parserState->rawKeyword->addRawRecordString(line);
                line = "";
            }

            if (parserState->rawKeyword
                && parserState->rawKeyword->isFinished()
                && parserState->rawKeyword->getSizeType() != Raw::UNKNOWN)
            {
                return true;
            }
        }
        if (parserState->rawKeyword
            && parserState->rawKeyword->getSizeType() == Raw::UNKNOWN)
        {
            parserState->rawKeyword->finalizeUnknownSize();
        }

        return false;
    }

    bool Parser::loadKeywordFromFile(const boost::filesystem::path& configFile) {

        try {
            Json::JsonObject jsonKeyword(configFile);
            ParserKeywordConstPtr parserKeyword = ParserKeyword::createFromJson(jsonKeyword);
            addParserKeyword(parserKeyword);
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

    boost::filesystem::path Parser::getRootPathFromFile(const boost::filesystem::path &inputDataFile) const {
        boost::filesystem::path root;
        if (inputDataFile.is_absolute())
            root = inputDataFile.parent_path();
        else
            root = boost::filesystem::current_path() / inputDataFile.parent_path();
        return root;
    }

    void Parser::applyUnitsToDeck(DeckPtr deck) const {
        deck->initUnitSystem();
        for (size_t index=0; index < deck->size(); ++index) {
            DeckKeywordPtr deckKeyword = deck->getKeyword( index );
            if (isRecognizedKeyword( deckKeyword->name())) {
                ParserKeywordConstPtr parserKeyword = getParserKeywordFromDeckName( deckKeyword->name() );
                if (parserKeyword->hasDimension()) {
                    parserKeyword->applyUnitsToDeck(deck , deckKeyword);
                }
            }
        }
    }
    

} // namespace Opm
