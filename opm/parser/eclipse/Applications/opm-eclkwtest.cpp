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

#include <iostream>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParserLog.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>


static void printDeckDiagnostics(Opm::DeckConstPtr deck, Opm::ParserLogConstPtr parserLog, bool printAllKeywords) {
    int recognizedKeywords = 0;
    int unrecognizedKeywords = 0;
    
    for (size_t i = 0; i < deck->size(); i++) {
        if (!deck->getKeyword(i)->isKnown()) 
            unrecognizedKeywords++;
        else
            recognizedKeywords++;

        if (printAllKeywords) {
            std::cout << "Keyword (" << i << "): " << deck->getKeyword(i)->name() << " " << std::endl;
        }
    }
    {
        for (size_t iw = 0; iw < parserLog->size(); iw++) {
            std::cout << parserLog->getFormattedMessage(iw) << "\n";
        }
    }
    std::cout << "Total number of log messages:    " << parserLog->size() << std::endl;
    std::cout << "Number of recognized keywords:   " << recognizedKeywords << std::endl;
    std::cout << "Number of unrecognized keywords: " << unrecognizedKeywords << std::endl;
    std::cout << "Total number of keywords:        " << deck->size() << std::endl;

}
/*
 * 
 */
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <Filename>" << "[-l] (list keywords)" << std::endl;
        exit(1);
    }

    bool printKeywords = false;
    if (argc == 3) {
        std::string arg(argv[2]);
        if (arg == "-l")
            printKeywords = true;
    }

    Opm::ParserPtr parser(new Opm::Parser());
    std::string file = argv[1];
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);
    Opm::DeckConstPtr deck = parser->parseFile(file, parserLog);

    printDeckDiagnostics(deck, parserLog, printKeywords);

    return 0;
}

