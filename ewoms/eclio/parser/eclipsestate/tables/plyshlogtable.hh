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

        PlyshlogTable() = default;
        PlyshlogTable(const DeckRecord& indexRecord, const DeckRecord& dataRecord);
        PlyshlogTable(const TableSchema& schema,
                      const OrderedMap<std::string, TableColumn>& columns,
                      bool jfunc,
                      double refPolymerConcentration,
                      double refSalinity,
                      double refTemperature,
                      bool hasRefSalinity,
                      bool hasRefTemperature);

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

        bool operator==(const PlyshlogTable& data) const;

    private:
        double m_refPolymerConcentration = 1.0;
        double m_refSalinity = 0.0;
        double m_refTemperature = 0.0;

        bool m_hasRefSalinity = false;
        bool m_hasRefTemperature = false;
    };
}

#endif
