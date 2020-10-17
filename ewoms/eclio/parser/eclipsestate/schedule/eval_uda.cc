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

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>

#include "well/injection.hh"
#include "eval_uda.hh"

namespace Ewoms {
namespace UDA {

  double eval_well_uda(const UDAValue& value, const std::string& well, const SummaryState& st, double udq_default) {
    if (value.is<double>())
        return value.getSI();

    const std::string& string_var = value.get<std::string>();
    double output_value = udq_default;

    if (st.has_well_var(well, value.get<std::string>()))
        output_value = st.get_well_var(well, string_var);
    else if (st.has(string_var))
        output_value = st.get(string_var);

    // We do not handle negative rates.
    // If negative rates occur a very small positive value is used to avoid 0.0
    // since 0.0 means default which is no rate limit (a large positive value)
    output_value = std::max(value.epsilonLimit(), output_value);
    return value.get_dim().convertRawToSi(output_value);
}

double eval_well_uda_rate(const UDAValue& value, const std::string& well, const SummaryState& st, double udq_default, InjectorType wellType, const UnitSystem& unitSystem) {
    double raw_rate = eval_well_uda(value, well, st, udq_default);
    return injection::rateToSI(raw_rate, wellType, unitSystem);
}

double eval_group_uda(const UDAValue& value, const std::string& group, const SummaryState& st, double udq_undefined) {
    if (value.is<double>())
        return value.getSI();

    const std::string& string_var = value.get<std::string>();
    double output_value = udq_undefined;

    if (st.has_group_var(group, value.get<std::string>()))
        output_value = st.get_group_var(group, string_var);
    else if (st.has(string_var))
        output_value = st.get(string_var);

    // We do not handle negative rates.
    // If negative rates occur a very small positive value is used to avoid 0.0
    // since 0.0 means default which is no rate limit (a large positive value)
    output_value = std::max(value.epsilonLimit(), output_value);
    return value.get_dim().convertRawToSi(output_value);
}

double eval_group_uda_rate(const UDAValue& value, const std::string& name, const SummaryState& st, double udq_undefined, Phase phase, const UnitSystem& unitSystem) {
    double raw_rate = eval_group_uda(value, name, st, udq_undefined);
    return injection::rateToSI(raw_rate, phase, unitSystem);
}

}
}
