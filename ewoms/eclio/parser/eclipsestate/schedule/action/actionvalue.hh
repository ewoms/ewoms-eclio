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
#ifndef ACTION_VALUE_H
#define ACTION_VALUE_H

#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionresult.hh>

enum TokenType {
  number,        //  0
  ecl_expr,      //  1
  open_paren,    //  2
  close_paren,   //  3
  op_gt,         //  4
  op_ge,         //  5
  op_lt,         //  6
  op_le,         //  7
  op_eq,         //  8
  op_ne,         //  9
  op_and,        // 10
  op_or,         // 11
  end,           // 12
  error          // 13
};

enum class FuncType {
  none,
  time,
  region,
  field,
  group,
  well,
  well_segment,
  well_connection,
  Well_lgr,
  aquifer,
  block
};

namespace Ewoms {
namespace Action {

class Value {
public:
    explicit Value(double value);
    Value() = default;

    Result eval_cmp(TokenType op, const Value& rhs) const;
    void add_well(const std::string& well, double value);
    double scalar() const;

private:
    Action::Result eval_cmp_wells(TokenType op, double rhs) const;

    double scalar_value;
    double is_scalar = false;
    std::vector<std::pair<std::string, double>> well_values;
};

}
}
#endif
