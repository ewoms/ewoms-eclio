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
#ifndef EWOMS_PARSER_PLYSHLOG_TABLE_H
#define	EWOMS_PARSER_PLYSHLOG_TABLE_H

#include <ewoms/eclio/parser/parserkeywords/p.hh>
#include "simpletable.hh"

namespace Ewoms {

    class DeckRecord;
    class TableManager;

    class PlyshlogTable : public SimpleTable {
    public:
        friend class TableManager;

        PlyshlogTable(const DeckRecord& indexRecord, const DeckRecord& dataRecord);

        double getRefPolymerConcentration() const;
        double getRefSalinity() const;
        double getRefTemperature() const;
        void setRefPolymerConcentration(const double refPlymerConcentration);
        void setRefSalinity(const double refSalinity);
        void setRefTemperature(const double refTemperature);
        bool hasRefSalinity() const;
        bool hasRefTemperature() const;
        void setHasRefSalinity(const bool has);
        void setHasRefTemperature(const bool has);
        const TableColumn& getWaterVelocityColumn() const;
        const TableColumn& getShearMultiplierColumn() const;

    private:
        double m_refPolymerConcentration;
        double m_refSalinity;
        double m_refTemperature;

        bool m_hasRefSalinity;
        bool m_hasRefTemperature;
    };
}

#endif
