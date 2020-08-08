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

#ifndef RAWKEYWORD_H
#define RAWKEYWORD_H

#include <memory>
#include <string>
#include <vector>
#include <cstddef>

#include <ewoms/eclio/opmlog/location.hh>

#include <ewoms/eclio/parser/rawdeck/rawenums.hh>
#include <ewoms/eclio/parser/rawdeck/rawconsts.hh>

namespace Ewoms {

    class RawRecord;
    class RawKeyword {
    public:
        RawKeyword(const std::string& name, const std::string& filename, std::size_t lineNR, bool raw_string, Raw::KeywordSizeEnum sizeType);
        RawKeyword(const std::string& name, const std::string& filename, std::size_t lineNR, bool raw_string, Raw::KeywordSizeEnum sizeType, std::size_t size_arg);
        bool terminateKeyword();
        bool addRecord(RawRecord record);

        const std::string& getKeywordName() const;
        Raw::KeywordSizeEnum getSizeType() const;

        // Special case method only for inspecting INCLUDE keywords;
        // the general getRecords functionality should use the
        // iterator interface.
        const RawRecord& getFirstRecord( ) const;

        bool isFinished() const;
        bool unKnownSize() const;
        bool rawStringKeyword() const;
        const Location& location() const;

        using const_iterator = std::vector< RawRecord >::const_iterator;
        using iterator = std::vector< RawRecord >::iterator;

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
        std::size_t size() const;
    private:
        std::string m_name;
        Location m_location;
        bool raw_string_keyword;
        Raw::KeywordSizeEnum m_sizeType;

        size_t m_fixedSize = 0;
        size_t m_numTables = 0;
        size_t m_currentNumTables = 0;
        bool m_isTempFinished = false;
        bool m_isFinished = false;

        std::vector< RawRecord > m_records;
    };
}
#endif  /* RAWKEYWORD_HH */

