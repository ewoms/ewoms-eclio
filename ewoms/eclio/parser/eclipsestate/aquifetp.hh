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

#ifndef EWOMS_AQUIFERFETP_H
#define EWOMS_AQUIFERFETP_H

/*
  The Aquiferfetp which stands for AquiferFetkovich is a data container object meant to hold the data for the fetkovich aquifer model.
  This includes the logic for parsing as well as the associated tables. It is meant to be used by ewoms-eclgrids and ewoms-eclsimulators in order to
  implement the Fetkovich analytical aquifer model in eflow.
*/

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/parserkeywords/a.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/aqudims.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablecontainer.hh>
#include <boost/concept_check.hpp>
#include <ewoms/eclio/parser/units/unitsystem.hh>

namespace Ewoms {

    class Aquifetp {
        public:

            struct AQUFETP_data{

                    // Aquifer ID
                    int aquiferID;
                    // Table IDs
                    int inftableID, pvttableID;
                    std::vector<int> cell_id;
                    // Variables constants
                    double  J, // Specified Productivity Index
			  rho, // water density in the aquifer
			  C_t, // total rock compressibility
			   V0, // initial volume of water in aquifer
			   d0; // aquifer datum depth
		    std::shared_ptr<double> p0; //Initial aquifer pressure at datum depth d0 - nullptr if the pressure has been defaulted.
            };

            Aquifetp(const Deck& deck);

            const std::vector<Aquifetp::AQUFETP_data>& getAquifers() const;
            int getAqPvtTabID(size_t aquiferIndex);

        private:

            std::vector<Aquifetp::AQUFETP_data> m_aqufetp;

    };
}

#endif
