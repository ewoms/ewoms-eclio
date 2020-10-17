// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
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
        double m_polymerConcentration = 0.0;
        double m_saltConcentration = 0.0;
        int m_plymwinjtable = -1;
        int m_skprwattable = -1;
        int m_skprpolytable = -1;

        static WellPolymerProperties serializeObject();

        bool operator==(const WellPolymerProperties& other) const;
        bool operator!=(const WellPolymerProperties& other) const;
        void handleWPOLYMER(const DeckRecord& record);
        void handleWPMITAB(const DeckRecord& record);
        void handleWSKPTAB(const DeckRecord& record);

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_polymerConcentration);
            serializer(m_saltConcentration);
            serializer(m_plymwinjtable);
            serializer(m_skprwattable);
            serializer(m_skprpolytable);
        }
    };
}

#endif
