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

#ifndef EWOMS_AUQIFER_CONFIG_H
#define EWOMS_AUQIFER_CONFIG_H

#include <ewoms/eclio/parser/eclipsestate/aquancon.hh>
#include <ewoms/eclio/parser/eclipsestate/aquifetp.hh>
#include <ewoms/eclio/parser/eclipsestate/aquiferct.hh>

namespace Ewoms {

class TableManager;
class EclipseGrid;
class Deck;

class AquiferConfig {
public:
    AquiferConfig() = default;
    AquiferConfig(const TableManager& tables, const EclipseGrid& grid, const Deck& deck);
    AquiferConfig(const Aquifetp& fetp, const AquiferCT& ct, const Aquancon& conn);

    static AquiferConfig serializeObject();

    bool active() const;
    const AquiferCT& ct() const;
    const Aquifetp& fetp() const;
    const Aquancon& connections() const;
    bool operator==(const AquiferConfig& other);
    bool hasAquifer(const int aquID) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        aquifetp.serializeOp(serializer);
        aquiferct.serializeOp(serializer);
        aqconn.serializeOp(serializer);
    }

private:
    Aquifetp aquifetp;
    AquiferCT aquiferct;
    Aquancon aqconn;
};

}

#endif
