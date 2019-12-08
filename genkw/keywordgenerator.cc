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

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cctype>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <ewoms/eclio/json/jsonobject.hh>
#include <genkw/keywordgenerator.hh>
#include <genkw/keywordloader.hh>
#include <ewoms/eclio/parser/parserkeyword.hh>

namespace {

const std::string testHeader =
    "#define BOOST_TEST_MODULE ParserRecordTests\n"
    "#include <boost/filesystem.hpp>\n"
    "#include <boost/test/unit_test.hpp>\n"
    "#include <memory>\n"
    "#include <ewoms/eclio/json/jsonobject.hh>\n"
    "#include <ewoms/eclio/parser/parserkeywords.hh>\n"
    "#include <ewoms/eclio/parser/parserkeyword.hh>\n"
    "#include <ewoms/eclio/parser/parseritem.hh>\n"
    "#include <ewoms/eclio/parser/parserrecord.hh>\n"
    "#include <ewoms/eclio/parser/units/unitsystem.hh>\n"
    "using namespace Ewoms;\n"
    "auto unitSystem =  UnitSystem::newMETRIC();\n";
}

namespace Ewoms {

    KeywordGenerator::KeywordGenerator(bool verbose)
        : m_verbose( verbose )
    {
    }

    std::string KeywordGenerator::headerPrelude(const std::string& suffix) {
        std::string header = "#ifndef PARSER_KEYWORDS_" + boost::to_upper_copy(suffix) + "_HH\n"
            "#define PARSER_KEYWORDS_" + boost::to_upper_copy(suffix) + "_HH\n"
            "#include <ewoms/eclio/parser/parserkeyword.hh>\n"
            "namespace Ewoms {\n"
            "namespace ParserKeywords {\n\n";

        return header;
    }

    std::string KeywordGenerator::headerAllPrelude() {
        std::string header = "#ifndef PARSER_KEYWORDS_HH\n"
            "#define PARSER_KEYWORDS_HH\n"
            "#include <ewoms/eclio/parser/parserkeyword.hh>\n";

        return header;
    }

    std::string KeywordGenerator::sourcePrelude(const std::string& suffix) {
        std::string header =
            "#include <ewoms/eclio/parser/deck/udavalue.hh>\n"
            "#include <ewoms/eclio/parser/parserkeyword.hh>\n"
            "#include <ewoms/eclio/parser/parseritem.hh>\n"
            "#include <ewoms/eclio/parser/parserrecord.hh>\n"
            "#include <ewoms/eclio/parser/parser.hh>\n"
            "#include <ewoms/eclio/parser/parserkeywords/"+suffix+".hh>\n\n\n"
            "namespace Ewoms {\n"
            "namespace ParserKeywords {\n\n";

        return header;
    }

    std::string KeywordGenerator::sourceAllPrelude() {
        std::string header =
            "#include <ewoms/eclio/parser/deck/udavalue.hh>\n"
            "#include <ewoms/eclio/parser/parserkeyword.hh>\n"
            "#include <ewoms/eclio/parser/parseritem.hh>\n"
            "#include <ewoms/eclio/parser/parserrecord.hh>\n"
            "#include <ewoms/eclio/parser/parser.hh>\n"
            "#include <ewoms/eclio/parser/parserkeywords.hh>\n\n\n"
            "namespace Ewoms {\n";

        return header;
    }

    void KeywordGenerator::ensurePath( const std::string& file_name) {
        boost::filesystem::path file(file_name);
        if (!boost::filesystem::is_directory( file.parent_path()))
            boost::filesystem::create_directories( file.parent_path());
    }

    bool KeywordGenerator::updateFile(const std::stringstream& newContent , const std::string& filename) {
        ensurePath(filename);
        std::ofstream outputStream(filename);
        outputStream << newContent.str();

        return true;
    }

    static bool write_file( const std::stringstream& stream, const std::string& file, bool verbose, std::string desc = "source" ) {
        auto update = KeywordGenerator::updateFile( stream, file );
        if( !verbose ) return update;

        if( update )
            std::cout << "Updated " << desc << " file written to: " << file << std::endl;
        else
            std::cout << "No changes to " << desc << " file: " << file << std::endl;

        return update;
    }

    bool KeywordGenerator::updateSources(const KeywordLoader& loader, const std::string& sourceBuildPath, const std::string& sourceDir) const {
        std::map< char, std::vector< const ParserKeyword* > > keywords;
        for( auto iter = loader.keyword_begin(); iter != loader.keyword_end(); ++iter )
            keywords[ std::tolower( iter->second->className().at(0) ) ].push_back( iter->second.get() );

        for( const auto& iter : keywords ) {
            std::stringstream stream;

            stream << sourcePrelude( std::string( 1, std::tolower( iter.first ) ) );
            for( auto& kw : iter.second ) {
                stream << kw->createCode() << std::endl;
            }

            stream << "}" << std::endl << "}" << std::endl;

            const auto final_path = sourceBuildPath + sourceDir + "/" + std::string( 1, iter.first ) + ".cc";
            write_file( stream, final_path, m_verbose, "source" );
        }

        // generate the global "parserkeywords.cc" source file which registers all
        // keywords
        std::stringstream stream;

        stream << sourceAllPrelude() << std::endl;

        stream << "void Parser::addDefaultKeywords()"  << std::endl
               << "{" << std::endl;
        for( auto iter = loader.keyword_begin(); iter != loader.keyword_end(); ++iter ) {
            stream << "  this->addKeyword< ParserKeywords::"
                   << iter->second->className()
                   << " >();" << std::endl;
        }
        stream << "}" << std::endl;

        stream << "} // namespace Ewoms" << std::endl;

        const auto final_path = sourceBuildPath + sourceDir + "/../parserkeywords.cc";
        return write_file( stream, final_path, m_verbose, "source" );
    }

    bool KeywordGenerator::updateHeaders(const KeywordLoader& loader, const std::string& headersBuildPath, const std::string& headersDir) const {
        bool update = false;

        std::map< char, std::vector< const ParserKeyword* > > keywords;
        for( auto iter = loader.keyword_begin(); iter != loader.keyword_end(); ++iter )
            keywords[ std::tolower( iter->second->className().at(0) ) ].push_back( iter->second.get() );

        for( const auto& iter : keywords ) {
            std::stringstream stream;

            stream << headerPrelude( std::string( 1, std::tolower( iter.first ) ) );
            for( auto& kw : iter.second )
                stream << kw->createDeclaration("   ") << std::endl;

            stream << "}" << std::endl << "}" << std::endl;
            stream << "#endif" << std::endl;

            const auto final_path = headersBuildPath + headersDir + "/" + std::string( 1, iter.first ) + ".hh";
            if( write_file( stream, final_path, m_verbose, "header" ) )
                update = true;
        }

        std::stringstream stream;
        stream << headerAllPrelude();

        for( const auto& iter : keywords )
            stream << "#include <"
                << headersDir + "/"
                << std::string( 1, std::tolower( iter.first ) ) + ".hh>"
                << std::endl;

        stream << "#endif" << std::endl;

        const auto final_path = headersBuildPath + "/" + headersDir + "/../parserkeywords.hh";
        return write_file( stream, final_path, m_verbose, "header" ) || update;
    }

    std::string KeywordGenerator::startTest(const std::string& keyword_name) {
        return std::string("BOOST_AUTO_TEST_CASE(TEST") + keyword_name + std::string("Keyword) {\n");
    }

    std::string KeywordGenerator::endTest() {
        return "}\n\n";
    }

    bool KeywordGenerator::updateTest(const KeywordLoader& loader , const std::string& testFile) const {
        std::stringstream stream;

        stream << testHeader;
        for (auto iter = loader.keyword_begin(); iter != loader.keyword_end(); ++iter) {
            const std::string& keywordName = (*iter).first;
            std::shared_ptr<ParserKeyword> keyword = (*iter).second;
            stream << startTest(keywordName);
            stream << "    std::string jsonFile = \"" << loader.getJsonFile( keywordName) << "\";" << std::endl;
            stream << "    boost::filesystem::path jsonPath( jsonFile );" << std::endl;
            stream << "    Json::JsonObject jsonConfig( jsonPath );" << std::endl;
            stream << "    ParserKeyword jsonKeyword(jsonConfig);" << std::endl;
            stream << "    ParserKeywords::" << keywordName << " inlineKeyword;" << std::endl;
            stream << "    BOOST_CHECK_EQUAL( jsonKeyword, inlineKeyword );" << std::endl;
            stream << "    if (jsonKeyword.hasDimension()) {" <<std::endl;
            stream << "        const auto& parserRecord = jsonKeyword.getRecord(0);" << std::endl;
            stream << "        for (size_t i=0; i < parserRecord.size(); i++){" << std::endl;
            stream << "            const auto& item = parserRecord.get( i );" << std::endl;
            stream << "            for (const auto& dim : item.dimensions())" << std::endl;
            stream << "                BOOST_CHECK_NO_THROW( unitSystem.getNewDimension( dim ));" << std::endl;
            stream << "        }" << std::endl;
            stream << "    }" << std::endl;
            stream << endTest(  );
        }

        return updateFile( stream , testFile );
    }
}

