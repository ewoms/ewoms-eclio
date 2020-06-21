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

#include <cassert>
#include <cmath>
#include <iostream>

#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/parserkeywords/v.hh>
#include <ewoms/eclio/parser/units/dimension.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/vfpinjtable.hh>

//Anonymous namespace
namespace {

/**
 * Trivial helper function that throws if a zero-sized item is found.
 */
template <typename T>
inline const Ewoms::DeckItem& getNonEmptyItem( const Ewoms::DeckRecord& record) {
    const auto& retval = record.getItem<T>();
    if (!retval.hasValue(0)) {
        throw std::invalid_argument("Missing data");
    }
    return retval;
}

} //Namespace

namespace Ewoms {

VFPInjTable::VFPInjTable()
{
    m_table_num = -1;
    m_datum_depth = 0.0;
    m_flo_type = FLO_INVALID;
}

VFPInjTable::VFPInjTable( const DeckKeyword& table, const UnitSystem& deck_unit_system) {
    using ParserKeywords::VFPINJ;

    //Check that the table has enough records
    if (table.size() < 4) {
        throw std::invalid_argument("VFPINJ table does not appear to have enough records to be valid");
    }

    //Get record 1, the metadata for the table
    const auto& header = table.getRecord(0);

    //Get the different header items
    m_table_num   = getNonEmptyItem<VFPINJ::TABLE>(header).get< int >(0);
    m_datum_depth = getNonEmptyItem<VFPINJ::DATUM_DEPTH>(header).getSIDouble(0);

    m_flo_type = getFloType(getNonEmptyItem<VFPINJ::RATE_TYPE>(header).get< std::string >(0));

    //Not used, but check that PRESSURE_DEF is indeed THP
    std::string quantity_string = getNonEmptyItem<VFPINJ::PRESSURE_DEF>(header).get< std::string >(0);
    if (quantity_string != "THP") {
        throw std::invalid_argument("PRESSURE_DEF is required to be THP");
    }

    //Check units used for this table
    std::string units_string = "";
    if (header.getItem<VFPINJ::UNITS>().hasValue(0)) {
        units_string = header.getItem<VFPINJ::UNITS>().get< std::string >(0);
    }
    else {
        //If units does not exist in record, the default value is the
        //unit system of the deck itself: do nothing...
    }

    if (units_string != "") {
        UnitSystem::UnitType table_unit_type;

        //FIXME: Only metric and field supported at the moment.
        //Need to change all of the convertToSI functions to support LAB/PVT-M

        if (units_string == "METRIC") {
            table_unit_type = UnitSystem::UnitType::UNIT_TYPE_METRIC;
        }
        else if (units_string == "FIELD") {
            table_unit_type = UnitSystem::UnitType::UNIT_TYPE_FIELD;
        }
        else if (units_string == "LAB") {
            table_unit_type = UnitSystem::UnitType::UNIT_TYPE_FIELD;
        }
        else if (units_string == "PVT-M") {
            throw std::invalid_argument("Unsupported UNITS string: 'PVT-M'");
        }
        else {
            throw std::invalid_argument("Invalid UNITS string");
        }

        //Sanity check
        if(table_unit_type != deck_unit_system.getType()) {
            throw std::invalid_argument("Deck units are not equal VFPINJ table units.");
        }
    }

    //Quantity in the body of the table
    std::string body_string = getNonEmptyItem<VFPINJ::BODY_DEF>(header).get< std::string >(0);
    if (body_string != "BHP") {
        throw std::invalid_argument("Invalid BODY_DEF string");
    }

    //Get actual rate / flow values
    m_flo_data = getNonEmptyItem<VFPINJ::FLOW_VALUES>(table.getRecord(1)).getData< double >();
    convertFloToSI(m_flo_type, m_flo_data, deck_unit_system);

    //Get actual tubing head pressure values
    m_thp_data = getNonEmptyItem<VFPINJ::THP_VALUES>(table.getRecord(2)).getData< double >();
    convertTHPToSI(m_thp_data, deck_unit_system);

    //Finally, read the actual table itself.
    size_t nt = m_thp_data.size();
    size_t nf = m_flo_data.size();
    m_data.resize(nt*nf);
    std::fill_n(m_data.data(), m_data.size(), std::nan("0"));

    //Check that size of table matches size of axis:
    if (table.size() != nt + 3) {
        throw std::invalid_argument("VFPINJ table does not contain enough records.");
    }

    const double table_scaling_factor = deck_unit_system.parse("Pressure").getSIScaling();
    for (size_t i=3; i<table.size(); ++i) {
        const auto& record = table.getRecord(i);
        //Get indices (subtract 1 to get 0-based index)
        int t = getNonEmptyItem<VFPINJ::THP_INDEX>(record).get< int >(0) - 1;

        //Rest of values (bottom hole pressure or tubing head temperature) have index of flo value
        const std::vector<double>& bhp_tht = getNonEmptyItem<VFPINJ::VALUES>(record).getData< double >();

        if (bhp_tht.size() != nf) {
            throw std::invalid_argument("VFPINJ table does not contain enough FLO values.");
        }

        for (unsigned int f=0; f<bhp_tht.size(); ++f) {
            const double& value = bhp_tht[f];
            if (value > 1.0e10) {
                //TODO: Replace with proper log message
                std::cerr << "Too large value encountered in VFPINJ in ["
                        << t << "," << f << "]=" << value << std::endl;
            }
            (*this)(t,f) = table_scaling_factor*value;
        }
    }

    check();
}

VFPInjTable VFPInjTable::serializeObject()
{
    VFPInjTable result;
    result.m_table_num = 1;
    result.m_datum_depth = 2.0;
    result.m_flo_type = FLO_WAT;
    result.m_flo_data = {3.0, 4.0};
    result.m_thp_data = {5.0, 6.0};
    result.m_data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};

    return result;
}

void VFPInjTable::check() {
    //Table number
    assert(m_table_num > 0);

    //Misc types
    assert(m_flo_type >= FLO_OIL && m_flo_type < FLO_INVALID);

    //Data axis size
    assert(m_flo_data.size() > 0);
    assert(m_thp_data.size() > 0);

    //Data axis sorted?
    assert(is_sorted(m_flo_data.begin(), m_flo_data.end()));
    assert(is_sorted(m_thp_data.begin(), m_thp_data.end()));

    //Check data size matches axes
    assert(m_data.size() == m_thp_data.size() * m_flo_data.size());

    //Finally, check that all data is within reasonable ranges, defined to be up-to 1.0e10...
    typedef array_type::size_type size_type;
    for (size_type t = 0; t < m_thp_data.size(); ++t) {
        for (size_type f = 0; f < m_flo_data.size(); ++f) {
            if (std::isnan((*this)(t,f))) {
                //TODO: Replace with proper log message
                std::cerr << "VFPINJ element [" << t << "," << f << "] not set!" << std::endl;
                throw std::invalid_argument("Missing VFPINJ value");
            }
        }
    }
}

VFPInjTable::FLO_TYPE VFPInjTable::getFloType(std::string flo_string) {
    if (flo_string == "OIL") {
        return FLO_OIL;
    }
    else if (flo_string == "WAT") {
        return FLO_WAT;
    }
    else if (flo_string == "GAS") {
        return FLO_GAS;
    }
    else {
        throw std::invalid_argument("Invalid RATE_TYPE string");
    }
    return FLO_INVALID;
}

void VFPInjTable::scaleValues(std::vector<double>& values,
                               const double& scaling_factor) {
    if (scaling_factor == 1.0) {
        return;
    }
    else {
        for (unsigned int i=0; i<values.size(); ++i) {
            values[i] *= scaling_factor;
        }
    }
}

void VFPInjTable::convertFloToSI(const FLO_TYPE& type,
                                  std::vector<double>& values,
                                  const UnitSystem& unit_system) {
    double scaling_factor = 1.0;
    switch (type) {
        case FLO_OIL:
        case FLO_WAT:
            scaling_factor = unit_system.parse("LiquidSurfaceVolume/Time").getSIScaling();
            break;
        case FLO_GAS:
            scaling_factor = unit_system.parse("GasSurfaceVolume/Time").getSIScaling();
            break;
        default:
            throw std::logic_error("Invalid FLO type");
    }
    scaleValues(values, scaling_factor);
}

void VFPInjTable::convertTHPToSI(std::vector<double>& values,
                                 const UnitSystem& unit_system) {
    double scaling_factor = unit_system.parse("Pressure").getSIScaling();
    scaleValues(values, scaling_factor);
}

bool VFPInjTable::operator==(const VFPInjTable& data) const {
    return this->getTableNum() == data.getTableNum() &&
           this->getDatumDepth () == data.getDatumDepth() &&
           this->getFloType() == data.getFloType() &&
           this->getFloAxis() == data.getFloAxis() &&
           this->getTHPAxis() == data.getTHPAxis() &&
           this->getTable() == data.getTable();
}

double VFPInjTable::operator()(size_t thp_idx, size_t flo_idx) const {
    return m_data[thp_idx*m_flo_data.size() + flo_idx];
}

double& VFPInjTable::operator()(size_t thp_idx, size_t flo_idx) {
    return m_data[thp_idx*m_flo_data.size() + flo_idx];
}

std::array<size_t,2> VFPInjTable::shape() const {
  return {{m_thp_data.size(), m_flo_data.size()}};
}

} //Namespace
