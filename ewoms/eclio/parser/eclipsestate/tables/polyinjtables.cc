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

/* This is the implementation for the following three keywords related to
 * polymer injectivity study :
 * PLYMWINJ, SKPRWAT, SKPRPOLY
 */

#include <ewoms/eclio/parser/deck/deckkeyword.hh>

#include <ewoms/eclio/parser/parserkeywords/p.hh>
#include <ewoms/eclio/parser/parserkeywords/s.hh>

#include <ewoms/eclio/parser/eclipsestate/tables/polyinjtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plymwinjtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/skprwattable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/skprpolytable.hh>

namespace Ewoms{

    // PolyInjTable

    PolyInjTable::PolyInjTable(const std::vector<double>& throughputs,
                               const std::vector<double>& velocities,
                               int tableNumber,
                               const std::vector<std::vector<double>>& data)
        : m_throughputs(throughputs)
        , m_velocities(velocities)
        , m_table_number(tableNumber)
        , m_data(data)
    {
    }

    int PolyInjTable::getTableNumber() const
    {
        return m_table_number;
    }

    const std::vector<double>& PolyInjTable::getThroughputs() const
    {
        return m_throughputs;
    }

    const std::vector<double>& PolyInjTable::getVelocities() const
    {
        return m_velocities;
    }

    const std::vector<std::vector<double>>& PolyInjTable::getTableData() const
    {
        return m_data;
    }

    bool PolyInjTable::operator==(const PolyInjTable& data) const
    {
        return this->getTableNumber() == data.getTableNumber() &&
               this->getThroughputs() == data.getThroughputs() &&
               this->getVelocities() == data.getVelocities() &&
               this->getTableData() == data.getTableData();
    }

    // PlymwinjTable

    PlymwinjTable::PlymwinjTable(const std::vector<double>& throughputs,
                                 const std::vector<double>& velocities,
                                 int tableNumber,
                                 const std::vector<std::vector<double>>& data)
        : PolyInjTable(throughputs, velocities, tableNumber, data)
    {
    }

    PlymwinjTable::PlymwinjTable(const Ewoms::DeckKeyword& table)
    {
        using namespace ParserKeywords;

        const DeckRecord& record0 = table.getRecord(0);

        m_table_number = record0.getItem<PLYMWINJ::TABLE_NUMBER>().get< int >(0);
        if (m_table_number <= 0) {
            const std::string msg = "PLYMWINJ table has non-positive table number " + std::to_string(m_table_number);
            throw std::invalid_argument(msg);
        }

        m_throughputs = table.getRecord(1).getItem<PLYMWINJ::THROUGHPUT>().getSIDoubleData();
        const size_t num_cols = m_throughputs.size();

        if (table.size() != num_cols + 3) {
            const std::string msg = "PLYMWINJ table " + std::to_string(m_table_number)
                                    + " does not have enough records!";
            throw std::invalid_argument(msg);
        }

        m_velocities = table.getRecord(2).getItem<PLYMWINJ::VELOCITY>().getSIDoubleData();
        const size_t num_rows = m_velocities.size();

        for (size_t i = 3; i < table.size(); ++i) {
            const DeckRecord& record_i = table.getRecord(i);
            const std::vector<double>& data_i = record_i.getItem<PLYMWINJ::MOLECULARWEIGHT>().getSIDoubleData();
            if (data_i.size() != num_rows) {
                const std::string msg = "PLYMWINJ table " + std::to_string(m_table_number)
                                        + " record " + std::to_string(i)
                                        + " does not have correct number of data ";
                throw std::invalid_argument(msg);
            }
            m_data.push_back(data_i);
        }
    }

    const std::vector<std::vector<double>>&
    PlymwinjTable::getMoleWeights() const
    {
        return getTableData();
    }

    bool PlymwinjTable::operator==(const PlymwinjTable& data) const
    {
        return static_cast<const PolyInjTable&>(*this) == static_cast<const PolyInjTable&>(data);
    }

    // SkprwatTable

    SkprwatTable::SkprwatTable(const std::vector<double>& throughputs,
                               const std::vector<double>& velocities,
                               int tableNumber,
                               const std::vector<std::vector<double>>& data)
        : PolyInjTable(throughputs, velocities, tableNumber, data)
    {
    }

    SkprwatTable::SkprwatTable(const Ewoms::DeckKeyword &table)
    {
        using namespace ParserKeywords;

        const DeckRecord& record0 = table.getRecord(0);

        m_table_number = record0.getItem<SKPRWAT::TABLE_NUMBER>().get< int >(0);
        if (m_table_number <= 0) {
            const std::string msg = "SKPRWAT table has non-positive table number " + std::to_string(m_table_number);
            throw std::invalid_argument(msg);
        }

        m_throughputs = table.getRecord(1).getItem<SKPRWAT::THROUGHPUT>().getSIDoubleData();
        const size_t num_cols = m_throughputs.size();

        if (table.size() != num_cols + 3) {
            const std::string msg = "SKPRWAT table " + std::to_string(m_table_number)
                                    + " does not have enough records!";
            throw std::invalid_argument(msg);
        }

        m_velocities = table.getRecord(2).getItem<SKPRWAT::VELOCITY>().getSIDoubleData();
        const size_t num_rows = m_velocities.size();

        for (size_t i = 3; i < table.size(); ++i) {
            const DeckRecord& record_i = table.getRecord(i);
            const std::vector<double>& data_i = record_i.getItem<SKPRWAT::SKINPRESSURE>().getSIDoubleData();
            if (data_i.size() != num_rows) {
                const std::string msg = "SKPRWAT table " + std::to_string(m_table_number)
                                        + " record " + std::to_string(i)
                                        + " does not have correct number of data ";
                throw std::invalid_argument(msg);
            }
            m_data.push_back(data_i);
        }
    }

    const std::vector<std::vector<double>>&
    SkprwatTable::getSkinPressures() const
    {
        return getTableData();
    }

    bool SkprwatTable::operator==(const SkprwatTable& data) const
    {
        return static_cast<const PolyInjTable&>(*this) == static_cast<const PolyInjTable&>(data);
    }

    // SkprpolyTable

    SkprpolyTable::SkprpolyTable(const std::vector<double>& throughputs,
                                 const std::vector<double>& velocities,
                                 int tableNumber,
                                 const std::vector<std::vector<double>>& data,
                                 double referenceConcentration)
        : PolyInjTable(throughputs, velocities, tableNumber, data)
        , m_ref_polymer_concentration(referenceConcentration)
    {
    }

    SkprpolyTable::SkprpolyTable(const Ewoms::DeckKeyword &table)
    {
        using namespace ParserKeywords;

        const DeckRecord& record0 = table.getRecord(0);

        m_table_number = record0.getItem<SKPRPOLY::TABLE_NUMBER>().get< int >(0);
        if (m_table_number <= 0) {
            const std::string msg = "SKPRPOLY table has non-positive table number " + std::to_string(m_table_number);
            throw std::invalid_argument(msg);
        }

        m_ref_polymer_concentration = record0.getItem<SKPRPOLY::POLYMERCONCENTRATION>().get< double >(0);
        if (m_ref_polymer_concentration <= 0.) {
            const std::string msg = "Non-positive reference polymer concentration is specified for SKPRPOLY table "
                                  + std::to_string(m_table_number);
            throw std::invalid_argument(msg);
        }

        m_throughputs = table.getRecord(1).getItem<SKPRPOLY::THROUGHPUT>().getSIDoubleData();
        const size_t num_cols = m_throughputs.size();

        if (table.size() != num_cols + 3) {
            const std::string msg = "SKPRPOLY table " + std::to_string(m_table_number)
                                    + " does not have enough records!";
            throw std::invalid_argument(msg);
        }

        m_velocities = table.getRecord(2).getItem<SKPRPOLY::VELOCITY>().getSIDoubleData();
        const size_t num_rows = m_velocities.size();

        for (size_t i = 3; i < table.size(); ++i) {
            const DeckRecord& record_i = table.getRecord(i);
            const std::vector<double>& data_i = record_i.getItem<SKPRPOLY::SKINPRESSURE>().getSIDoubleData();
            if (data_i.size() != num_rows) {
                const std::string msg = "SKPRPOLY table " + std::to_string(m_table_number)
                                        + " record " + std::to_string(i)
                                        + " does not have correct number of data ";
                throw std::invalid_argument(msg);
            }
            m_data.push_back(data_i);
        }
    }

    double SkprpolyTable::referenceConcentration() const
    {
        return m_ref_polymer_concentration;
    }

    const std::vector<std::vector<double>>&
    SkprpolyTable::getSkinPressures() const
    {
        return getTableData();
    }

    bool SkprpolyTable::operator==(const SkprpolyTable& data) const
    {
        return  this->referenceConcentration() == data.referenceConcentration() &&
                static_cast<const PolyInjTable&>(*this) == static_cast<const PolyInjTable&>(data);
    }

}
