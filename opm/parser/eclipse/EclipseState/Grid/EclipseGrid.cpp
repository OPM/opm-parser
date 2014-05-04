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

#include <boost/lexical_cast.hpp>

#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>

#include <ert/ecl/ecl_grid.h>
namespace Opm {

    
    EclipseGrid::EclipseGrid(std::shared_ptr<const RUNSPECSection> runspecSection, std::shared_ptr<const GRIDSection> gridSection) {
        if (runspecSection->hasKeyword("DIMENS")) {
            DeckKeywordConstPtr dimens = runspecSection->getKeyword("DIMENS");
            DeckRecordConstPtr record = dimens->getRecord(0);
            std::vector<int> dims = {record->getItem("NX")->getInt(0) , 
                                     record->getItem("NY")->getInt(0) , 
                                     record->getItem("NZ")->getInt(0) };

            if (hasCornerPointKeywords(gridSection)) {
                initCornerPointGrid(dims , gridSection);
            } else if (hasCartesianKeywords(gridSection)) {
                initCartesianGrid(dims , gridSection);
            } else
                throw std::invalid_argument("The GRID section must have COORD / ZCORN or D?? + TOPS keywords");
            
        } else
            throw std::invalid_argument("The RUNSPEC section must have the DIMENS keyword with grid dimensions");
    }

    bool EclipseGrid::equal(const EclipseGrid& other) const {
        return ecl_grid_compare( m_grid.get() , other.m_grid.get() , true , false );
    }


    int EclipseGrid::getNumActive( ) const {
        return ecl_grid_get_nactive( m_grid.get() );
    }

    int EclipseGrid::getNX( ) const {
        return ecl_grid_get_nx( m_grid.get() );
    }

    int EclipseGrid::getNY( ) const {
        return ecl_grid_get_ny( m_grid.get() );
    }

    int EclipseGrid::getNZ( ) const {
        return ecl_grid_get_nz( m_grid.get() );
    }

    
    bool EclipseGrid::hasCornerPointKeywords(std::shared_ptr<const GRIDSection> gridSection) {
        if (gridSection->hasKeyword("ZCORN") && gridSection->hasKeyword("COORD"))
            return true;
        else
            return false;
    }


    void EclipseGrid::assertCornerPointKeywords( const std::vector<int>& dims , std::shared_ptr<const GRIDSection> gridSection ) const 
    {
        const int nx = dims[0];
        const int ny = dims[1];
        const int nz = dims[2];
        {
            DeckKeywordConstPtr ZCORNKeyWord = gridSection->getKeyword("ZCORN");
            
            if (ZCORNKeyWord->getDataSize() != static_cast<size_t>(8*nx*ny*nz))
                throw std::invalid_argument("Wrong size in ZCORN keyword - expected 8*x*ny*nz = " + boost::lexical_cast<std::string>(8*nx*ny*nz));
        }

        {
            DeckKeywordConstPtr COORDKeyWord = gridSection->getKeyword("COORD");
            if (COORDKeyWord->getDataSize() != static_cast<size_t>(6*(nx + 1)*(ny + 1)))
                throw std::invalid_argument("Wrong size in COORD keyword - expected 6*(nx + 1)*(ny + 1) = " + boost::lexical_cast<std::string>(6*(nx + 1)*(ny + 1)));
        }

        if (gridSection->hasKeyword("ACTNUM")) {
            DeckKeywordConstPtr ACTNUMKeyWord = gridSection->getKeyword("ACTNUM");
            if (ACTNUMKeyWord->getDataSize() != static_cast<size_t>(nx*ny*nz))
                throw std::invalid_argument("Wrong size in ACTNUM keyword - expected 8*x*ny*nz = " + boost::lexical_cast<std::string>(nx*ny*nz));
        }
    }
        



    void EclipseGrid::initCornerPointGrid(const std::vector<int>& dims , std::shared_ptr<const GRIDSection> gridSection) {
        assertCornerPointKeywords( dims , gridSection );
        {
            DeckKeywordConstPtr ZCORNKeyWord = gridSection->getKeyword("ZCORN");
            DeckKeywordConstPtr COORDKeyWord = gridSection->getKeyword("COORD");
            const std::vector<double>& zcorn = ZCORNKeyWord->getSIDoubleData();
            const std::vector<double>& coord = COORDKeyWord->getSIDoubleData();
            const int * actnum = NULL;
            double    * mapaxes = NULL;
            
            if (gridSection->hasKeyword("ACTNUM")) {
                DeckKeywordConstPtr actnumKeyword = gridSection->getKeyword("ACTNUM");
                const std::vector<int>& actnumVector = actnumKeyword->getIntData();
                actnum = actnumVector.data();
                
            }
            
            if (gridSection->hasKeyword("MAPAXES")) {
                DeckKeywordConstPtr mapaxesKeyword = gridSection->getKeyword("MAPAXES");
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
    

    bool EclipseGrid::hasCartesianKeywords(std::shared_ptr<const GRIDSection> gridSection) {
        if (hasDVDEPTHZKeywords( gridSection ))
            return true;
        else
            return hasDTOPSKeywords(gridSection);
    }


    bool EclipseGrid::hasDVDEPTHZKeywords(std::shared_ptr<const GRIDSection> gridSection) {
        if (gridSection->hasKeyword("DXV") &&
            gridSection->hasKeyword("DYV") &&
            gridSection->hasKeyword("DZV") &&
            gridSection->hasKeyword("DEPTHZ"))
            return true;
        else
            return false;
    }

    bool EclipseGrid::hasDTOPSKeywords(std::shared_ptr<const GRIDSection> gridSection) {
        if ((gridSection->hasKeyword("DX") || gridSection->hasKeyword("DXV")) &&
            (gridSection->hasKeyword("DY") || gridSection->hasKeyword("DYV")) &&
            (gridSection->hasKeyword("DZ") || gridSection->hasKeyword("DZV")) &&
            gridSection->hasKeyword("TOPS"))
            return true;
        else
            return false;
    }


    void EclipseGrid::initCartesianGrid(const std::vector<int>& dims , std::shared_ptr<const GRIDSection> gridSection) {
        if (hasDVDEPTHZKeywords( gridSection ))
            initDVDEPTHZGrid( dims , gridSection );
        else if (hasDTOPSKeywords(gridSection))
            initDTOPSGrid( dims ,gridSection );
        else 
            throw std::invalid_argument("Tried to initialize cartesian grid without all required keywords");
    }
    
    void EclipseGrid::assertVectorSize(const std::vector<double>& vector , size_t expectedSize , const std::string& vectorName) {
        if (vector.size() != expectedSize)
            throw std::invalid_argument("Wrong size for keyword: " + vectorName + ". Expected: " + boost::lexical_cast<std::string>(expectedSize) + " got: " + boost::lexical_cast<std::string>(vector.size()));
    }

    
    void EclipseGrid::initDVDEPTHZGrid(const std::vector<int>& dims , std::shared_ptr<const GRIDSection> gridSection) {
        const std::vector<double>& DXV = gridSection->getKeyword("DXV")->getSIDoubleData();
        const std::vector<double>& DYV = gridSection->getKeyword("DYV")->getSIDoubleData();
        const std::vector<double>& DZV = gridSection->getKeyword("DZV")->getSIDoubleData();
        const std::vector<double>& DEPTHZ = gridSection->getKeyword("DEPTHZ")->getSIDoubleData();

        assertVectorSize( DEPTHZ , static_cast<size_t>( (dims[0] + 1)*(dims[1] +1 )) , "DEPTHZ");
        assertVectorSize( DXV    , static_cast<size_t>( dims[0] ) , "DXV");
        assertVectorSize( DYV    , static_cast<size_t>( dims[1] ) , "DYV");
        assertVectorSize( DZV    , static_cast<size_t>( dims[2] ) , "DZV");
        
        m_grid.reset( ecl_grid_alloc_dxv_dyv_dzv_depthz( dims[0] , dims[1] , dims[2] , DXV.data() , DYV.data() , DZV.data() , DEPTHZ.data() , NULL ) , ecl_grid_free);
    }


    void EclipseGrid::initDTOPSGrid(const std::vector<int>& dims , std::shared_ptr<const GRIDSection> gridSection) {
        std::vector<double> DX = createDVector( dims , 0 , "DX" , "DXV" , gridSection);
        std::vector<double> DY = createDVector( dims , 1 , "DY" , "DYV" , gridSection);
        std::vector<double> DZ = createDVector( dims , 2 , "DZ" , "DZV" , gridSection);
        std::vector<double> TOPS = createTOPSVector( dims , DZ , gridSection );
        
        m_grid.reset( ecl_grid_alloc_dx_dy_dz_tops( dims[0] , dims[1] , dims[2] , DX.data() , DY.data() , DZ.data() , TOPS.data() , NULL ) , ecl_grid_free);
    }

    
    std::vector<double> EclipseGrid::createTOPSVector(const std::vector<int>& dims , const std::vector<double>& DZ , std::shared_ptr<const GRIDSection> gridSection) {
        size_t volume = dims[0] * dims[1] * dims[2];
        size_t area = dims[0] * dims[1];
        DeckKeywordConstPtr TOPSKeyWord = gridSection->getKeyword("TOPS");
        std::vector<double> TOPS = TOPSKeyWord->getSIDoubleData();
        
        if (TOPS.size() >= area) {
            TOPS.resize( volume );
            for (size_t targetIndex = TOPS.size(); targetIndex < volume; targetIndex++) {
                size_t sourceIndex = targetIndex - area;
                TOPS[targetIndex] = TOPS[sourceIndex] + DZ[sourceIndex];
            }
        }
        
        if (TOPS.size() != volume)
            throw std::invalid_argument("TOPS size mismatch");

        return TOPS;
    }

    
    
    std::vector<double> EclipseGrid::createDVector(const std::vector<int>& dims , size_t dim , const std::string& DKey , const std::string& DVKey, std::shared_ptr<const GRIDSection> gridSection) {
        size_t volume = dims[0] * dims[1] * dims[2];
        size_t area = dims[0] * dims[1];
        std::vector<double> D;
        if (gridSection->hasKeyword(DKey)) {
            DeckKeywordConstPtr DKeyWord = gridSection->getKeyword(DKey);
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
            DeckKeywordConstPtr DVKeyWord = gridSection->getKeyword(DVKey);
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

    void EclipseGrid::exportACTNUM( std::vector<int>& actnum) const {
        int volume = getNX() * getNY() * getNZ();
        if (getNumActive() == volume)
            actnum.resize(0);
        else {
            actnum.resize( volume );
            ecl_grid_init_actnum_data( m_grid.get() , actnum.data() );
        }
    }

    void EclipseGrid::exportMAPAXES( std::vector<double>& mapaxes) const {
        if (ecl_grid_use_mapaxes( m_grid.get())) {
            mapaxes.resize(6);
            ecl_grid_init_mapaxes_data_double( m_grid.get() , mapaxes.data() );
        } else {
            mapaxes.resize(0);
        }
    }
        
    void EclipseGrid::exportCOORD( std::vector<double>& coord) const {
        coord.resize( ecl_grid_get_coord_size( m_grid.get() ));
        ecl_grid_init_coord_data_double( m_grid.get() , coord.data() );
    }

    void EclipseGrid::exportZCORN( std::vector<double>& zcorn) const {
        zcorn.resize( ecl_grid_get_zcorn_size( m_grid.get() ));
        ecl_grid_init_zcorn_data_double( m_grid.get() , zcorn.data() );
    }



}


