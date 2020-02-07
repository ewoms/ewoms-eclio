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

#ifndef EWOMS_AQUIFERCT_H
#define EWOMS_AQUIFERCT_H

/*
  The AquiferCT which stands for AquiferCarterTracy is a data container object meant to hold the data for the aquifer carter tracy model.
  This includes the logic for parsing as well as the associated tables. It is meant to be used by ewoms-eclgrids and ewoms-eclsimulators in order to
  implement the Carter Tracy analytical aquifer model in eflow.
*/
#include <ewoms/eclio/parser/parserkeywords/a.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <ewoms/eclio/parser/eclipsestate/tables/aqudims.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablecontainer.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/aqutabtable.hh>

namespace Ewoms {

    class TableManager;

    class AquiferCT {
        public:

        struct AQUCT_data{

            AQUCT_data(const DeckRecord& record, const TableManager& tables);

            int aquiferID;
            int inftableID, pvttableID;

            double  phi_aq , // aquifer porosity
                    d0,      // aquifer datum depth
                    C_t ,    // total compressibility
                    r_o ,    // aquifer inner radius
                    k_a ,    // aquifer permeability
                    c1,      // 0.008527 (METRIC, PVT-M); 0.006328 (FIELD); 3.6 (LAB)
                    h ,      // aquifer thickness
                    theta ,  // angle subtended by the aquifer boundary
                    c2 ;     // 6.283 (METRIC, PVT-M); 1.1191 (FIELD); 6.283 (LAB).

            std::pair<bool, double> p0; //Initial aquifer pressure at datum depth, d0
            std::vector<double> td, pi;
            std::vector<int> cell_id;

            bool operator==(const AQUCT_data& other) const;
        };

        AquiferCT(const TableManager& tables, const Deck& deck);
        AquiferCT(const std::vector<AquiferCT::AQUCT_data>& data);

        std::size_t size() const;
        std::vector<AquiferCT::AQUCT_data>::const_iterator begin() const;
        std::vector<AquiferCT::AQUCT_data>::const_iterator end() const;
        const std::vector<AquiferCT::AQUCT_data>& data() const;
        bool operator==(const AquiferCT& other) const;
    private:
        std::vector<AquiferCT::AQUCT_data> m_aquct;
    };
}

#endif
