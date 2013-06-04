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

#ifndef RAWDECK_HPP
#define RAWDECK_HPP
#include <vector>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <opm/parser/eclipse/RawDeck/RawKeyword.hpp>
#include <opm/parser/eclipse/RawDeck/RawParserKWs.hpp>

namespace Opm {

    /// Class representing the most high level structure, a deck. The RawDeck holds non parsed
    /// data, in the form of a list of RawKeywords. The order of the keywords is important, as this
    /// reflects the order they were read in from the eclipse file. The RawDeck forms the basis of the 
    /// semantic parsing that comes after the RawDeck has been created from the eclipse file.

    class RawDeck {
    public:

        /// Constructor that requires information about the fixed record length keywords. 
        /// All relevant keywords with a fixed number of records 
        /// must be specified through the RawParserKW class. This is to be able to know how the records
        /// of the keyword is structured.

        RawDeck(RawParserKWsConstPtr rawParserKWs);
        void addKeyword(RawKeywordConstPtr keyword);
        RawKeywordConstPtr getKeyword(size_t index) const;
        size_t size() const;
        
        // This will move to Parser class when m_rawParserKWs is moved.
        bool isKeywordFinished(RawKeywordConstPtr rawKeyword);
      
        friend std::ostream& operator<<(std::ostream& os, const RawDeck& deck);
        virtual ~RawDeck();

    private:
        std::vector<RawKeywordConstPtr> m_keywords;
        
        // This variable should be replaced by an equivalent collection of ParserKWs, and put in the Parser class
        RawParserKWsConstPtr m_rawParserKWs;

        void processIncludeKeyword(RawKeywordConstPtr keyword, const boost::filesystem::path& dataFolderPath);
        static boost::filesystem::path verifyValidInputPath(const std::string& inputPath);
    };

    typedef boost::shared_ptr<RawDeck> RawDeckPtr;
    typedef boost::shared_ptr<const RawDeck> RawDeckConstPtr;
}

#endif  /* RAWDECK_HPP */

