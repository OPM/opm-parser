#define BOOST_TEST_MODULE ParserIntegrationTests
#include <math.h>

#include <opm/common/utility/platform_dependent/disable_warnings.h>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include <opm/common/utility/platform_dependent/reenable_warnings.h>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckDoubleItem.hpp>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseMode.hpp>
#include <opm/parser/eclipse/Parser/ParserIntItem.hpp>
#include <opm/parser/eclipse/Parser/ParserDoubleItem.hpp>

#include <opm/parser/eclipse/EclipseState/Tables/SlgofTable.hpp>

using namespace Opm;

// the data which ought to be parsed
const char *parserData =
    "TABDIMS\n"
    "-- NTSFUN NTPVT NSSFUN NPPVT NTFIP NRPVT\n"
    "        1     1     30     1     1     1 /\n"
    "\n"
    "--  S_l k_rg k_rog p_cog\n"
    "SLGOF\n"
    "    0.1 1.0 0.0 9.0\n"
    "    0.2 0.9 0.2 8.0\n"
    "    0.3 0.8 0.3 7.0\n"
    "    0.4 0.7 0.3 6.0\n"
    "    0.5 0.6 0.4 5.0\n"
    "    0.6 0.5 0.5 4.0\n"
    "    0.7 0.3 0.8 3.0\n"
    "    0.8 0.2 0.9 2.0\n"
    "    0.9 0.1 1.0 1.0\n"
    "    1.0 0.0 1.0 0.0 /\n";

static void check_parser(ParserPtr parser) {
    DeckPtr deck =  parser->parseString(parserData, ParseMode());
    DeckKeywordConstPtr kw1 = deck->getKeyword("SLGOF");
    BOOST_CHECK_EQUAL(1U , kw1->size());

    DeckRecordConstPtr record0 = kw1->getRecord(0);
    BOOST_CHECK_EQUAL(1U , record0->size());

    DeckItemConstPtr item0 = record0->getItem(0);
    BOOST_CHECK_EQUAL(10U * 4, item0->size());
}

static void check_SlgofTable(ParserPtr parser) {
    DeckPtr deck =  parser->parseString(parserData, ParseMode());
    Opm::SlgofTable slgofTable(deck->getKeyword("SLGOF")->getRecord(0)->getItem(0));

    BOOST_CHECK_EQUAL(10U, slgofTable.getSlColumn().size());
    BOOST_CHECK_EQUAL(0.1, slgofTable.getSlColumn()[0]);
    BOOST_CHECK_EQUAL(1.0, slgofTable.getSlColumn()[9]);
    BOOST_CHECK_EQUAL(0.0, slgofTable.getKrgColumn()[9]);
    BOOST_CHECK_EQUAL(1.0, slgofTable.getKrogColumn()[9]);
    BOOST_CHECK_EQUAL(0.0, slgofTable.getPcogColumn()[9]);
}

BOOST_AUTO_TEST_CASE( parse_SLGOF_OK ) {
    ParserPtr parser(new Parser(/*addDefault=*/true));

    check_parser( parser );
    check_SlgofTable(parser);
}
