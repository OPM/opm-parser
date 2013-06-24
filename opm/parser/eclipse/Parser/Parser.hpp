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

#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <map>
#include <fstream>
#include <boost/shared_ptr.hpp>

#include <opm/parser/eclipse/Logger/Logger.hpp>
#include <opm/parser/eclipse/RawDeck/RawKeyword.hpp>
#include <opm/parser/eclipse/RawDeck/RawDeck.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Parser/ParserKeyword.hpp>

namespace Opm {

    /// The hub of the parsing process.
    /// An input file in the eclipse data format is specified, several steps of parsing is performed
    /// and the semantically parsed result is returned.

    class Parser {
    public:
        Parser();

        /// The starting point of the parsing process. The supplied file is parsed, and the resulting Deck is returned.
        DeckPtr parse(const std::string &path);
        
        /// Function to parse directly from a raw deck
        DeckPtr parseFromRawDeck(RawDeckConstPtr rawDeck);

        /// Reads an eclipse file and returns a tokenized RawDeck
        RawDeckPtr readToRawDeck(const std::string& path) const;

        /// Method to add ParserKeyword instances, these holding type and size information about the keywords and their data.
        void addKeyword(ParserKeywordConstPtr parserKeyword);
        bool hasKeyword(const std::string& keyword) const;

    private:
        std::map<std::string, ParserKeywordConstPtr> m_parserKeywords;
        void readToRawDeck(RawDeckPtr rawDeck, const std::string& path) const;
        void processIncludeKeyword(RawDeckPtr rawDeck, RawKeywordConstPtr keyword, const boost::filesystem::path& dataFolderPath) const;
        boost::filesystem::path verifyValidInputPath(const std::string& inputPath) const;
        void populateDefaultKeywords();
        bool isFixedLenghtKeywordFinished(RawKeywordConstPtr rawKeyword) const;


    };

    typedef boost::shared_ptr<Parser> ParserPtr;
    typedef boost::shared_ptr<const Parser> ParserConstPtr;
} // namespace Opm
#endif  /* PARSER_H */

