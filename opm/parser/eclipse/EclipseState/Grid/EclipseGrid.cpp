/*
  Copyright 2014 Statoil ASA.

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


#include <iostream>
#include <tuple>

#include <boost/lexical_cast.hpp>

#include <opm/parser/eclipse/OpmLog/OpmLog.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>

#include <ert/ecl/ecl_grid.h>

namespace Opm {

    /**
       Will create an EclipseGrid instance based on an existing
       GRID/EGRID file.
    */
    EclipseGrid::EclipseGrid(const std::string& filename )
        : m_minpvValue(0),
          m_minpvMode(MinpvMode::ModeEnum::Inactive),
          m_pinch("PINCH")
    {
        ecl_grid_type * new_ptr = ecl_grid_load_case( filename.c_str() );
        if (new_ptr)
            m_grid.reset( new_ptr , ecl_grid_free );
        else
            throw std::invalid_argument("Could not load grid from binary file: " + filename);

        m_nx = static_cast<size_t>( ecl_grid_get_nx( c_ptr() ));
        m_ny = static_cast<size_t>( ecl_grid_get_ny( c_ptr() ));
        m_nz = static_cast<size_t>( ecl_grid_get_nz( c_ptr() ));
    }

    EclipseGrid::EclipseGrid(const ecl_grid_type * src_ptr)
        : m_minpvValue(0),
          m_minpvMode(MinpvMode::ModeEnum::Inactive),
          m_pinch("PINCH")
    {
        m_grid.reset( ecl_grid_alloc_copy( src_ptr ) , ecl_grid_free );

        m_nx = static_cast<size_t>( ecl_grid_get_nx( c_ptr() ));
        m_ny = static_cast<size_t>( ecl_grid_get_ny( c_ptr() ));
        m_nz = static_cast<size_t>( ecl_grid_get_nz( c_ptr() ));
    }

    /*
      This creates a grid which only has dimension, and no pointer to
      a true grid structure. This grid will answer false to
      hasCellInfo() - but can be used in all situations where the grid
      dependency is really only on the dimensions.
    */

    EclipseGrid::EclipseGrid(size_t nx, size_t ny , size_t nz,
                             double dx, double dy, double dz)
        : m_minpvValue(0),
          m_minpvMode(MinpvMode::ModeEnum::Inactive),
          m_pinch("PINCH")
    {
        m_nx = nx;
        m_ny = ny;
        m_nz = nz;
        m_grid.reset(ecl_grid_alloc_rectangular(nx, ny, nz, dx, dy, dz, NULL), ecl_grid_free);
    }


    namespace
    {
        // keyword must be DIMENS or SPECGRID
        std::vector<int> getDims(DeckKeywordConstPtr keyword)
        {
            DeckRecordConstPtr record = keyword->getRecord(0);
            std::vector<int> dims = {record->getItem("NX")->getInt(0) ,
                                     record->getItem("NY")->getInt(0) ,
                                     record->getItem("NZ")->getInt(0) };
            return dims;
        }
    } // anonymous namespace

    EclipseGrid::EclipseGrid(std::shared_ptr<const Deck> deck)
        : m_minpvValue(0),
          m_minpvMode(MinpvMode::ModeEnum::Inactive),
          m_pinch("PINCH")
    {
        const bool hasRUNSPEC = Section::hasRUNSPEC(deck);
        const bool hasGRID = Section::hasGRID(deck);
        if (hasRUNSPEC && hasGRID) {
            // Equivalent to first constructor.
            auto runspecSection = std::make_shared<const RUNSPECSection>(deck);
            if (runspecSection->hasKeyword<ParserKeywords::DIMENS>()) {
                DeckKeywordConstPtr dimens = runspecSection->getKeyword<ParserKeywords::DIMENS>();
                std::vector<int> dims = getDims(dimens);
                initGrid(dims, deck);
            } else {
                const std::string msg = "The RUNSPEC section must have the DIMENS keyword with logically Cartesian grid dimensions.";
                OpmLog::addMessage(Log::MessageType::Error , msg);
                throw std::invalid_argument(msg);
            }
        } else if (hasGRID) {
            // Look for SPECGRID instead of DIMENS.
            if (deck->hasKeyword<ParserKeywords::SPECGRID>()) {
                DeckKeywordConstPtr specgrid = deck->getKeyword<ParserKeywords::SPECGRID>();
                std::vector<int> dims = getDims(specgrid);
                initGrid(dims, deck);
            } else {
                const std::string msg = "With no RUNSPEC section, the GRID section must specify the grid dimensions using the SPECGRID keyword.";
                OpmLog::addMessage(Log::MessageType::Error , msg);
                throw std::invalid_argument(msg);
            }
        } else {
            // The deck contains no relevant section, so it is probably a sectionless GRDECL file.
            // Either SPECGRID or DIMENS is OK.
            if (deck->hasKeyword("SPECGRID")) {
                DeckKeywordConstPtr specgrid = deck->getKeyword<ParserKeywords::SPECGRID>();
                std::vector<int> dims = getDims(specgrid);
                initGrid(dims, deck);
            } else if (deck->hasKeyword<ParserKeywords::DIMENS>()) {
                DeckKeywordConstPtr dimens = deck->getKeyword<ParserKeywords::DIMENS>();
                std::vector<int> dims = getDims(dimens);
                initGrid(dims, deck);
            } else {
                const std::string msg = "The deck must specify grid dimensions using either DIMENS or SPECGRID.";
                OpmLog::addMessage(Log::MessageType::Error , msg);
                throw std::invalid_argument(msg);
            }
        }
    }


    void EclipseGrid::initGrid( const std::vector<int>& dims, DeckConstPtr deck) {
        m_nx = static_cast<size_t>(dims[0]);
        m_ny = static_cast<size_t>(dims[1]);
        m_nz = static_cast<size_t>(dims[2]);

        if (hasCornerPointKeywords(deck)) {
            initCornerPointGrid(dims , deck);
        } else if (hasCartesianKeywords(deck)) {
            initCartesianGrid(dims , deck);
        }

        if (deck->hasKeyword<ParserKeywords::PINCH>()) {
            auto record = deck->getKeyword<ParserKeywords::PINCH>( )->getRecord(0);
            auto item = record->getItem<ParserKeywords::PINCH::THRESHOLD_THICKNESS>( );
            m_pinch.setValue( item->getSIDouble(0) );
        }

        if (deck->hasKeyword<ParserKeywords::MINPV>() && deck->hasKeyword<ParserKeywords::MINPVFIL>()) {
            throw std::invalid_argument("Can not have both MINPV and MINPVFIL in deck.");
        }

        if (deck->hasKeyword<ParserKeywords::MINPV>()) {
            auto record = deck->getKeyword<ParserKeywords::MINPV>( )->getRecord(0);
            auto item = record->getItem<ParserKeywords::MINPV::VALUE>( );
            m_minpvValue = item->getSIDouble(0);
            m_minpvMode = MinpvMode::ModeEnum::EclSTD;
        }


        if (deck->hasKeyword<ParserKeywords::MINPVFIL>()) {
            auto record = deck->getKeyword<ParserKeywords::MINPVFIL>( )->getRecord(0);
            auto item = record->getItem<ParserKeywords::MINPVFIL::VALUE>( );
            m_minpvValue = item->getSIDouble(0);
            m_minpvMode = MinpvMode::ModeEnum::OpmFIL;
        }
    }



    size_t EclipseGrid::getNX( ) const {
        return m_nx;
    }

    size_t EclipseGrid::getNY( ) const {
        return m_ny;
    }

    size_t EclipseGrid::getNZ( ) const {
        return m_nz;
    }

    size_t EclipseGrid::getCartesianSize( ) const {
        return m_nx * m_ny * m_nz;
    }

    bool EclipseGrid::isPinchActive( ) const {
        return m_pinch.hasValue();
    }

    double EclipseGrid::getPinchThresholdThickness( ) const {
        return m_pinch.getValue();
    }

    MinpvMode::ModeEnum EclipseGrid::getMinpvMode() const {
        return m_minpvMode;
    }

    double EclipseGrid::getMinpvValue( ) const {
        return m_minpvValue;
    }


    size_t EclipseGrid::getGlobalIndex(size_t i, size_t j, size_t k) const {
        return (i + j * getNX() + k * getNX() * getNY());
    }

    void EclipseGrid::assertGlobalIndex(size_t globalIndex) const {
        if (globalIndex >= getCartesianSize())
            throw std::invalid_argument("input index above valid range");
    }

    void EclipseGrid::assertIJK(size_t i , size_t j , size_t k) const {
        if (i >= getNX() || j >= getNY() || k >= getNZ())
            throw std::invalid_argument("input index above valid range");
    }



    void EclipseGrid::initCartesianGrid(const std::vector<int>& dims , DeckConstPtr deck) {
        if (hasDVDEPTHZKeywords( deck ))
            initDVDEPTHZGrid( dims , deck );
        else if (hasDTOPSKeywords(deck))
            initDTOPSGrid( dims ,deck );
        else
            throw std::invalid_argument("Tried to initialize cartesian grid without all required keywords");
    }


    void EclipseGrid::initDVDEPTHZGrid(const std::vector<int>& dims , DeckConstPtr deck) {
        const std::vector<double>& DXV = deck->getKeyword<ParserKeywords::DXV>()->getSIDoubleData();
        const std::vector<double>& DYV = deck->getKeyword<ParserKeywords::DYV>()->getSIDoubleData();
        const std::vector<double>& DZV = deck->getKeyword<ParserKeywords::DZV>()->getSIDoubleData();
        const std::vector<double>& DEPTHZ = deck->getKeyword<ParserKeywords::DEPTHZ>()->getSIDoubleData();

        assertVectorSize( DEPTHZ , static_cast<size_t>( (dims[0] + 1)*(dims[1] +1 )) , "DEPTHZ");
        assertVectorSize( DXV    , static_cast<size_t>( dims[0] ) , "DXV");
        assertVectorSize( DYV    , static_cast<size_t>( dims[1] ) , "DYV");
        assertVectorSize( DZV    , static_cast<size_t>( dims[2] ) , "DZV");

        m_grid.reset( ecl_grid_alloc_dxv_dyv_dzv_depthz( dims[0] , dims[1] , dims[2] , DXV.data() , DYV.data() , DZV.data() , DEPTHZ.data() , NULL ) , ecl_grid_free);
    }


    void EclipseGrid::initDTOPSGrid(const std::vector<int>& dims , DeckConstPtr deck) {
        std::vector<double> DX = createDVector( dims , 0 , "DX" , "DXV" , deck);
        std::vector<double> DY = createDVector( dims , 1 , "DY" , "DYV" , deck);
        std::vector<double> DZ = createDVector( dims , 2 , "DZ" , "DZV" , deck);
        std::vector<double> TOPS = createTOPSVector( dims , DZ , deck );

        m_grid.reset( ecl_grid_alloc_dx_dy_dz_tops( dims[0] , dims[1] , dims[2] , DX.data() , DY.data() , DZ.data() , TOPS.data() , NULL ) , ecl_grid_free);
    }



    void EclipseGrid::initCornerPointGrid(const std::vector<int>& dims , DeckConstPtr deck) {
        assertCornerPointKeywords( dims , deck);
        {
            DeckKeywordConstPtr ZCORNKeyWord = deck->getKeyword<ParserKeywords::ZCORN>();
            DeckKeywordConstPtr COORDKeyWord = deck->getKeyword<ParserKeywords::COORD>();
            const std::vector<double>& zcorn = ZCORNKeyWord->getSIDoubleData();
            const std::vector<double>& coord = COORDKeyWord->getSIDoubleData();
            const int * actnum = NULL;
            double    * mapaxes = NULL;

            if (deck->hasKeyword<ParserKeywords::ACTNUM>()) {
                DeckKeywordConstPtr actnumKeyword = deck->getKeyword<ParserKeywords::ACTNUM>();
                const std::vector<int>& actnumVector = actnumKeyword->getIntData();
                actnum = actnumVector.data();

            }

            if (deck->hasKeyword<ParserKeywords::MAPAXES>()) {
                DeckKeywordConstPtr mapaxesKeyword = deck->getKeyword<ParserKeywords::MAPAXES>();
                DeckRecordConstPtr record = mapaxesKeyword->getRecord(0);
                mapaxes = new double[6];
                for (size_t i = 0; i < 6; i++) {
                    DeckItemConstPtr item = record->getItem(i);
                    mapaxes[i] = item->getSIDouble(0);
                }
            }


            {
                const std::vector<float> zcorn_float( zcorn.begin() , zcorn.end() );
                const std::vector<float> coord_float( coord.begin() , coord.end() );
                float * mapaxes_float = NULL;
                if (mapaxes) {
                    mapaxes_float = new float[6];
                    for (size_t i=0; i < 6; i++)
                        mapaxes_float[i] = mapaxes[i];
                }

                ecl_grid_type * ecl_grid = ecl_grid_alloc_GRDECL_data(dims[0] , dims[1] , dims[2] , zcorn_float.data() , coord_float.data() , actnum , mapaxes_float);
                m_grid.reset( ecl_grid , ecl_grid_free);

                if (mapaxes) {
                    delete[] mapaxes_float;
                    delete[] mapaxes;
                }
            }
        }
    }



    bool EclipseGrid::hasCornerPointKeywords(DeckConstPtr deck) {
        if (deck->hasKeyword<ParserKeywords::ZCORN>() && deck->hasKeyword<ParserKeywords::COORD>())
            return true;
        else
            return false;
    }


    void EclipseGrid::assertCornerPointKeywords( const std::vector<int>& dims , DeckConstPtr deck)
    {
        const int nx = dims[0];
        const int ny = dims[1];
        const int nz = dims[2];
        {
            DeckKeywordConstPtr ZCORNKeyWord = deck->getKeyword<ParserKeywords::ZCORN>();

            if (ZCORNKeyWord->getDataSize() != static_cast<size_t>(8*nx*ny*nz)) {
                const std::string msg =
                    "Wrong size of the ZCORN keyword: Expected 8*x*ny*nz = "
                    + std::to_string(static_cast<long long>(8*nx*ny*nz)) + " is "
                    + std::to_string(static_cast<long long>(ZCORNKeyWord->getDataSize()));

                OpmLog::addMessage(Log::MessageType::Error , msg);
                throw std::invalid_argument(msg);
            }
        }

        {
            DeckKeywordConstPtr COORDKeyWord = deck->getKeyword<ParserKeywords::COORD>();
            if (COORDKeyWord->getDataSize() != static_cast<size_t>(6*(nx + 1)*(ny + 1))) {
                const std::string msg =
                    "Wrong size of the COORD keyword: Expected 8*(nx + 1)*(ny + 1) = "
                    + std::to_string(static_cast<long long>(8*(nx + 1)*(ny + 1))) + " is "
                    + std::to_string(static_cast<long long>(COORDKeyWord->getDataSize()));
                OpmLog::addMessage(Log::MessageType::Error , msg);
                throw std::invalid_argument(msg);
            }
        }

        if (deck->hasKeyword<ParserKeywords::ACTNUM>()) {
            DeckKeywordConstPtr ACTNUMKeyWord = deck->getKeyword<ParserKeywords::ACTNUM>();
            if (ACTNUMKeyWord->getDataSize() != static_cast<size_t>(nx*ny*nz)) {
                const std::string msg =
                    "Wrong size of the ACTNUM keyword: Expected nx*ny*nz = "
                    + std::to_string(static_cast<long long>(nx*ny*nz)) + " is "
                    + std::to_string(static_cast<long long>(ACTNUMKeyWord->getDataSize()));
                OpmLog::addMessage(Log::MessageType::Error , msg);
                throw std::invalid_argument(msg);
            }
        }
    }



    bool EclipseGrid::hasCartesianKeywords(DeckConstPtr deck) {
        if (hasDVDEPTHZKeywords( deck ))
            return true;
        else
            return hasDTOPSKeywords(deck);
    }


    bool EclipseGrid::hasDVDEPTHZKeywords(DeckConstPtr deck) {
        if (deck->hasKeyword<ParserKeywords::DXV>() &&
            deck->hasKeyword<ParserKeywords::DYV>() &&
            deck->hasKeyword<ParserKeywords::DZV>() &&
            deck->hasKeyword<ParserKeywords::DEPTHZ>())
            return true;
        else
            return false;
    }

    bool EclipseGrid::hasDTOPSKeywords(DeckConstPtr deck) {
        if ((deck->hasKeyword<ParserKeywords::DX>() || deck->hasKeyword<ParserKeywords::DXV>()) &&
            (deck->hasKeyword<ParserKeywords::DY>() || deck->hasKeyword<ParserKeywords::DYV>()) &&
            (deck->hasKeyword<ParserKeywords::DZ>() || deck->hasKeyword<ParserKeywords::DZV>()) &&
            deck->hasKeyword<ParserKeywords::TOPS>())
            return true;
        else
            return false;
    }



    void EclipseGrid::assertVectorSize(const std::vector<double>& vector , size_t expectedSize , const std::string& vectorName) {
        if (vector.size() != expectedSize)
            throw std::invalid_argument("Wrong size for keyword: " + vectorName + ". Expected: " + boost::lexical_cast<std::string>(expectedSize) + " got: " + boost::lexical_cast<std::string>(vector.size()));
    }


    /*
      There is 'something' rotten which is triggered by through this
      method. The TOPS keyword can be of any size in the range
      [nx*ny,nx*ny*nz]. If the keyword has less than nx*ny*nz elements
      the top of the cells is inferred by adding together the top and
      thickness of the cell immediately above.

      For some strange reason the code does not work when the TOPS
      keyword has been fully specified in the input deck. The 'not
      works' is quite mysterious:

        o The TOPS vector itself is numerically correct.

        o The ert Eclipse grid created with ecl_grid_alloc_dx_dy_dz_tops() in
          EclipseGrid::initDTOPSGrid() seems to be perfectly right.

        o The COORD, ZCORN and ACTNUM exported from the ert Grid and
          used as input to the UnstructuredGrid constructor in opm-core
          is correct.

      The final UnstructuredGrid instance created in
      GridManager::initFromEclipseGrid() is bogus - presumably some
      memory is overwritten somewhere - for simple diff based
      debugging of this the grid_ascii_dump() function in opm-core can
      be used.

      -----------------------------------------------------------------

      The current 'solution' is just to enforce that the TOPS keyword
      has exactly nx*ny elements, but this is probably masking a more
      serious problem somewhere.
    */



    std::vector<double> EclipseGrid::createTOPSVector(const std::vector<int>& dims , const std::vector<double>& DZ , DeckConstPtr deck) {
        size_t volume = dims[0] * dims[1] * dims[2];
        size_t area = dims[0] * dims[1];
        DeckKeywordConstPtr TOPSKeyWord = deck->getKeyword<ParserKeywords::TOPS>();
        std::vector<double> TOPS = TOPSKeyWord->getSIDoubleData();

        if (TOPS.size() >= area) {
            size_t initialTOPSize = TOPS.size();

            /* This test really should not be necessary. */
            if (initialTOPSize != area)
                throw std::invalid_argument("Internal error - must have size(TOPS) == nx*ny");

            TOPS.resize( volume );
            for (size_t targetIndex = initialTOPSize; targetIndex < volume; targetIndex++) {
                size_t sourceIndex = targetIndex - area;
                TOPS[targetIndex] = TOPS[sourceIndex] + DZ[sourceIndex];
            }
        }

        if (TOPS.size() != volume)
            throw std::invalid_argument("TOPS size mismatch");

        return TOPS;
    }



    std::vector<double> EclipseGrid::createDVector(const std::vector<int>& dims , size_t dim , const std::string& DKey , const std::string& DVKey, DeckConstPtr deck) {
        size_t volume = dims[0] * dims[1] * dims[2];
        size_t area = dims[0] * dims[1];
        std::vector<double> D;
        if (deck->hasKeyword(DKey)) {
            DeckKeywordConstPtr DKeyWord = deck->getKeyword(DKey);
            D = DKeyWord->getSIDoubleData();


            if (D.size() >= area && D.size() < volume) {
                /*
                  Only the top layer is required; for layers below the
                  top layer the value from the layer above is used.
                */
                size_t initialDSize = D.size();
                D.resize( volume );
                for (size_t targetIndex = initialDSize; targetIndex < volume; targetIndex++) {
                    size_t sourceIndex = targetIndex - area;
                    D[targetIndex] = D[sourceIndex];
                }
            }

            if (D.size() != volume)
                throw std::invalid_argument(DKey + " size mismatch");
        } else {
            DeckKeywordConstPtr DVKeyWord = deck->getKeyword(DVKey);
            const std::vector<double>& DV = DVKeyWord->getSIDoubleData();
            if (DV.size() != (size_t) dims[dim])
                throw std::invalid_argument(DVKey + " size mismatch");
            D.resize( volume );
            scatterDim( dims , dim , DV , D );
        }
        return D;
    }


    void EclipseGrid::scatterDim(const std::vector<int>& dims , size_t dim , const std::vector<double>& DV , std::vector<double>& D) {
        int index[3];
        for (index[2] = 0;  index[2] < dims[2]; index[2]++) {
            for (index[1] = 0; index[1] < dims[1]; index[1]++) {
                for (index[0] = 0;  index[0] < dims[0]; index[0]++) {
                    size_t globalIndex = index[2] * dims[1] * dims[0] + index[1] * dims[0] + index[0];
                    D[globalIndex] = DV[ index[dim] ];
                }
            }
        }
    }


    /*
      This function checks if the grid has a pointer to an underlying
      ecl_grid_type; which must be used to read cell info as
      size/depth/active of individual cells.
    */

    bool EclipseGrid::hasCellInfo() const {
        return static_cast<bool>( m_grid );
    }


    void EclipseGrid::assertCellInfo() const {
        if (!hasCellInfo())
            throw std::invalid_argument("Tried to access cell information in a grid with only dimensions");
    }


    const ecl_grid_type * EclipseGrid::c_ptr() const {
        assertCellInfo();
        return m_grid.get();
    }


    bool EclipseGrid::equal(const EclipseGrid& other) const {
        bool status = (m_pinch.equal( other.m_pinch ) && (ecl_grid_compare( c_ptr() , other.c_ptr() , true , false , false )) && (m_minpvMode == other.getMinpvMode()));
        if(m_minpvMode!=MinpvMode::ModeEnum::Inactive){
            status = status && (m_minpvValue == other.getMinpvValue());
        }
        return  status;
    }


    size_t EclipseGrid::getNumActive( ) const {
        return static_cast<size_t>(ecl_grid_get_nactive( c_ptr() ));
    }

    bool EclipseGrid::cellActive( size_t globalIndex ) const {
        assertGlobalIndex( globalIndex );
        return ecl_grid_cell_active1( c_ptr() , static_cast<int>(globalIndex));
    }

    bool EclipseGrid::cellActive( size_t i , size_t j , size_t k ) const {
        assertIJK(i,j,k);
        return ecl_grid_cell_active3( c_ptr() , static_cast<int>(i),static_cast<int>(j),static_cast<int>(k));
    }


    double EclipseGrid::getCellVolume(size_t globalIndex) const {
        assertGlobalIndex( globalIndex );
        return ecl_grid_get_cell_volume1( c_ptr() , static_cast<int>(globalIndex));
    }


    double EclipseGrid::getCellVolume(size_t i , size_t j , size_t k) const {
        assertIJK(i,j,k);
        return ecl_grid_get_cell_volume3( c_ptr() , static_cast<int>(i),static_cast<int>(j),static_cast<int>(k));
    }

    std::tuple<double,double,double> EclipseGrid::getCellCenter(size_t globalIndex) const {
        assertGlobalIndex( globalIndex );
        {
            double x,y,z;
            ecl_grid_get_xyz1( c_ptr() , static_cast<int>(globalIndex) , &x , &y , &z);
            return std::tuple<double,double,double> {x,y,z};
        }
    }


    std::tuple<double,double,double> EclipseGrid::getCellCenter(size_t i,size_t j, size_t k) const {
        assertIJK(i,j,k);
        {
            double x,y,z;
            ecl_grid_get_xyz3( c_ptr() , static_cast<int>(i),static_cast<int>(j),static_cast<int>(k), &x , &y , &z);
            return std::tuple<double,double,double> {x,y,z};
        }
    }

    double EclipseGrid::getCellDepth(size_t globalIndex) const {
        assertGlobalIndex( globalIndex );
        return ecl_grid_get_cdepth1( c_ptr() , static_cast<int>(globalIndex));
    }


    double EclipseGrid::getCellDepth(size_t i,size_t j, size_t k) const {
        assertIJK(i,j,k);
        return ecl_grid_get_cdepth3( c_ptr() , static_cast<int>(i),static_cast<int>(j),static_cast<int>(k));
    }



    void EclipseGrid::exportACTNUM( std::vector<int>& actnum) const {
        size_t volume = getNX() * getNY() * getNZ();
        if (getNumActive() == volume)
            actnum.resize(0);
        else {
            actnum.resize( volume );
            ecl_grid_init_actnum_data( c_ptr() , actnum.data() );
        }
    }

    void EclipseGrid::exportMAPAXES( std::vector<double>& mapaxes) const {
        if (ecl_grid_use_mapaxes( c_ptr())) {
            mapaxes.resize(6);
            ecl_grid_init_mapaxes_data_double( c_ptr() , mapaxes.data() );
        } else {
            mapaxes.resize(0);
        }
    }

    void EclipseGrid::exportCOORD( std::vector<double>& coord) const {
        coord.resize( ecl_grid_get_coord_size( c_ptr() ));
        ecl_grid_init_coord_data_double( c_ptr() , coord.data() );
    }

    void EclipseGrid::exportZCORN( std::vector<double>& zcorn) const {
        zcorn.resize( ecl_grid_get_zcorn_size( c_ptr() ));
        ecl_grid_init_zcorn_data_double( c_ptr() , zcorn.data() );
    }



    void EclipseGrid::resetACTNUM( const int * actnum) {
        assertCellInfo();
        ecl_grid_reset_actnum( m_grid.get() , actnum );
    }


    void EclipseGrid::fwriteEGRID( const std::string& filename, bool output_metric ) const {
        assertCellInfo();
        ecl_grid_fwrite_EGRID( m_grid.get() , filename.c_str(), output_metric );
    }


}


