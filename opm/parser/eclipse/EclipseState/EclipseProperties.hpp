/*
  Copyright 2013 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  OPM is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPM_ECLIPSE_PROPERTIES_HPP
#define OPM_ECLIPSE_PROPERTIES_HPP

#include <utility>
#include <memory>
#include <set>

#include <opm/parser/eclipse/EclipseState/Grid/GridProperties.hpp>


namespace Opm {

    template< typename > class GridProperty;
    template< typename > class GridProperties;

    class Box;
    class BoxManager;
    class Deck;
    class DeckItem;
    class DeckKeyword;
    class DeckRecord;
    class EclipseGrid;
    class EclipseState;
    class InitConfig;
    class IOConfig;
    class ParseMode;
    class Schedule;
    class Section;
    class TableManager;
    class TransMult;
    class UnitSystem;

    class EclipseProperties
    {
    public:
        enum EnabledTypes {
            IntProperties = 0x01,
            DoubleProperties = 0x02,
            AllProperties = IntProperties | DoubleProperties
        };

        EclipseProperties( std::shared_ptr<const Deck>         deck,
                           std::shared_ptr<const TableManager> tableManager,
                           std::shared_ptr<const EclipseGrid>  eclipseGrid);

        static void setKeywordBox( const DeckKeyword& deckKeyword, const DeckRecord&, BoxManager& boxManager);

        std::shared_ptr<GridProperty<int> > getRegion( const DeckItem& regionItem ) const;
        std::shared_ptr<GridProperty<int> > getDefaultRegion() const;
        std::string getDefaultRegionKeyword() const;
        void setDefaultRegionKeyword(std::string defaultRegionKeyword);

        std::shared_ptr<GridProperty<int> > getIntGridProperty( const std::string& keyword ) const;
        std::shared_ptr<GridProperty<double> > getDoubleGridProperty( const std::string& keyword ) const;
        std::shared_ptr<GridProperties<int> > getIntGridProperties() const;
        std::shared_ptr<GridProperties<double> > getDoubleGridProperties() const;
        bool hasDeckIntGridProperty(const std::string& keyword) const;
        bool hasDeckDoubleGridProperty(const std::string& keyword) const;
        bool supportsGridProperty(const std::string& keyword, int enabledTypes=AllProperties) const;

    private:
        void processGridProperties( std::shared_ptr<const Deck> deck,
                                    std::shared_ptr<const EclipseGrid> eclipseGrid,
                                    int enabledTypes);

        double getSIScaling(const std::string &dimensionString) const;

        void scanSection(std::shared_ptr<Opm::Section> section,
                         std::shared_ptr<const EclipseGrid> eclipseGrid,
                         int enabledTypes);

        void handleADDKeyword(     const DeckKeyword& deckKeyword, BoxManager& boxManager, int enabledTypes);
        void handleBOXKeyword(     const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleCOPYKeyword(    const DeckKeyword& deckKeyword, BoxManager& boxManager, int enabledTypes);
        void handleENDBOXKeyword(  BoxManager& boxManager);
        void handleEQUALSKeyword(  const DeckKeyword& deckKeyword, BoxManager& boxManager, int enabledTypes);
        void handleMULTIPLYKeyword(const DeckKeyword& deckKeyword, BoxManager& boxManager, int enabledTypes);

        void handleADDREGKeyword(  const DeckKeyword& deckKeyword, int enabledTypes);
        void handleCOPYREGKeyword( const DeckKeyword& deckKeyword, int enabledTypes);
        void handleEQUALREGKeyword(const DeckKeyword& deckKeyword, int enabledTypes);
        void handleMULTIREGKeyword(const DeckKeyword& deckKeyword, int enabledTypes);

        void loadGridPropertyFromDeckKeyword(std::shared_ptr<const Box> inputBox,
                                             const DeckKeyword& deckKeyword,
                                             int enabledTypes = AllProperties);

        void initProperties( std::shared_ptr<const Deck> deck,
                             std::shared_ptr<const TableManager> tableManager,
                             std::shared_ptr<const EclipseGrid>  eclipseGrid );

        const UnitSystem& m_deckUnitSystem;
        std::shared_ptr<GridProperties<int> > m_intGridProperties;
        std::shared_ptr<GridProperties<double> > m_doubleGridProperties;
        std::string m_defaultRegion;
    };
}

#endif // OPM_ECLIPSE_PROPERTIES_HPP
