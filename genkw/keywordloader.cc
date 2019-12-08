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

    KeywordLoader::KeywordLoader(bool verbose)
        : m_verbose(verbose)
    {
    }

    size_t KeywordLoader::size() const {
        return m_keywords.size();
    }

    bool KeywordLoader::hasKeyword(const std::string& keyword) const {
        if (m_keywords.find( keyword ) == m_keywords.end())
            return false;
        else
            return true;
    }

    std::shared_ptr<const ParserKeyword> KeywordLoader::getKeyword(const std::string& keyword) const {
        auto iter = m_keywords.find( keyword );
        if (iter == m_keywords.end())
            throw std::invalid_argument("Keyword " + keyword + " not loaded");
        else
            return (*iter).second;
    }

    std::string KeywordLoader::getJsonFile(const std::string& keyword) const {
        auto iter = m_jsonFile.find( keyword );
        if (iter == m_jsonFile.end())
            throw std::invalid_argument("Keyword " + keyword + " not loaded");
        else
            return (*iter).second;
    }

    void KeywordLoader::loadKeyword(boost::filesystem::path& path) {
        std::shared_ptr<Json::JsonObject> jsonConfig = std::make_shared<Json::JsonObject>( path );
        try {
            std::shared_ptr<ParserKeyword> parserKeyword = std::make_shared<ParserKeyword>(*jsonConfig);
            boost::filesystem::path abs_path = boost::filesystem::absolute( path );
            addKeyword( parserKeyword , abs_path.generic_string() );
        } catch (const std::exception& exc) {
            std::cerr << std::endl;
            std::cerr << "Failed to create parserkeyword from: " << path.string() << std::endl;
            std::cerr << exc.what() << std::endl;
            std::cerr << std::endl;
            throw;
        }
    }

    void KeywordLoader::loadKeyword(const std::string& filename) {
        boost::filesystem::path path( filename );
        if (m_verbose)
            std::cout << "Loading keyword from file: " << filename << std::endl;
        return loadKeyword( path );
    }

    void KeywordLoader::addKeyword(std::shared_ptr<ParserKeyword> keyword , const std::string& jsonFile) {
        const std::string& name = keyword->getName();

        if (hasKeyword(name)) {
            m_keywords[name] = keyword;
            m_jsonFile[name] = jsonFile;
        } else {
            m_keywords.insert( std::pair<std::string , std::shared_ptr<ParserKeyword> > (name , keyword) );
            m_jsonFile.insert( std::pair<std::string , std::string> ( name , jsonFile));
        }
    }

    std::map<std::string , std::shared_ptr<ParserKeyword> >::const_iterator KeywordLoader::keyword_begin( ) const {
        return m_keywords.begin( );
    }

    std::map<std::string , std::shared_ptr<ParserKeyword> >::const_iterator KeywordLoader::keyword_end( ) const {
        return m_keywords.end( );
    }

}

