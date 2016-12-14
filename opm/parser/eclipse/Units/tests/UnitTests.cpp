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

#define BOOST_TEST_MODULE UnitTests

#include <opm/parser/eclipse/Units/UnitSystem.hpp>
#include <opm/parser/eclipse/Units/Dimension.hpp>
#include <opm/parser/eclipse/Units.hpp>

#include <boost/test/unit_test.hpp>

#include <memory>
#include <ostream>

using namespace Opm;

BOOST_AUTO_TEST_CASE(CreateDimension) {
    Dimension length("Length" , 1);
    BOOST_CHECK_EQUAL("Length" , length.getName());
    BOOST_CHECK_EQUAL(1 , length.getSIScaling());
}

BOOST_AUTO_TEST_CASE(makeComposite) {
    auto composite = Dimension::newComposite("Length*Length*Length/Time" , 100);
    BOOST_CHECK_EQUAL("Length*Length*Length/Time" , composite.getName());
    BOOST_CHECK_EQUAL(100 , composite.getSIScaling());
}


BOOST_AUTO_TEST_CASE(CreateDimensionInvalidNameThrows) {
    BOOST_CHECK_THROW(Dimension(" " , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension(".LX" , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension("*" , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension("/" , 1) , std::invalid_argument);
    BOOST_CHECK_THROW(Dimension("2" , 1) , std::invalid_argument);
    BOOST_CHECK_NO_THROW(Dimension("1" , 1));
}


BOOST_AUTO_TEST_CASE(CreateUnitSystem) {
    UnitSystem system(UnitSystem::UnitType::UNIT_TYPE_METRIC);
    BOOST_CHECK_EQUAL("Metric" , system.getName());
}


BOOST_AUTO_TEST_CASE(UnitSystemEmptyHasNone) {
    UnitSystem system(UnitSystem::UnitType::UNIT_TYPE_METRIC);
    BOOST_CHECK_EQUAL( false , system.hasDimension("Length"));
    BOOST_CHECK_EQUAL( false , system.hasDimension("LXY"));
}



BOOST_AUTO_TEST_CASE(UnitSystemGetMissingDimensionThrows) {
    UnitSystem system(UnitSystem::UnitType::UNIT_TYPE_METRIC);
    BOOST_CHECK_THROW( system.getDimension("Length") , std::out_of_range );
}

BOOST_AUTO_TEST_CASE(UnitSystemGetNewOK) {
    UnitSystem system(UnitSystem::UnitType::UNIT_TYPE_METRIC);
    system.addDimension("Length" , 10 );
    system.addDimension("Time" , 100);

    BOOST_CHECK( !system.hasDimension("Length*Length/Time"));
    Dimension comp = system.getNewDimension( "Length*Length/Time" );
    BOOST_CHECK( system.hasDimension("Length*Length/Time"));
    BOOST_CHECK_EQUAL(1 , comp.getSIScaling());
}




BOOST_AUTO_TEST_CASE(UnitSystemAddDimensions) {
    UnitSystem system(UnitSystem::UnitType::UNIT_TYPE_METRIC);
    system.addDimension("Length" , 1 );
    system.addDimension("Time" , 86400 );
    system.addDimension("Temperature", 1.0, 273.15);

    auto length = system.getDimension( "Length" );
    auto time = system.getDimension( "Time" );
    auto temperature = system.getDimension( "Temperature" );
    BOOST_CHECK_EQUAL(1     , length.getSIScaling());
    BOOST_CHECK_EQUAL(86400 , time.getSIScaling());
    BOOST_CHECK_EQUAL(1.0   , temperature.getSIScaling());
    BOOST_CHECK_EQUAL(273.15, temperature.getSIOffset());

    system.addDimension("Length" , 0.3048);
    length = system.getDimension("Length");
    BOOST_CHECK_EQUAL(0.3048 , length.getSIScaling());
}


BOOST_AUTO_TEST_CASE(UnitSystemParseInvalidThrows) {
    UnitSystem system(UnitSystem::UnitType::UNIT_TYPE_METRIC);
    BOOST_CHECK_THROW( system.parse("//") , std::invalid_argument);
    BOOST_CHECK_THROW( system.parse("Length * Length / Time") , std::out_of_range );

    system.addDimension("Length" , 3.00 );
    system.addDimension("Time" , 9.0 );

    auto volumePerTime = system.parse( "Length*Length*Length/Time" );
    BOOST_CHECK_EQUAL("Length*Length*Length/Time" , volumePerTime.getName() );
    BOOST_CHECK_EQUAL(3.0 , volumePerTime.getSIScaling());
}



static void checkSystemHasRequiredDimensions( const UnitSystem& system) {
    BOOST_CHECK( system.hasDimension("1"));
    BOOST_CHECK( system.hasDimension("Length"));
    BOOST_CHECK( system.hasDimension("Mass"));
    BOOST_CHECK( system.hasDimension("Time"));
    BOOST_CHECK( system.hasDimension("Permeability"));
    BOOST_CHECK( system.hasDimension("Pressure"));
    BOOST_CHECK( system.hasDimension("Temperature"));
}



BOOST_AUTO_TEST_CASE(CreateMetricSystem) {
    auto system = UnitSystem::newMETRIC();
    checkSystemHasRequiredDimensions( system );

    BOOST_CHECK_EQUAL( Metric::Length       , system.getDimension("Length").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Mass         , system.getDimension("Mass").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Time         , system.getDimension("Time").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Permeability , system.getDimension("Permeability").getSIScaling() );
    BOOST_CHECK_EQUAL( Metric::Pressure     , system.getDimension("Pressure").getSIScaling() );
}



BOOST_AUTO_TEST_CASE(CreateFieldSystem) {
    auto system = UnitSystem::newFIELD();
    checkSystemHasRequiredDimensions( system );

    BOOST_CHECK_EQUAL( Field::Length       , system.getDimension("Length").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Mass         , system.getDimension("Mass").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Time         , system.getDimension("Time").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Permeability , system.getDimension("Permeability").getSIScaling() );
    BOOST_CHECK_EQUAL( Field::Pressure     , system.getDimension("Pressure").getSIScaling() );
}




BOOST_AUTO_TEST_CASE(DimensionEqual) {
    Dimension d1("Length" , 1);
    Dimension d2("Length" , 1);
    Dimension d3("Time" , 1);
    Dimension d4("Length" , 2);

    BOOST_CHECK_EQUAL( true  , d1.equal(d1) );
    BOOST_CHECK_EQUAL( true  , d1.equal(d2) );
    BOOST_CHECK_EQUAL( false , d1.equal(d3) );
    BOOST_CHECK_EQUAL( false , d1.equal(d4) );
}

namespace Opm {
inline std::ostream& operator<<( std::ostream& stream, const UnitSystem& us ) {
    return stream << us.getName() << " :: " << static_cast<int>(us.getType());
}
}


BOOST_AUTO_TEST_CASE(UnitSystemEqual) {
    auto metric1 = UnitSystem::newMETRIC();
    auto metric2 = UnitSystem::newMETRIC();
    auto field = UnitSystem::newFIELD();

    BOOST_CHECK_EQUAL( metric1, metric1 );
    BOOST_CHECK_EQUAL( metric1, metric2 );
    BOOST_CHECK_NE( metric1, field );
    metric1.addDimension("g" , 3.00 );
    BOOST_CHECK_NE( metric1, metric2 );
    BOOST_CHECK_NE( metric2, metric1 );
}



BOOST_AUTO_TEST_CASE(LabUnitConversions) {
    using Meas = UnitSystem::measure;

    auto lab = UnitSystem::newLAB();

    {
        const auto furlong = 660*unit::feet;
        BOOST_CHECK_CLOSE( 2.01168e4 , lab.from_si( Meas::length , furlong ) , 1.0e-10 );
        BOOST_CHECK_CLOSE( furlong   , lab.to_si( Meas::length , 2.01168e4 ) , 1.0e-10 );
    }

    struct Factor { Meas m; double f; };

    for (const auto& q : { Factor{ Meas::density               , 1.0e3  }   ,
                           Factor{ Meas::pressure              , 101325.0 } ,
                           Factor{ Meas::viscosity             , 1.0e-3 }   ,
                           Factor{ Meas::liquid_surface_volume , 1.0e-6 }   ,
                           Factor{ Meas::gas_surface_volume    , 1.0e-6 }   ,
                           Factor{ Meas::time                  , 3600.0 }   ,
                           Factor{ Meas::mass                  , 1.0e-3 }   })
    {
        BOOST_CHECK_CLOSE( q.f , lab.to_si( q.m , 1.0 )   , 1.0e-10 );
        BOOST_CHECK_CLOSE( 1.0 , lab.from_si( q.m , q.f ) , 1.0e-10 );
    }
}


BOOST_AUTO_TEST_CASE( VectorConvert ) {
    std::vector<double> d0 = {1,2,3};
    std::vector<double> d1 = {1,2,3};
    UnitSystem units = UnitSystem::newLAB();

    units.from_si( UnitSystem::measure::pressure , d0 );
    for (size_t i = 0; i < d1.size(); i++)
        BOOST_CHECK_EQUAL( units.from_si( UnitSystem::measure::pressure , d1[i] ) , d0[i]);
}

BOOST_AUTO_TEST_CASE( GasOilRatioNotIdentityForField ) {
    const double gas = 14233.4;
    const double oil = 4223;

    const double ratio = gas / oil;
    const auto units = UnitSystem::newFIELD();

    const auto field_gas = (gas * 35.314666721) / 1000;
    const auto field_oil = oil * 6.28981100;

    const auto gor = UnitSystem::measure::gas_oil_ratio;

    BOOST_CHECK_CLOSE( units.from_si( gor, ratio ), field_gas / field_oil, 1e-5 );
}

BOOST_AUTO_TEST_CASE ( UnitConstants ) {
    using namespace Opm::prefix;
    using namespace Opm::unit;

    BOOST_REQUIRE_EQUAL (meter, 1);
    BOOST_REQUIRE_EQUAL (kilogram, 1);
    BOOST_REQUIRE_EQUAL (second, 1);

    BOOST_REQUIRE_CLOSE (milli*darcy, 9.86923667e-16, 0.01);
    BOOST_REQUIRE_CLOSE (mega*darcy, 9.86923e-7, 0.01);
    BOOST_REQUIRE_CLOSE (convert::to(mega*darcy, milli*darcy), 1e9, 0.01);

    BOOST_REQUIRE_CLOSE (convert::to(convert::from(1.0, barsa), psia), 14.5038, 0.01);
    BOOST_REQUIRE_CLOSE (convert::to(1*atm, barsa), 1.01325, 0.01);

    const double flux_SI = 10000*cubic(meter)/year;
    BOOST_REQUIRE_CLOSE (flux_SI, 3.17098e-4, 0.01);
    const double flux_m3py = convert::to(flux_SI, cubic(meter)/year);
    BOOST_REQUIRE_CLOSE (flux_m3py, 1e4, 0.01);
}
