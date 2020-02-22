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

#ifndef EWOMS_PARSER_ECLIPSE_ECLIPSESTATE_TABLES_VFPINJTABLE_HH_
#define EWOMS_PARSER_ECLIPSE_ECLIPSESTATE_TABLES_VFPINJTABLE_HH_

#include <array>
#include <vector>

namespace Ewoms {

class DeckKeyword;
class UnitSystem;

class VFPInjTable {
public:
    typedef std::vector<double> array_type;

    enum FLO_TYPE {
        FLO_OIL=1,
        FLO_WAT,
        FLO_GAS,
        FLO_INVALID
    };

    VFPInjTable();
    VFPInjTable(int table_num,
                double datum_depth,
                FLO_TYPE flo_type,
                const std::vector<double>& flo_data,
                const std::vector<double>& thp_data,
                const array_type& data);

    VFPInjTable(const DeckKeyword& table, const UnitSystem& deck_unit_system);

    inline int getTableNum() const {
        return m_table_num;
    }

    inline double getDatumDepth() const {
        return m_datum_depth;
    }

    inline FLO_TYPE getFloType() const {
        return m_flo_type;
    }

    inline const std::vector<double>& getFloAxis() const {
        return m_flo_data;
    }

    inline const std::vector<double>& getTHPAxis() const {
        return m_thp_data;
    }

    /**
     * Returns the data of the table itself. For ordered access
     * use operator()(thp_idx, flo_idx)
     *
     * This gives the bottom hole pressure value in the table for the coordinate
     * given by
     * flo_axis = getFloAxis();
     * thp_axis = getTHPAxis();
     *
     * flo_coord = flo_axis(flo_idx);
     * thp_coord = thp_axis(thp_idx);
     */
    inline const array_type& getTable() const {
        return m_data;
    }

    bool operator==(const VFPInjTable& data) const;

    std::array<size_t,2> shape() const;

    double operator()(size_t thp_idx, size_t flo_idx) const;

private:
    int m_table_num;
    double m_datum_depth;
    FLO_TYPE m_flo_type;

    std::vector<double> m_flo_data;
    std::vector<double> m_thp_data;

    array_type m_data;
    void check();

    double& operator()(size_t thp_idx, size_t flo_idx);

    static FLO_TYPE getFloType(std::string flo_string);

    static void scaleValues(std::vector<double>& values,
                            const double& scaling_factor);

    static void convertFloToSI(const FLO_TYPE& type,
                            std::vector<double>& values,
                            const UnitSystem& unit_system);
    static void convertTHPToSI(std::vector<double>& values,
                            const UnitSystem& unit_system);
};

}

#endif
