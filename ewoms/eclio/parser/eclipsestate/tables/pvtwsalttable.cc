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

#include <vector>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pvtwsalttable.hh>

namespace Ewoms {

        static const size_t numEntries = 5;
        PvtwsaltTable::PvtwsaltTable()
        {
        }

        PvtwsaltTable::PvtwsaltTable(double refPressValue,
                                     double refSaltConValue,
                                     const std::vector<double>& tableValues)
            : m_pRefValues(refPressValue)
            , m_saltConsRefValues(refSaltConValue)
            , m_tableValues(tableValues)
        {
        }

        void PvtwsaltTable::init(const Ewoms::DeckRecord& record0, const Ewoms::DeckRecord& record1)
        {

            m_pRefValues = record0.getItem("P_REF").getSIDoubleData()[0];
            m_saltConsRefValues = record0.getItem("SALT_CONCENTRATION_REF").getSIDoubleData()[0];
            m_tableValues = record1.getItem("DATA").getSIDoubleData();
        }

        size_t PvtwsaltTable::size() const
        {
            return m_tableValues.size()/numEntries;
        }

        const std::vector<double>& PvtwsaltTable::getTableValues() const
        {
            return m_tableValues;
        }

        double PvtwsaltTable::getReferencePressureValue() const
        {
            return m_pRefValues;
        }

        double PvtwsaltTable::getReferenceSaltConcentrationValue() const
        {
            return m_saltConsRefValues;
        }

        std::vector<double> PvtwsaltTable::getSaltConcentrationColumn() const
        {
            size_t tableindex = 0;
            std::vector<double> saltCons(this->size());
            for(size_t i=0; i<this->size(); ++i){
                saltCons[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return saltCons;

        }

        std::vector<double> PvtwsaltTable::getFormationVolumeFactorColumn() const
        {
            size_t tableindex = 1;
            std::vector<double> formationvolumefactor(this->size());
            for(size_t i=0; i<this->size(); ++i){
                formationvolumefactor[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return formationvolumefactor;

        }

        std::vector<double> PvtwsaltTable::getCompressibilityColumn() const
        {
            size_t tableindex = 2;
            std::vector<double> compresibility(this->size());
            for(size_t i=0; i<this->size(); ++i){
                compresibility[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return compresibility;

        }

        std::vector<double> PvtwsaltTable::getViscosityColumn() const
        {
            size_t tableindex = 3;
            std::vector<double> viscosity(this->size());
            for(size_t i=0; i<this->size(); ++i){
                viscosity[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return viscosity;

        }

        std::vector<double> PvtwsaltTable::getViscosibilityColumn() const
        {
            size_t tableindex = 4;
            std::vector<double> viscosibility(this->size());
            for(size_t i=0; i<this->size(); ++i){
                viscosibility[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return viscosibility;

        }

        bool PvtwsaltTable::operator==(const PvtwsaltTable& data) const
        {
            return m_pRefValues == data.m_pRefValues &&
                   m_saltConsRefValues == data.m_saltConsRefValues &&
                   m_tableValues == data.m_tableValues;
        }

}

