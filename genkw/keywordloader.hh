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
#ifndef KEYWORD_LOADER_H
#define KEYWORD_LOADER_H

#include <map>
#include <string>
#include <vector>

#include <ewoms/eclio/parser/parserkeyword.hh>

namespace Ewoms {

    class KeywordLoader {

    public:
        KeywordLoader(const std::vector<std::string>& keyword_files, bool verbose);
        std::string getJsonFile(const std::string& keyword) const;

        std::map<char , std::vector<ParserKeyword> >::const_iterator begin( ) const;
        std::map<char , std::vector<ParserKeyword> >::const_iterator end( ) const;
    private:
        std::map<char, std::vector<ParserKeyword>> keywords;
        std::map<std::string , std::string > m_jsonFile;
    };
}

#endif
