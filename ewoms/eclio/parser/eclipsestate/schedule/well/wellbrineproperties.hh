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

#ifndef EWOMS_WELLBRINEPROPERTIES_H
#define EWOMS_WELLBRINEPROPERTIES_H

namespace Ewoms
{

class DeckRecord;

struct WellBrineProperties
{
    double m_saltConcentration = 0.0;
    void handleWSALT(const DeckRecord& rec);
    bool operator!=(const WellBrineProperties& other) const;
    bool operator==(const WellBrineProperties& other) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(m_saltConcentration);
    }
};

} // namespace Ewoms

#endif // EWOMS_WELLBRINEPROPERTIES_H
