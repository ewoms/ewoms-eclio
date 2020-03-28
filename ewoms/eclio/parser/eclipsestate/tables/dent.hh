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
#ifndef EWOMS_PARSER_DENT_H
#define EWOMS_PARSER_DENT_H

#include <cstddef>
#include <vector>

namespace Ewoms {

    class DeckKeyword;
    class DeckRecord;

    class DenT {
    public:

        struct entry {
            double T0;
            double C1;
            double C2;

            entry() = default;
            entry(double T0_, double C1_, double C2_);
            explicit entry(const DeckRecord& record);
            bool operator==(const entry& other) const;

            template<class Serializer>
            void serializeOp(Serializer& serializer)
            {
                serializer(T0);
                serializer(C1);
                serializer(C2);
            }
        };

        DenT() = default;
        explicit DenT(const DeckKeyword& keyword);

        static DenT serializeObject();

        const entry& operator[](const std::size_t index) const;
        bool operator==(const DenT& other) const;
        std::size_t size() const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer.vector(m_records);
        }

    private:
        std::vector<entry> m_records;
    };
}

#endif
