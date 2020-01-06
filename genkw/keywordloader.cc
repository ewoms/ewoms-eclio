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

#include <stdexcept>
#include <iostream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <ewoms/eclio/json/jsonobject.hh>
#include <genkw/keywordloader.hh>
#include <ewoms/eclio/parser/parserkeyword.hh>

namespace Ewoms {

    KeywordLoader::KeywordLoader(const std::vector<std::string>& keyword_files, bool verbose)
    {
        std::map<std::string, ParserKeyword> keyword_map;
        for (const auto& keyword_file : keyword_files) {
            if (verbose)
                std::cout << "Loading keyword from file: " << keyword_file << std::endl;

            boost::filesystem::path path( keyword_file );
            std::unique_ptr<ParserKeyword> parserKeyword;

            try {
                Json::JsonObject jsonConfig = Json::JsonObject( path );
                parserKeyword.reset( new ParserKeyword(jsonConfig) );
                boost::filesystem::path abs_path = boost::filesystem::absolute( path );
            } catch (const std::exception& exc) {
                std::cerr << std::endl;
                std::cerr << "Failed to create parserkeyword from: " << path.string() << std::endl;
                std::cerr << exc.what() << std::endl;
                std::cerr << std::endl;
                throw;
            }

            const std::string name = parserKeyword->getName();
            if (keyword_map.find(name) != keyword_map.end()) {
                this->m_jsonFile.erase(name);
                keyword_map.erase(name);
            }

            this->m_jsonFile.insert( std::pair<std::string , std::string> ( name , keyword_file));
            keyword_map.insert( std::pair<std::string , ParserKeyword> ( name , std::move(*parserKeyword)));
        }

        this->keywords['Y'] = {};
        this->keywords['X'] = {};
        for (const auto& kw_pair : keyword_map) {
            const auto& name = kw_pair.first;
            const auto& parserKeyword = kw_pair.second;
            this->keywords[name[0]].push_back( std::move(parserKeyword) );
        }
    }

    std::string KeywordLoader::getJsonFile(const std::string& keyword) const {
        auto iter = m_jsonFile.find( keyword );
        if (iter == m_jsonFile.end())
            throw std::invalid_argument("Keyword " + keyword + " not loaded");
        else
            return (*iter).second;
    }

    std::map<char, std::vector<ParserKeyword >>::const_iterator KeywordLoader::begin( ) const {
        return this->keywords.begin( );
    }

    std::map<char , std::vector<ParserKeyword >>::const_iterator KeywordLoader::end( ) const {
        return this->keywords.end( );
    }
}

