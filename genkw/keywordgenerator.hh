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

#ifndef KEYWORD_GENERATOR_H
#define KEYWORD_GENERATOR_H

#include <string>

namespace Ewoms {

    class KeywordLoader;

    class KeywordGenerator {

    public:
        KeywordGenerator(bool verbose);

        static void ensurePath( const std::string& file_name);
        static std::string endTest();
        static std::string startTest(const std::string& test_name);
        static std::string headerPrelude( const std::string& );
        static std::string headerAllPrelude();
        static std::string sourcePrelude(const std::string& suffix);
        static std::string sourceAllPrelude();
        static bool updateFile(const std::stringstream& newContent, const std::string& filename);

        bool updateSources(const KeywordLoader& loader, const std::string& sourceBuildPath, const std::string& sourceDir) const;
        bool updateHeaders(const KeywordLoader& loader, const std::string& headerBuildPath, const std::string& headerDir) const;
        bool updateTest(const KeywordLoader& loader , const std::string& testFile) const;

    private:
        bool m_verbose;
    };
}

#endif
