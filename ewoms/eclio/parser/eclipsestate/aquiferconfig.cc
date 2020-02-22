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

#include <ewoms/eclio/parser/eclipsestate/aquiferconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>

#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>

namespace Ewoms {

AquiferConfig::AquiferConfig(const TableManager& tables, const EclipseGrid& grid, const Deck& deck):
    aquifetp(deck),
    aquiferct(tables, deck),
    aqconn(grid,deck)
{}

AquiferConfig::AquiferConfig(const Aquifetp& fetp, const AquiferCT& ct, const Aquancon& conn) :
    aquifetp(fetp),
    aquiferct(ct),
    aqconn(conn)
{}

bool AquiferConfig::active() const {
    return this->aqconn.active();
}

bool AquiferConfig::operator==(const AquiferConfig& other) {
    return this->aquifetp == other.aquifetp &&
           this->aquiferct == other.aquiferct &&
           this->aqconn == other.aqconn;
}

const AquiferCT& AquiferConfig::ct() const {
    return this->aquiferct;
}

const Aquifetp& AquiferConfig::fetp() const {
    return this->aquifetp;
}

const Aquancon& AquiferConfig::connections() const {
    return this->aqconn;
}

}
