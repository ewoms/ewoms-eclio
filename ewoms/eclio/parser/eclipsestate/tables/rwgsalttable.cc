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
#include "config.h"

#include <vector>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rwgsalttable.hh>

namespace Ewoms {

        static const size_t numEntries = 3;
        RwgsaltTable::RwgsaltTable()
        {
        }

        RwgsaltTable RwgsaltTable::serializeObject()
        {
            RwgsaltTable result;
            result.m_tableValues = {1.0, 2.0, 3.0};

            return result;
        }

        void RwgsaltTable::init(const Ewoms::DeckRecord& record1)
        {
            m_tableValues = record1.getItem("DATA").getSIDoubleData();
        }

        size_t RwgsaltTable::size() const
        {
            return m_tableValues.size()/numEntries;
        }

        const std::vector<double>& RwgsaltTable::getTableValues() const
        {
            return m_tableValues;
        }

        std::vector<double> RwgsaltTable::getPressureColumn() const
        {
            size_t tableindex = 0;
            std::vector<double> pressure(this->size());
            for(size_t i=0; i<this->size(); ++i){
                pressure[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return pressure;

        }

        std::vector<double> RwgsaltTable::getSaltConcentrationColumn() const
        {
            size_t tableindex = 1;
            std::vector<double> saltConc(this->size());
            for(size_t i=0; i<this->size(); ++i){
                saltConc[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return saltConc;

        }

        std::vector<double> RwgsaltTable::getVaporizedWaterGasRatioColumn() const
        {
            size_t tableindex = 2;
            std::vector<double> vaporizedwatergasratio(this->size());
            for(size_t i=0; i<this->size(); ++i){
                vaporizedwatergasratio[i] = m_tableValues[tableindex];
                tableindex = tableindex+numEntries;
            }
            return vaporizedwatergasratio;

        }

        bool RwgsaltTable::operator==(const RwgsaltTable& data) const
        {
            return m_tableValues == data.m_tableValues;
        }

}

