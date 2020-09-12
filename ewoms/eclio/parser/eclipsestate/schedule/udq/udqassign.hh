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

#ifndef UDQASSIGN_HH_
#define UDQASSIGN_HH_

#include <string>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqset.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

class UDQAssign{
public:

    /*
      If the same keyword is assigned several times the different assignment
      records are assembled in one UDQAssign instance. This is an attempt to
      support restart in a situation where a full UDQ ASSIGN statement can be
      swapped with a UDQ DEFINE statement.
    */
    struct AssignRecord {
        std::vector<std::string> selector;
        double value;
        std::size_t report_step;

        bool operator==(const AssignRecord& data) const {
            return selector == data.selector &&
                report_step == data.report_step &&
                      value == data.value;
        }

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(selector);
            serializer(value);
            serializer(report_step);
        }
    };

    UDQAssign();
    UDQAssign(const std::string& keyword, const std::vector<std::string>& selector, double value, std::size_t report_step);

    static UDQAssign serializeObject();

    const std::string& keyword() const;
    UDQVarType var_type() const;
    void add_record(const std::vector<std::string>& selector, double value, std::size_t report_step);
    UDQSet eval(const std::vector<std::string>& wells) const;
    UDQSet eval() const;
    std::size_t report_step() const;

    bool operator==(const UDQAssign& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(m_keyword);
        serializer(m_var_type);
        serializer.vector(records);
    }

private:
    std::string m_keyword;
    UDQVarType m_var_type;
    std::vector<AssignRecord> records;
};
}

#endif
