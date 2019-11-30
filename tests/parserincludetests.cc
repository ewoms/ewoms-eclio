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

#define BOOST_TEST_MODULE ParserTests
#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parserkeyword.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parsecontext.hh>

inline std::string prefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

BOOST_AUTO_TEST_CASE(ParserKeyword_includeInvalid) {
    boost::filesystem::path inputFilePath(prefix() + "includeInvalid.data");

    Ewoms::Parser parser;
    Ewoms::ParseContext parseContext;
    Ewoms::ErrorGuard errors;

    parseContext.update(Ewoms::ParseContext::PARSE_MISSING_INCLUDE , Ewoms::InputError::THROW_EXCEPTION );
    BOOST_CHECK_THROW(parser.parseFile(inputFilePath.string() , parseContext, errors) , std::invalid_argument);

    parseContext.update(Ewoms::ParseContext::PARSE_MISSING_INCLUDE , Ewoms::InputError::IGNORE );
    BOOST_CHECK_NO_THROW(parser.parseFile(inputFilePath.string() , parseContext, errors));
}

BOOST_AUTO_TEST_CASE(DATA_FILE_IS_SYMLINK) {
  boost::filesystem::path inputFilePath(prefix() + "includeSymlinkTestdata/symlink4/path/case.data");
  Ewoms::Parser parser;
  std::cout << "Input file: " << inputFilePath.string() << std::endl;
  auto deck = parser.parseFile(inputFilePath.string());

  BOOST_CHECK_EQUAL(true , deck.hasKeyword("OIL"));
  BOOST_CHECK_EQUAL(false , deck.hasKeyword("WATER"));
}

BOOST_AUTO_TEST_CASE(Verify_find_includes_Data_file_is_a_symlink) {
    boost::filesystem::path inputFilePath(prefix() + "includeSymlinkTestdata/symlink1/case_symlink.data");
    Ewoms::Parser parser;
    auto deck = parser.parseFile(inputFilePath.string());

    BOOST_CHECK_EQUAL(true , deck.hasKeyword("OIL"));
    BOOST_CHECK_EQUAL(false , deck.hasKeyword("WATER"));
}

BOOST_AUTO_TEST_CASE(Verify_find_includes_Data_file_has_include_that_is_a_symlink) {
    boost::filesystem::path inputFilePath(prefix() + "includeSymlinkTestdata/symlink2/caseWithIncludedSymlink.data");
    Ewoms::Parser parser;
    auto deck = parser.parseFile(inputFilePath.string());

    BOOST_CHECK_EQUAL(true , deck.hasKeyword("OIL"));
    BOOST_CHECK_EQUAL(false , deck.hasKeyword("WATER"));
}

BOOST_AUTO_TEST_CASE(Verify_find_includes_Data_file_has_include_file_that_again_includes_a_symlink) {
    boost::filesystem::path inputFilePath(prefix() + "includeSymlinkTestdata/symlink3/case.data");
    Ewoms::Parser parser;
    auto deck = parser.parseFile(inputFilePath.string());

    BOOST_CHECK_EQUAL(true , deck.hasKeyword("OIL"));
    BOOST_CHECK_EQUAL(false , deck.hasKeyword("WATER"));
}

BOOST_AUTO_TEST_CASE(ParserKeyword_includeValid) {
    boost::filesystem::path inputFilePath(prefix() + "includeValid.data");

    Ewoms::Parser parser;
    auto deck = parser.parseFile(inputFilePath.string());

    BOOST_CHECK_EQUAL(true , deck.hasKeyword("OIL"));
    BOOST_CHECK_EQUAL(false , deck.hasKeyword("WATER"));
}

BOOST_AUTO_TEST_CASE(ParserKeyword_includeWrongCase) {
    boost::filesystem::path inputFile1Path(prefix() + "includeWrongCase1.data");
    boost::filesystem::path inputFile2Path(prefix() + "includeWrongCase2.data");
    boost::filesystem::path inputFile3Path(prefix() + "includeWrongCase3.data");

    Ewoms::Parser parser;

#if HAVE_CASE_SENSITIVE_FILESYSTEM
    // so far, we expect the files which are included to exhibit
    // exactly the same spelling as their names on disk. Eclipse seems
    // to be a bit more relaxed when it comes to this, so we might
    // have to change the current behavior one not-so-fine day...
    Ewoms::ParseContext parseContext;
    Ewoms::ErrorGuard errors;
    parseContext.update(Ewoms::ParseContext::PARSE_MISSING_INCLUDE , Ewoms::InputError::THROW_EXCEPTION );

    BOOST_CHECK_THROW(parser.parseFile(inputFile1Path.string(), parseContext, errors), std::invalid_argument);
    BOOST_CHECK_THROW(parser.parseFile(inputFile2Path.string(), parseContext, errors), std::invalid_argument);
    BOOST_CHECK_THROW(parser.parseFile(inputFile3Path.string(), parseContext, errors), std::invalid_argument);
#else
    // for case-insensitive filesystems, the include statement will
    // always work regardless of how the capitalization of the
    // included files is wrong...
    BOOST_CHECK_EQUAL(true, parser.parseFile(inputFile1Path.string() ).hasKeyword("OIL"));
    BOOST_CHECK_EQUAL(false, parser.parseFile(inputFile1Path.string()).hasKeyword("WATER"));
    BOOST_CHECK_EQUAL(true, parser.parseFile(inputFile2Path.string() ).hasKeyword("OIL"));
    BOOST_CHECK_EQUAL(false, parser.parseFile(inputFile2Path.string()).hasKeyword("WATER"));
    BOOST_CHECK_EQUAL(true, parser.parseFile(inputFile3Path.string() ).hasKeyword("OIL"));
    BOOST_CHECK_EQUAL(false, parser.parseFile(inputFile3Path.string()).hasKeyword("WATER"));
#endif
}

