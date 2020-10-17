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

#include <cstdlib>

#include <iostream>
#include <fstream>
#include <locale>

#include <genkw/keywordgenerator.hh>
#include <genkw/keywordloader.hh>

int main(int , char ** argv) {
    const char * keyword_list_file = argv[1];
    const char * source_file_path = argv[2];
    const char * init_file_name = argv[3];
    const char * header_file_base_path = argv[4];
    const char * header_file_path = argv[5];

    std::vector<std::string> keyword_list;
    {
        std::string buffer;
        std::ifstream is(keyword_list_file);
        std::getline( is , buffer );
        is.close();

        size_t start = 0;
        while (true) {
            size_t end = buffer.find( ";" , start);
            if (end == std::string::npos) {
                keyword_list.push_back( buffer.substr(start) );
                break;
            }

            keyword_list.push_back( buffer.substr(start, end - start ));
            start = end + 1;
        }
    }
    Ewoms::KeywordLoader loader( keyword_list, false );
    Ewoms::KeywordGenerator generator( true );

    generator.updateKeywordSource(loader , source_file_path );
    generator.updateInitSource(loader , init_file_name );
    generator.updateHeader(loader, header_file_base_path, header_file_path );
}
