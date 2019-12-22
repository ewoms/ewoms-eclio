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

#ifndef WELLPOLYMERPROPERTIES_HH_H
#define WELLPOLYMERPROPERTIES_HH_H

namespace Ewoms {

    class DeckRecord;

    struct WellPolymerProperties {
        double m_polymerConcentration;
        double m_saltConcentration;
        int m_plymwinjtable;
        int m_skprwattable;
        int m_skprpolytable;

        WellPolymerProperties(double polymerConcentration,
                              double saltConcentration,
                              int plymwinjtable,
                              int skprwattable,
                              int skprpolytable);

        bool operator==(const WellPolymerProperties& other) const;
        bool operator!=(const WellPolymerProperties& other) const;
        WellPolymerProperties();
        void handleWPOLYMER(const DeckRecord& record);
        void handleWPMITAB(const DeckRecord& record);
        void handleWSKPTAB(const DeckRecord& record);
    };
}

#endif
