/*
 * File:   DeckKeyword.hpp
 * Author: kflik
 *
 * Created on June 3, 2013, 12:55 PM
 */

#ifndef DECKKEYWORD_HPP
#define DECKKEYWORD_HPP

#include <string>
#include <vector>
#include <memory>

#include <opm/parser/eclipse/Deck/Iterator.hpp>

namespace Opm {
    class ParserKeyword;
    class DeckRecord;

    class DeckKeyword {
    public:

        typedef iterator_base< const DeckRecord > const_iterator;

        DeckKeyword(const std::string& keywordName);
        DeckKeyword(const std::string& keywordName, bool knownKeyword);

        const std::string& name() const;
        void setLocation(const std::string& fileName, int lineNumber);
        const std::string& getFileName() const;
        int getLineNumber() const;

        /*!
         * \brief Returns the Parser keyword from which the current deck keyword was created from.
         *
         * If no parser keyword is available, this method throws
         * std::invalid_exception. Use hasParserKeyword() to test if one is available..
         */
        std::shared_ptr<const ParserKeyword> getParserKeyword() const;

        bool hasParserKeyword() const;

        void setParserKeyword(std::shared_ptr<const ParserKeyword> &parserKeyword);

        size_t size() const;
        void addRecord(std::shared_ptr< const DeckRecord > record);
        std::shared_ptr< const DeckRecord > getRecord(size_t index) const;
        std::shared_ptr< const DeckRecord > getDataRecord() const;
        void setDataKeyword(bool isDataKeyword = true);
        bool isKnown() const;
        bool isDataKeyword() const;

        const std::vector<int>& getIntData() const;
        const std::vector<double>& getRawDoubleData() const;
        const std::vector<double>& getSIDoubleData() const;
        const std::vector<float>& getSIFloatData() const;
        const std::vector<float>& getRawFloatData() const;
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
        const_iterator cbegin() const;
        const_iterator cend() const;

    private:
        std::string m_keywordName;
        std::string m_fileName;
        int m_lineNumber;

        std::shared_ptr<const ParserKeyword> m_parserKeyword;
        std::vector<std::shared_ptr< const DeckRecord >> m_recordList;
        bool m_knownKeyword;
        bool m_isDataKeyword;
    };
    typedef std::shared_ptr<DeckKeyword> DeckKeywordPtr;
    typedef std::shared_ptr<const DeckKeyword> DeckKeywordConstPtr;
}

#endif  /* DECKKEYWORD_HPP */

