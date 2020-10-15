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

#ifndef RECORD_H
#define RECORD_H

#include <deque>
#include <memory>
#include <string>
#include <ewoms/common/string_view.hh>
#include <list>

namespace Ewoms {
class KeywordLocation;

    /// Class representing the lowest level of the Raw datatypes, a record. A record is simply
    /// a vector containing the record elements, represented as strings. Some logic is present
    /// to handle special elements in a record string, particularly with quote characters.

    class RawRecord {
    public:
        RawRecord( const Ewoms::string_view&, const KeywordLocation&, bool text);
        explicit RawRecord( const Ewoms::string_view&, const KeywordLocation&);

        inline Ewoms::string_view pop_front();
        inline Ewoms::string_view front() const;
        void push_front( Ewoms::string_view token, std::size_t count );
        inline size_t size() const;
        std::size_t max_size() const;

        std::string getRecordString() const;
        inline Ewoms::string_view getItem(size_t index) const;

    private:
        Ewoms::string_view m_sanitizedRecordString;
        std::deque< Ewoms::string_view > m_recordItems;
        std::size_t m_max_size;
    };

    /*
     * These are frequently called, but fairly trivial in implementation, and
     * inlining the calls gives a decent low-effort performance benefit.
     */
    Ewoms::string_view RawRecord::pop_front() {
        auto front = m_recordItems.front();
        this->m_recordItems.pop_front();
        return front;
    }

    Ewoms::string_view RawRecord::front() const {
        return this->m_recordItems.front();
    }

    size_t RawRecord::size() const {
        return m_recordItems.size();
    }

    Ewoms::string_view RawRecord::getItem(size_t index) const {
        return this->m_recordItems.at( index );
    }
}

#endif  /* RECORD_HH */

