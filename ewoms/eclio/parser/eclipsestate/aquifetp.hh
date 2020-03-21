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
#include <vector>

namespace Ewoms {

class Deck;
class DeckRecord;

class Aquifetp {
    public:

    struct AQUFETP_data{
        AQUFETP_data(const DeckRecord& record);
        AQUFETP_data() = default;
        AQUFETP_data(int aquiferID_, int pvttableID_, double J_, double C_t_, double V0_, double d0_, const std::pair<bool, double>& p0_);
        bool operator==(const AQUFETP_data& other) const;

        int aquiferID;
        int pvttableID;
        double  J, // Specified Productivity Index
                C_t, // total rock compressibility
                V0, // initial volume of water in aquifer
                d0; // aquifer datum depth
        std::pair<bool, double> p0;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(aquiferID);
            serializer(pvttableID);
            serializer(J);
            serializer(C_t);
            serializer(V0);
            serializer(d0);
            serializer(p0);
        }
    };

    Aquifetp() = default;
    Aquifetp(const Deck& deck);
    Aquifetp(const std::vector<Aquifetp::AQUFETP_data>& data);
    const std::vector<Aquifetp::AQUFETP_data>& data() const;

    std::size_t size() const;
    std::vector<Aquifetp::AQUFETP_data>::const_iterator begin() const;
    std::vector<Aquifetp::AQUFETP_data>::const_iterator end() const;
    bool operator==(const Aquifetp& other) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.vector(m_aqufetp);
    }

private:
    std::vector<Aquifetp::AQUFETP_data> m_aqufetp;
};
}

#endif
