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

#include <ewoms/eclio/output/aggregateactionxdata.hh>
#include <ewoms/eclio/output/aggregategroupdata.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqactive.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqdefine.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqassign.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionx.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqfunctiontable.hh>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>
#include <iostream>

// #####################################################################
// Class Ewoms::RestartIO::Helpers::AggregateGroupData
// ---------------------------------------------------------------------

namespace {
    const std::map<std::string, int> lhsQuantityToIndex = {
                                                           {"F",   1},
                                                           {"W",   2},
                                                           {"G",   3},
                                                           {"D",  10},
                                                           {"M",  11},
                                                           {"Y",  12},
    };

    const std::map<std::string, double> monthToNo = {
                                                           {"JAN",   1.},
                                                           {"FEB",   2.},
                                                           {"MAR",   3.},
                                                           {"APR",   4.},
                                                           {"MAY",   5.},
                                                           {"JUN",   6.},
                                                           {"JUL",   7.},
                                                           {"AUG",   8.},
                                                           {"SEP",   9.},
                                                           {"OCT",  10.},
                                                           {"NOV",  11.},
                                                           {"DEC",  12.},
    };

            const std::map<std::string, int> rhsQuantityToIndex = {
                                                           {"F",   1},
                                                           {"W",   2},
                                                           {"G",   3},
        };

        using logic_enum = Ewoms::Action::Condition::Logical;
        const std::map<logic_enum, int> logicalToIndex_13 = {
                                                          {logic_enum::AND,   1},
                                                          {logic_enum::OR,    2},
                                                          {logic_enum::END,   0},
        };

        const std::map<logic_enum, int> logicalToIndex_17 = {
                                                          {logic_enum::AND,   1},
                                                          {logic_enum::OR,    0},
                                                          {logic_enum::END,   0},
        };

        using cmp_enum = Ewoms::Action::Condition::Comparator;
        const std::map<cmp_enum, int> cmpToIndex = {
                                                          {cmp_enum::GREATER,       1},
                                                          {cmp_enum::LESS,          2},
                                                          {cmp_enum::GREATER_EQUAL, 3},
                                                          {cmp_enum::LESS_EQUAL,    4},
                                                          {cmp_enum::EQUAL,         5},
                                                          {cmp_enum::INVALID,       0},
        };

    namespace iACT {

        Ewoms::RestartIO::Helpers::WindowedArray<int>
        allocate(const std::vector<int>& actDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<int>;
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(actDims[0]) },
                WV::WindowSize{ static_cast<std::size_t>(actDims[1]) }
            };
        }

        template <class IACTArray>
        void staticContrib(const Ewoms::Action::ActionX& actx, IACTArray& iAct)
        {
            //item [0]: is unknown, (=0)
            iAct[0] = 0;
            //item [1]: The number of lines of schedule data including ENDACTIO
            iAct[1] = actx.keyword_strings().size();
            //item [2]: is unknown, (=1)
            iAct[2] = 1;
            //item [3]: is unknown, (=7)
            iAct[3] = 7;
            //item [4]: is unknown, (=0)
            iAct[4] = 0;
            //item [5]: The number of times the action is triggered
            iAct[5] = actx.max_run();
            //item [6]: is unknown, (=0)
            iAct[6] = 0;
            //item [7]: is unknown, (=0)
            iAct[7] = 0;
            //item [8]: The number of times the action is triggered
            iAct[8] = actx.conditions().size();
        }

    } // iAct

        namespace sACT {

        Ewoms::RestartIO::Helpers::WindowedArray<float>
        allocate(const std::vector<int>& actDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<float>;
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(actDims[0]) },
                WV::WindowSize{ static_cast<std::size_t>(actDims[2]) }
            };
        }

        template <class SACTArray>
        void staticContrib(SACTArray& sAct)
        {
            //item [0 - 4]: is unknown, (=0)
            for (std::size_t ind = 0; ind < 5 ; ind++) {
                sAct[ind] = 0;
            }
        }

    } // sAct

    namespace zACT {

        Ewoms::RestartIO::Helpers::WindowedArray<
            Ewoms::EclIO::PaddedOutputString<8>
        >
        allocate(const std::vector<int>& actDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<
                Ewoms::EclIO::PaddedOutputString<8>
            >;

            return WV {
                WV::NumWindows{ static_cast<std::size_t>(actDims[0]) },
                WV::WindowSize{ static_cast<std::size_t>(actDims[3]) }
            };
        }

    template <class ZACTArray>
    void staticContrib(const Ewoms::Action::ActionX& actx, ZACTArray& zAct)
    {
        // entry 1 is udq keyword
        zAct[0] = actx.name();
    }
    } // zAct

    namespace zLACT {

         Ewoms::RestartIO::Helpers::WindowedArray<
            Ewoms::EclIO::PaddedOutputString<8>
        >
        allocate(const std::vector<int>& actDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<
                Ewoms::EclIO::PaddedOutputString<8>
            >;

            return WV {
                WV::NumWindows{ static_cast<std::size_t>(actDims[0]) },
                WV::WindowSize{ static_cast<std::size_t>(actDims[4]) }
            };
        }

    template <class ZLACTArray>
    void staticContrib(const Ewoms::Action::ActionX& actx, int noEPrZlact, ZLACTArray& zLact)
        {
            std::size_t ind = 0;
            int l_sstr = 8;
            int max_l_str = 128;
            // write out the schedule input lines
            const auto& schedule_data = actx.keyword_strings();
            for (auto z_data : schedule_data) {
                int n_sstr =  z_data.size()/l_sstr;
                if (static_cast<int>(z_data.size()) > max_l_str) {
                    std::cout << "Too long input data string (max 128 characters): " << z_data << std::endl;
                    throw std::invalid_argument("Actionx: " + actx.name());
                }
                else {
                    for (int i = 0; i < n_sstr; i++) {
                        zLact[ind + i] = z_data.substr(i*l_sstr, l_sstr);
                    }
                    //add remainder of last non-zero string
                    if ((z_data.size() % l_sstr) > 0)
                        zLact[ind + n_sstr] = z_data.substr(n_sstr*l_sstr);
                }
                ind += static_cast<std::size_t>(noEPrZlact);
            }
        }
    } // zLact

    namespace zACN {

         Ewoms::RestartIO::Helpers::WindowedArray<
            Ewoms::EclIO::PaddedOutputString<8>
        >
        allocate(const std::vector<int>& actDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<
                Ewoms::EclIO::PaddedOutputString<8>
            >;

            return WV {
                WV::NumWindows{ static_cast<std::size_t>(actDims[0]) },
                WV::WindowSize{ static_cast<std::size_t>(actDims[5]) }
            };
        }

    template <class ZACNArray>
    void staticContrib(const Ewoms::Action::ActionX& actx, ZACNArray& zAcn)
        {
            std::size_t ind = 0;
            int noEPZacn = 13;
            // write out the schedule Actionx conditions
            const auto& actx_cond = actx.conditions();
            for (auto z_data : actx_cond) {
                // left hand quantity
                if ((z_data.lhs.quantity.substr(0,1) != "D") &&
                    (z_data.lhs.quantity.substr(0,1) != "M") &&
                    (z_data.lhs.quantity.substr(0,1) != "Y"))
                    zAcn[ind + 0] = z_data.lhs.quantity;
                // right hand quantity
                if ((z_data.rhs.quantity.substr(0,1) == "W") ||
                    (z_data.rhs.quantity.substr(0,1) == "G"))
                    zAcn[ind + 1] = z_data.rhs.quantity;
                // operator (comparator)
                zAcn[ind + 2] = z_data.cmp_string;
                // well-name if left hand quantity is a well quantity
                if (z_data.lhs.quantity.substr(0,1) == "W") {
                    zAcn[ind + 3] = z_data.lhs.args[0];
                }
                // well-name if right hand quantity is a well quantity
                if (z_data.rhs.quantity.substr(0,1) == "W") {
                    zAcn[ind + 4] = z_data.rhs.args[0];
                }

                // group-name if left hand quantity is a group quantity
                if (z_data.lhs.quantity.substr(0,1) == "G") {
                    zAcn[ind + 5] = z_data.lhs.args[0];
                }
                // group-name if right hand quantity is a group quantity
                if (z_data.rhs.quantity.substr(0,1) == "G") {
                    zAcn[ind + 6] = z_data.rhs.args[0];
                }

                //increment index according to no of items pr condition
                ind += static_cast<std::size_t>(noEPZacn);
            }
        }
    } // zAcn

}

    namespace iACN {

        Ewoms::RestartIO::Helpers::WindowedArray<int>
        allocate(const std::vector<int>& actDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<int>;
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(actDims[0]) },
                WV::WindowSize{ static_cast<std::size_t>(actDims[6]) }
            };
        }

        template <class IACNArray>
        void staticContrib(const Ewoms::Action::ActionX& actx, IACNArray& iAcn)
        {
            //item [0 - 9]: are unknown, (=0)

            /*item [10] type of quantity for condition
                1 for a field quantity (number of flowing producing wells)
                2 for a well quantity
                3 for a (node) group quantity
                //9 for a well group quantity
                10 for DAY
                11 for MNTH
                12 for YEAR
            */
            std::size_t ind = 0;
            int noEPZacn = 26;
            // write out the schedule Actionx conditions
            const auto& actx_cond = actx.conditions();
            for (auto cond_it = actx_cond.begin(); cond_it < actx_cond.end(); cond_it++) {
                auto z_data = *cond_it;
                // left hand quantity
                std::string lhsQtype = z_data.lhs.quantity.substr(0,1);
                const auto it_lhsq = lhsQuantityToIndex.find(lhsQtype);
                if (it_lhsq != lhsQuantityToIndex.end()) {
                    iAcn[ind + 10] = it_lhsq->second;
                }
                else {
                    std::cout << "Unknown condition type: " << z_data.lhs.quantity << std::endl;
                    throw std::invalid_argument("Actionx: " + actx.name());
                }

                /*item[11] - quantity type for rhs quantity
                    1 - for field variables
                    2 - for well variables?
                    3 - for group variables
                    8 - for constant values
                */
                iAcn[ind + 11] = 8;
                std::string rhsQtype = z_data.rhs.quantity.substr(0,1);
                const auto it_rhsq = rhsQuantityToIndex.find(rhsQtype);
                if (it_rhsq != rhsQuantityToIndex.end()) {
                    iAcn[ind + 11] = it_rhsq->second;
                }

                 /*item[12] - index for lhs type
                    1 - for MNTH
                    0 - for all other types
                */
                std::string lhsQ = z_data.lhs.quantity;
                if ( lhsQ == "MNTH") {
                    iAcn[ind + 12] = 1;
                }

                /*item [13] - relates to operator
                    OR   is 2
                    AND is 1
                */
                const auto it_logic_13 = logicalToIndex_13.find(z_data.logic);
                if (it_logic_13 != logicalToIndex_13.end()) {
                    iAcn[ind + 13] = it_logic_13->second;
                }
                else {
                    std::cout << "Unknown Boolean operator type for condition: " << z_data.lhs.quantity << std::endl;
                    throw std::invalid_argument("Actionx: " + actx.name());
                }

                /*item [16] - related to the operater used in ACTIONX for defined quantities
                    >     is  1
                    <     is  2
                    >=    is  3
                    <=    is  4
                    =     is  5
                */
                const auto it_cmp = cmpToIndex.find(z_data.cmp);
                if (it_cmp != cmpToIndex.end()) {
                    iAcn[ind + 16] = it_cmp->second;
                }
                else {
                    std::cout << "Unknown operator type for condition: " << z_data.lhs.quantity << std::endl;
                    throw std::invalid_argument("Actionx: " + actx.name());
                }

                /*item [17] - relates to operator and if the right hand condition is a constant or not
                 * First condition => [17] =  0
                 * Second+ conditions
                    *If the previous condition has a constant rhs => [17] =  0
                    *If rhs is {W,G, F} and
                        *If previous condition is AND => [17] =  1
                        *If previous condition is OR  => [17] =  0
                */
                if (cond_it > actx_cond.begin()) {
                    const std::string prev_rhs_quant = (cond_it-1)->rhs.quantity.substr(0,1);
                    const auto it_prev_rhs = rhsQuantityToIndex.find(prev_rhs_quant);
                    if (it_prev_rhs != rhsQuantityToIndex.end()) {
                    const auto it_logic_17 = logicalToIndex_17.find((cond_it-1)->logic);
                        if (it_logic_17 != logicalToIndex_17.end()) {
                            iAcn[ind + 17] = it_logic_17->second;
                        }
                        else {
                            std::cout << "Unknown Boolean operator type for condition: " << z_data.lhs.quantity << std::endl;
                            throw std::invalid_argument("Actionx: " + actx.name());
                        }
                    }
                }
                //increment index according to no of items pr condition
                ind += static_cast<std::size_t>(noEPZacn);
            }
        }
    } // iAcn

        namespace sACN {

        Ewoms::RestartIO::Helpers::WindowedArray<double>
        allocate(const std::vector<int>& actDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<double>;
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(actDims[0]) },
                WV::WindowSize{ static_cast<std::size_t>(actDims[7]) }
            };
        }

        template <class SACNArray>
        void staticContrib(const Ewoms::Action::ActionX& actx,
                           const Ewoms::SummaryState& st,
                           SACNArray& sAcn)
        {
            std::size_t ind = 0;
            int noEPZacn = 16;
            double undef_high_val = 1.0E+20;
            // write out the schedule Actionx conditions
            const auto& actx_cond = actx.conditions();
            for (const auto&  z_data : actx_cond) {

                // item [0 - 1] = 0 (unknown)
                sAcn[ind + 0] = 0.;
                sAcn[ind + 1] = 0.;

                //item  [2, 5, 7, 9]: value of condition 1 (zero if well, group or field variable
                const std::string& rhsQtype = z_data.rhs.quantity.substr(0,1);
                const auto& it_rhsq = rhsQuantityToIndex.find(rhsQtype);
                if (it_rhsq == rhsQuantityToIndex.end()) {
                    //come here if constant value condition
                    double t_val = 0.;
                    if (rhsQtype == "M") {
                       const auto& it_mnth = monthToNo.find(z_data.rhs.quantity);
                       if (it_mnth != monthToNo.end()) {
                           t_val = it_mnth->second;
                       }
                       else {
                            std::cout << "Unknown Month: " << z_data.rhs.quantity << std::endl;
                            throw std::invalid_argument("Actionx: " + actx.name() + "  Condition: " + z_data.lhs.quantity );
                        }
                    }
                    else {
                        t_val = std::stod(z_data.rhs.quantity);
                    }
                    sAcn[ind + 2] = t_val;
                    sAcn[ind + 5] = sAcn[ind + 2];
                    sAcn[ind + 7] = sAcn[ind + 2];
                    sAcn[ind + 9] = sAcn[ind + 2];
                }
                //Treat well, group and field right hand side conditions
                if (it_rhsq != rhsQuantityToIndex.end()) {
                    //Well variable
                    if (it_rhsq->first == "W") {
                        sAcn[ind + 4] = st.get_well_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 5] = st.get_well_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 6] = st.get_well_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 7] = st.get_well_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 8] = st.get_well_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 9] = st.get_well_var(z_data.rhs.args[0], z_data.rhs.quantity);
                    }
                    //group variable
                    if (it_rhsq->first == "G") {
                        sAcn[ind + 4] = st.get_group_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 5] = st.get_group_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 6] = st.get_group_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 7] = st.get_group_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 8] = st.get_group_var(z_data.rhs.args[0], z_data.rhs.quantity);
                        sAcn[ind + 9] = st.get_group_var(z_data.rhs.args[0], z_data.rhs.quantity);
                    }
                    //field variable
                    if (it_rhsq->first == "F") {
                        sAcn[ind + 4] = st.get(z_data.rhs.quantity);
                        sAcn[ind + 5] = st.get(z_data.rhs.quantity);
                        sAcn[ind + 6] = st.get(z_data.rhs.quantity);
                        sAcn[ind + 7] = st.get(z_data.rhs.quantity);
                        sAcn[ind + 8] = st.get(z_data.rhs.quantity);
                        sAcn[ind + 9] = st.get(z_data.rhs.quantity);
                    }
                }

                //treat cases with left hand side condition being: DAY, MNTH og YEAR variable
                const std::string& lhsQtype = z_data.lhs.quantity.substr(0,1);
                const auto& it_lhsq = lhsQuantityToIndex.find(lhsQtype);
                if ((it_lhsq->first == "D") || (it_lhsq->first == "M") || (it_lhsq->first == "Y")) {
                    sAcn[ind + 4] = undef_high_val;
                    sAcn[ind + 5] = undef_high_val;
                    sAcn[ind + 6] = undef_high_val;
                    sAcn[ind + 7] = undef_high_val;
                    sAcn[ind + 8] = undef_high_val;
                    sAcn[ind + 9] = undef_high_val;
                }

                //increment index according to no of items pr condition
                ind += static_cast<std::size_t>(noEPZacn);
            }
        }

    } // sAcn

// =====================================================================

Ewoms::RestartIO::Helpers::AggregateActionxData::
AggregateActionxData(const std::vector<int>& actDims)
    : iACT_ (iACT::allocate(actDims)),
      sACT_ (sACT::allocate(actDims)),
      zACT_ (zACT::allocate(actDims)),
      zLACT_(zLACT::allocate(actDims)),
      zACN_ (zACN::allocate(actDims)),
      iACN_ (iACN::allocate(actDims)),
      sACN_ (sACN::allocate(actDims))
{}

// ---------------------------------------------------------------------

void
Ewoms::RestartIO::Helpers::AggregateActionxData::
captureDeclaredActionxData( const Ewoms::Schedule&    sched,
                            const Ewoms::SummaryState& st,
                            const std::vector<int>& actDims,
                            const std::size_t       simStep)
{
    auto acts = sched.actions(simStep);
    std::size_t act_ind = 0;
    for (auto actx_it = acts.begin(); actx_it < acts.end(); actx_it++) {
        {
            auto i_act = this->iACT_[act_ind];
            iACT::staticContrib(*actx_it, i_act);
        }

        {
            auto s_act = this->sACT_[act_ind];
            sACT::staticContrib(s_act);
        }

        {
            auto z_act = this->zACT_[act_ind];
            zACT::staticContrib(*actx_it, z_act);
        }

        {
            auto z_lact = this->zLACT_[act_ind];
            zLACT::staticContrib(*actx_it, actDims[8], z_lact);
        }

        {
            auto z_acn = this->zACN_[act_ind];
            zACN::staticContrib(*actx_it, z_acn);
        }

        {
            auto i_acn = this->iACN_[act_ind];
            iACN::staticContrib(*actx_it, i_acn);
        }

        {
            auto s_acn = this->sACN_[act_ind];
            sACN::staticContrib(*actx_it, st, s_acn);
        }

        act_ind +=1;
    }
}

