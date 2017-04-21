#define BOOST_TEST_MODULE EclipseGridTests

#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>

#include <opm/parser/eclipse/EclipseState/saturation.hpp>

using namespace Opm;

const std::string base_input = R"(
RUNSPEC

METRIC

OIL
GAS
WATER

-- three saturation tables to verify SATNUM region specific modifications of
-- curves
TABDIMS
    3 / 

DIMENS
    7 3 5 / -- grid of distinct, all-prime dimensions, total of 105 cells

ENDSCALE
    /

PROPS

SWL
    105*0.2 /

SWU
    105*0.9 /

REGIONS

)";

const std::string base_input_end = R"(

SATNUM -- evenly divided regions
    35*1 35*2 35*3 /

SOLUTION

SCHEDULE

)";

const std::string familyI = R"(

SWOF
--  Sw      Krw     Krow     Pcow
    0.1     0       1.0      2.0
    0.15    0       0.9      1.0
    0.2     0.01    0.5      0.5
    0.93    0.91    0.0      0.0
    /
    0.00    0       1.0      2.0
    0.05    0.01    1.0      2.0
    0.15    0.03    0.5      0.5
    0.852   1.00    0.0      0.0
    /
    0.00    0.00    0.9      2.0
    0.05    0.02    0.8      1.0
    0.10    0.03    0.5      0.5
    0.801   1.00    0.0      0.0
    /

SGOF
--  Sg      Krg     Kro     Pcog
    0.00    0.00    0.9     2.0
    0.05    0.02    0.8     1.0
    0.10    0.03    0.5     0.5
    0.80    1.00    0.0     0.0
    /
    0.05    0.00    1.0     2
    0.10    0.02    0.9     1
    0.15    0.03    0.5     0.5
    0.85    1.00    0.0     0
    /
    0.1     0       1.0     2
    0.15    0       0.9     1
    0.2     0.01    0.5     0.5
    0.9     0.91    0.0     0
    /
)";

const std::string familyII = R"(
SWFN
--  Sw      Krw     Pcow
    0.1     0       2.0
    0.15    0       1.0
    0.2     0.01    0.5
    0.93    0.91    0.0
    /
    0.00    0       2.0
    0.05    0.01    2.0
    0.15    0.03    0.5
    0.852   1.00    0.0
    /
    0.00    0.00    2.0
    0.05    0.02    1.0
    0.10    0.03    0.5
    0.801   1.00    0.0
    /

SGFN
--  Sg      Krg     Pcog
    0.00    0.00    2.0
    0.05    0.02    1.0
    0.10    0.03    0.5
    0.80    1.00    0.0
    /
    0.05    0.00    2
    0.10    0.02    1
    0.15    0.03    0.5
    0.85    1.00    0
    /
    0.1     0       2
    0.15    0       1
    0.2     0.01    0.5
    0.9     0.91    0
    /

SOF3
--  So      Krow    Krog
    0.07    0.0     0.0
    0.8     0.5     0.5
    0.85    0.9     1.0
    0.9     1.0     2.0
    /
    0.148   0.0     0.0
    0.85    0.5     0.5
    0.05    1.0     2.0
    1.00    1.0     2.0
    /
    0.199   0.0     0.0
    0.90    0.5     0.5
    0.95    0.8     1.0
    1.00    0.9     2.0
    /
)";


BOOST_AUTO_TEST_CASE( testsat ) {
    Parser parser;
    const auto inputI  = base_input + familyI  + base_input_end;
    const auto inputII = base_input + familyII + base_input_end;
    auto deckI = parser.parseString( inputI, ParseContext{} );
    auto deckII = parser.parseString( inputII, ParseContext{} );

    saturation satI( deckI, 3 * 5 * 7 );
    saturation satII( deckII, 3 * 5 * 7 );
    BOOST_CHECK( satI == satII );

    BOOST_CHECK_CLOSE( 1, satI.oil_relperm( 0, 0.24217 ), 1e-5 );
}
