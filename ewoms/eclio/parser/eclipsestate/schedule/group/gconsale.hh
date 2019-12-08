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

#ifndef GCONSALE_H
#define GCONSALE_H

#include <map>
#include <string>

#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>

namespace Ewoms {

    class GConSale {
    public:

        enum class MaxProcedure {
            NONE, CON, CON_P, WELL, PLUG, RATE, MAXR, END
        };

        struct GCONSALEGroup {
            UDAValue sales_target;
            UDAValue max_sales_rate;
            UDAValue min_sales_rate;
            MaxProcedure max_proc;
            double udq_undefined;
            UnitSystem unit_system;
        };

        struct GCONSALEGroupProp {
            double sales_target;
            double max_sales_rate;
            double min_sales_rate;
            MaxProcedure max_proc;
        };

        GConSale() = default;

        bool has(const std::string& name) const;
        const GCONSALEGroup& get(const std::string& name) const;
        const GCONSALEGroupProp get(const std::string& name, const SummaryState& st) const;
        static MaxProcedure stringToProcedure(const std::string& procedure);
        void add(const std::string& name, const UDAValue& sales_target, const UDAValue& max_rate, const UDAValue& min_rate, const std::string& procedure, double udq_undefined_arg, const UnitSystem& unit_system);
        size_t size() const;

    private:
        std::map<std::string, GCONSALEGroup> groups;
    };

}

#endif
