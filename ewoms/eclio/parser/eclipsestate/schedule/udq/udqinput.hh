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
#ifndef UDQINPUT__HH_
#define UDQINPUT__HH_

#include <memory>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

class UDQAssign;
class UDQDefine;

class UDQIndex {
public:
    UDQIndex() = default;

    UDQIndex(std::size_t insert_index_arg, std::size_t typed_insert_index_arg, UDQAction action_arg, UDQVarType var_type_arg) :
        insert_index(insert_index_arg),
        typed_insert_index(typed_insert_index_arg),
        action(action_arg),
        var_type(var_type_arg)
    {
    }

    static UDQIndex serializeObject()
    {
        UDQIndex result;
        result.insert_index = 1;
        result.typed_insert_index = 2;
        result.action = UDQAction::ASSIGN;
        result.var_type = UDQVarType::WELL_VAR;

        return result;
    }

    bool operator==(const UDQIndex& data) const {
        return insert_index == data.insert_index &&
               typed_insert_index == data.typed_insert_index &&
               action == data.action &&
               var_type == data.var_type;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(insert_index);
        serializer(typed_insert_index);
        serializer(action);
        serializer(var_type);
    }

    std::size_t insert_index;
    std::size_t typed_insert_index;
    UDQAction action;
    UDQVarType var_type;
};

class UDQInput{
public:
    UDQInput(const UDQIndex& index, const UDQDefine& udq_define, const std::string& unit);
    UDQInput(const UDQIndex& index, const UDQAssign& udq_assign, const std::string& unit);

    template<typename T>
    const T& get() const;

    template<typename T>
    bool is() const;

    const std::string& keyword() const;
    const UDQVarType& var_type() const;
    const std::string& unit() const;
    const UDQIndex index;
private:
    const UDQDefine * define;
    const UDQAssign * assign;
    const std::string m_keyword;
    UDQVarType m_var_type;
    const std::string m_unit;
};
}

#endif
