// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "config.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <tuple>
#include <getopt.h>
#include <sstream>
#include <stdexcept>

#include <ewoms/eclio/io/eclfile.hh>
#include <ewoms/eclio/io/ecloutput.hh>

static void printHelp() {

    std::cout << "\rewriteEclFile needs a minimum of one arguments which is the input file name.  \n"
              << "\nIn addition, the program takes these options (which must be given before the arguments):\n\n"
              << "-h Print help and exit.\n\n";
}

int main(int argc, char **argv) {

    int c                          = 0;

    while ((c = getopt(argc, argv, "h")) != -1) {
        switch (c) {
        case 'h':
            printHelp();
            return 0;
        default:
            return EXIT_FAILURE;
        }
    }

    int argOffset = optind;

    Ewoms::EclIO::EclFile reffile(argv[argOffset]);
    auto arrayList = reffile.getList();

    std::string outputFile=std::string(argv[argOffset]);

    int p1 = outputFile.find_last_of(".");
    std::string ext = outputFile.substr(p1+1);

    outputFile = outputFile.substr(0,p1) + "_REWRITE." + ext;
    Ewoms::EclIO::EclOutput outFile(outputFile, reffile.formattedInput());

    if (reffile.is_ix())
        outFile.set_ix();

    reffile.loadData();

    std::vector<int> elementSizeList = reffile.getElementSizeList();

    for (size_t n = 0; n < arrayList.size(); n++){

        std::string name = std::get<0>(arrayList[n]);
        auto arrType = std::get<1>(arrayList[n]);

        if (arrType == Ewoms::EclIO::INTE) {
            auto data = reffile.get<int>(n);
            outFile.write(name, data);
        } else if (arrType == Ewoms::EclIO::CHAR) {
            auto data = reffile.get<std::string>(n);
            outFile.write(name, data);
        } else if (arrType == Ewoms::EclIO::C0NN) {
            auto data = reffile.get<std::string>(n);
            outFile.write(name, data, elementSizeList[n]);
        } else if (arrType == Ewoms::EclIO::REAL) {
            auto data = reffile.get<float>(n);
            outFile.write(name, data);
        } else if (arrType == Ewoms::EclIO::DOUB) {
            auto data = reffile.get<double>(n);
            outFile.write(name, data);
        } else if (arrType == Ewoms::EclIO::LOGI) {
            auto data = reffile.get<bool>(n);
            outFile.write(name, data);
        } else if (arrType == Ewoms::EclIO::MESS) {
            outFile.message(name);
        }
    }

    return 0;
}
