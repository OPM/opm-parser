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
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParserKW.hpp>
#include <opm/parser/eclipse/RawDeck/RawParserKWs.hpp>
#include <opm/parser/eclipse/RawDeck/RawConsts.hpp>
#include <opm/parser/eclipse/Logger/Logger.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>


namespace Opm {

    Parser::Parser() {
    }

    DeckPtr Parser::parse(const std::string &path) {
        RawDeckPtr rawDeck = readToRawDeck(path);

        DeckPtr deck(new Deck());
        for (size_t i = 0; i < rawDeck->size(); i++) {
            RawKeywordConstPtr rawKeyword = rawDeck->getKeyword(i);

            if (hasKeyword(rawKeyword->getKeywordName())) {
                ParserKWConstPtr parserKW = m_parserKeywords[rawKeyword->getKeywordName()];
                DeckKWConstPtr deckKW = parserKW->parse(rawKeyword);
                deck->addKeyword(deckKW);
            }
            else
                std::cerr << "Keyword: " << rawKeyword->getKeywordName() << " is not recognized, skipping this." << std::endl;
        }
        return deck;
    }

    void Parser::addKW(ParserKWConstPtr parserKW) {
        m_parserKeywords.insert(std::make_pair(parserKW->getName(), parserKW));
    }
    
    bool Parser::hasKeyword(const std::string& keyword) const {
        return m_parserKeywords.find(keyword) != m_parserKeywords.end();
    }

    RawDeckPtr Parser::readToRawDeck(const std::string& path) {
        RawDeckPtr rawDeck(new RawDeck(RawParserKWsConstPtr(new RawParserKWs())));
        readToRawDeck(rawDeck, path);
        return rawDeck;
    }

    /// The main data reading function, reads one and one keyword into the RawDeck
    /// If the INCLUDE keyword is found, the specified include file is inline read into the RawDeck.
    /// The data is read into a keyword, record by record, until the fixed number of records specified
    /// in the RawParserKW is met, or till a slash on a separate line is found.

    void Parser::readToRawDeck(RawDeckPtr rawDeck, const std::string& path) {
        boost::filesystem::path dataFolderPath = verifyValidInputPath(path);
        {
            std::ifstream inputstream;
            inputstream.open(path.c_str());

            std::string line;
            RawKeywordPtr currentRawKeyword;
            while (std::getline(inputstream, line)) {
                std::string keywordString;
                if (currentRawKeyword == NULL) {
                    if (RawKeyword::tryParseKeyword(line, keywordString)) {
                        currentRawKeyword = RawKeywordPtr(new RawKeyword(keywordString));
                        if (rawDeck->isKeywordFinished(currentRawKeyword)) {
                            rawDeck->addKeyword(currentRawKeyword);
                            currentRawKeyword.reset();
                        }
                    }
                } else if (currentRawKeyword != NULL && RawKeyword::lineContainsData(line)) {
                    currentRawKeyword->addRawRecordString(line);
                    if (rawDeck->isKeywordFinished(currentRawKeyword)) {
                        // The INCLUDE keyword has fixed lenght 1, will hit here
                        if (currentRawKeyword->getKeywordName() == Opm::RawConsts::include)
                            processIncludeKeyword(rawDeck, currentRawKeyword, dataFolderPath);
                        else
                            rawDeck->addKeyword(currentRawKeyword);

                        currentRawKeyword.reset();
                    }
                } else if (currentRawKeyword != NULL && RawKeyword::lineTerminatesKeyword(line)) {
                    if (!currentRawKeyword->isPartialRecordStringEmpty()) {
                        // This is an error in the input file, but sometimes occurs
                        currentRawKeyword->addRawRecordString(std::string(1, Opm::RawConsts::slash));
                    }
                    // Don't need to check for include here, since only non-fixed lenght keywords come here.
                    rawDeck->addKeyword(currentRawKeyword);
                    currentRawKeyword.reset();
                }
            }
            inputstream.close();
        }
    }

    void Parser::processIncludeKeyword(RawDeckPtr rawDeck, RawKeywordConstPtr keyword, const boost::filesystem::path& dataFolderPath) {
        RawRecordConstPtr firstRecord = keyword->getRecord(0);
        std::string includeFileString = firstRecord->getItem(0);
        boost::filesystem::path pathToIncludedFile(dataFolderPath);
        pathToIncludedFile /= includeFileString;

        readToRawDeck(rawDeck, pathToIncludedFile.string());
    }

    boost::filesystem::path Parser::verifyValidInputPath(const std::string& inputPath) {
        Logger::info("Verifying path: " + inputPath);
        boost::filesystem::path pathToInputFile(inputPath);
        if (!boost::filesystem::is_regular_file(pathToInputFile)) {
            Logger::error("Unable to open file with path: " + inputPath);
            throw std::invalid_argument("Given path is not a valid file-path, path: " + inputPath);
        }
        return pathToInputFile.parent_path();
    }


} // namespace Opm
