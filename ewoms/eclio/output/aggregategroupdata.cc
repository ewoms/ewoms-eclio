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

#include <ewoms/eclio/output/aggregategroupdata.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/gtnode.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <stdexcept>

// #####################################################################
// Class Ewoms::RestartIO::Helpers::AggregateGroupData
// ---------------------------------------------------------------------

namespace {

// maximum number of groups
std::size_t ngmaxz(const std::vector<int>& inteHead)
{
    return inteHead[20];
}

// maximum number of wells in any group
int nwgmax(const std::vector<int>& inteHead)
{
    return inteHead[19];
}

template <typename GroupOp>
void groupLoop(const std::vector<const Ewoms::Group*>& groups,
               GroupOp&&                             groupOp)
{
    auto groupID = std::size_t{0};
    for (const auto* group : groups) {
        groupID += 1;

        if (group == nullptr) { continue; }

        groupOp(*group, groupID - 1);
    }
}

int currentGroupLevel(const Ewoms::Schedule& sched, const Ewoms::Group& group, const size_t simStep)
{
    if (group.defined( simStep )) {
        auto current = group;
        int level = 0;
        while (current.name() != "FIELD") {
            level += 1;
            current = sched.getGroup(current.parent(), simStep);
        }

        return level;
    } else {
        std::stringstream str;
        str << "actual group has not been defined at report time: " << simStep;
        throw std::invalid_argument(str.str());
    }
}

int groupType(const Ewoms::Group& group) {
    if (group.wellgroup())
        return 0;
    else
        return 1;
}

std::size_t groupSize(const Ewoms::Group& group) {
    if (group.wellgroup())
        return group.wells().size();
    else
        return group.groups().size();
}

namespace IGrp {
std::size_t entriesPerGroup(const std::vector<int>& inteHead)
{
    // INTEHEAD[36] = NIGRPZ
    return inteHead[36];
}

Ewoms::RestartIO::Helpers::WindowedArray<int>
allocate(const std::vector<int>& inteHead)
{
    using WV = Ewoms::RestartIO::Helpers::WindowedArray<int>;

    return WV {
        WV::NumWindows{ ngmaxz(inteHead) },
            WV::WindowSize{ entriesPerGroup(inteHead) }
    };
}

template <class IGrpArray>
void staticContrib(const Ewoms::Schedule&    sched,
                   const Ewoms::Group&      group,
                   const int               nwgmax,
                   const int               ngmaxz,
                   const std::size_t       simStep,
                   IGrpArray&              iGrp)
{
    if (group.wellgroup()) {
        int igrpCount = 0;
        //group has child wells
        //store the well number (sequence index) in iGrp according to the sequence they are defined
        for (const auto& well_name : group.wells()) {
            const auto& well = sched.getWell(well_name, simStep);
            iGrp[igrpCount] = well.seqIndex() + 1;
            igrpCount += 1;
        }
    } else if (!group.groups().empty()) {
        int igrpCount = 0;
        for (const auto& group_name : group.groups()) {
            const auto& child_group = sched.getGroup(group_name, simStep);
            iGrp[igrpCount] = child_group.insert_index();
            igrpCount += 1;
        }
    }

    //assign the number of child wells or child groups to
    // location nwgmax
    iGrp[nwgmax] = groupSize(group);
    iGrp[nwgmax + 26] = groupType(group);

    //find group level ("FIELD" is level 0) and store the level in
    //location nwgmax + 27
    iGrp[nwgmax+27] = currentGroupLevel(sched, group, simStep);

    // set values for group probably connected to GCONPROD settings
    //
    if (group.name() != "FIELD")
    {
        iGrp[nwgmax+ 5] = -1;
        iGrp[nwgmax+12] = -1;
        iGrp[nwgmax+17] = -1;
        iGrp[nwgmax+22] = -1;

        //assign values to group number (according to group sequence)
        iGrp[nwgmax+88] = group.insert_index();
        iGrp[nwgmax+89] = group.insert_index();
        iGrp[nwgmax+95] = group.insert_index();
        iGrp[nwgmax+96] = group.insert_index();
    }
    else
    {
        //assign values to group number (according to group sequence)
        iGrp[nwgmax+88] = ngmaxz;
        iGrp[nwgmax+89] = ngmaxz;
        iGrp[nwgmax+95] = ngmaxz;
        iGrp[nwgmax+96] = ngmaxz;
    }

    //find parent group and store index of parent group in
    //location nwgmax + 28
    if (group.name() == "FIELD")
        iGrp[nwgmax+28] = 0;
    else {
        const auto& parent_group = sched.getGroup(group.parent(), simStep);
        if (parent_group.name() == "FIELD")
            iGrp[nwgmax+28] = ngmaxz;
        else
            iGrp[nwgmax+28] = parent_group.insert_index();
    }
}
} // Igrp

namespace SGrp {
std::size_t entriesPerGroup(const std::vector<int>& inteHead)
{
    // INTEHEAD[37] = NSGRPZ
    return inteHead[37];
}

Ewoms::RestartIO::Helpers::WindowedArray<float>
allocate(const std::vector<int>& inteHead)
{
    using WV = Ewoms::RestartIO::Helpers::WindowedArray<float>;

    return WV {
        WV::NumWindows{ ngmaxz(inteHead) },
            WV::WindowSize{ entriesPerGroup(inteHead) }
    };
}

template <class SGrpArray>
void staticContrib(SGrpArray& sGrp)
{
    const auto dflt   = -1.0e+20f;
    const auto dflt_2 = -2.0e+20f;
    const auto infty  =  1.0e+20f;
    const auto zero   =  0.0f;
    const auto one    =  1.0f;

    const auto init = std::vector<float> { // 112 Items (0..111)
                                          // 0     1      2      3      4
                                          infty, infty, dflt , infty,  zero ,     //   0..  4  ( 0)
                                          zero , infty, infty, infty , infty,     //   5..  9  ( 1)
                                          infty, infty, infty, infty , dflt ,     //  10.. 14  ( 2)
                                          infty, infty, infty, infty , dflt ,     //  15.. 19  ( 3)
                                          infty, infty, infty, infty , dflt ,     //  20.. 24  ( 4)
                                          zero , zero , zero , dflt_2, zero ,     //  24.. 29  ( 5)
                                          zero , zero , zero , zero  , zero ,     //  30.. 34  ( 6)
                                          infty ,zero , zero , zero  , infty,     //  35.. 39  ( 7)
                                          zero , zero , zero , zero  , zero ,     //  40.. 44  ( 8)
                                          zero , zero , zero , zero  , zero ,     //  45.. 49  ( 9)
                                          zero , infty, infty, infty , infty,     //  50.. 54  (10)
                                          infty, infty, infty, infty , infty,     //  55.. 59  (11)
                                          infty, infty, infty, infty , infty,     //  60.. 64  (12)
                                          infty, infty, infty, infty , zero ,     //  65.. 69  (13)
                                          zero , zero , zero , zero  , zero ,     //  70.. 74  (14)
                                          zero , zero , zero , zero  , infty,     //  75.. 79  (15)
                                          infty, zero , infty, zero  , zero ,     //  80.. 84  (16)
                                          zero , zero , zero , zero  , zero ,     //  85.. 89  (17)
                                          zero , zero , one  , zero  , zero ,     //  90.. 94  (18)
                                          zero , zero , zero , zero  , zero ,     //  95.. 99  (19)
                                          zero , zero , zero , zero  , zero ,     // 100..104  (20)
                                          zero , zero , zero , zero  , zero ,     // 105..109  (21)
                                          zero , zero                             // 110..111  (22)
    };

    const auto sz = static_cast<
        decltype(init.size())>(sGrp.size());

    auto b = std::begin(init);
    auto e = b + std::min(init.size(), sz);

    std::copy(b, e, std::begin(sGrp));
}
} // SGrp

namespace XGrp {
std::size_t entriesPerGroup(const std::vector<int>& inteHead)
{
    // INTEHEAD[38] = NXGRPZ
    return inteHead[38];
}

Ewoms::RestartIO::Helpers::WindowedArray<double>
allocate(const std::vector<int>& inteHead)
{
    using WV = Ewoms::RestartIO::Helpers::WindowedArray<double>;

    return WV {
        WV::NumWindows{ ngmaxz(inteHead) },
            WV::WindowSize{ entriesPerGroup(inteHead) }
    };
}

// here define the dynamic group quantities to be written to the restart file
template <class XGrpArray>
void dynamicContrib(const std::vector<std::string>&      restart_group_keys,
                    const std::vector<std::string>&      restart_field_keys,
                    const std::map<std::string, size_t>& groupKeyToIndex,
                    const std::map<std::string, size_t>& fieldKeyToIndex,
                    const Ewoms::Group&                    group,
                    const Ewoms::SummaryState&             sumState,
                    XGrpArray&                           xGrp)
{
    std::string groupName = group.name();
    const std::vector<std::string>& keys = (groupName == "FIELD")
        ? restart_field_keys : restart_group_keys;
    const std::map<std::string, size_t>& keyToIndex = (groupName == "FIELD")
        ? fieldKeyToIndex : groupKeyToIndex;

    for (const auto& key : keys) {
        std::string compKey = (groupName == "FIELD")
            ? key : key + ":" + groupName;

        if (sumState.has(compKey)) {
            double keyValue = sumState.get(compKey);
            const auto itr = keyToIndex.find(key);
            xGrp[itr->second] = keyValue;
        }
    }
}
} // XGrp

namespace ZGrp {
std::size_t entriesPerGroup(const std::vector<int>& inteHead)
{
    // INTEHEAD[39] = NZGRPZ
    return inteHead[39];
}

Ewoms::RestartIO::Helpers::WindowedArray<
    Ewoms::EclIO::PaddedOutputString<8>
    >
allocate(const std::vector<int>& inteHead)
{
    using WV = Ewoms::RestartIO::Helpers::WindowedArray<
        Ewoms::EclIO::PaddedOutputString<8>
        >;

    return WV {
        WV::NumWindows{ ngmaxz(inteHead) },
            WV::WindowSize{ entriesPerGroup(inteHead) }
    };
}

template <class ZGroupArray>
void staticContrib(const Ewoms::Group& group, ZGroupArray& zGroup)
{
    zGroup[0] = group.name();
}
} // ZGrp
} // Anonymous

// =====================================================================

Ewoms::RestartIO::Helpers::AggregateGroupData::
AggregateGroupData(const std::vector<int>& inteHead)
    : iGroup_ (IGrp::allocate(inteHead))
    , sGroup_ (SGrp::allocate(inteHead))
    , xGroup_ (XGrp::allocate(inteHead))
    , zGroup_ (ZGrp::allocate(inteHead))
    , nWGMax_ (nwgmax(inteHead))
    , nGMaxz_ (ngmaxz(inteHead))
{}

// ---------------------------------------------------------------------

void
Ewoms::RestartIO::Helpers::AggregateGroupData::
captureDeclaredGroupData(const Ewoms::Schedule&                 sched,
                         const std::size_t                    simStep,
                         const Ewoms::SummaryState&             sumState,
                         const std::vector<int>&              inteHead)
{
    std::vector<const Ewoms::Group*> curGroups(ngmaxz(inteHead), nullptr);
    for (const auto& group_name : sched.groupNames(simStep)) {
        const auto& group = sched.getGroup(group_name, simStep);
        int ind = (group.name() == "FIELD")
            ? ngmaxz(inteHead)-1 : group.insert_index()-1;
        curGroups[ind] = std::addressof(group);
    }

    groupLoop(curGroups, [&sched, simStep, this]
              (const Group& group, const std::size_t groupID) -> void
                         {
                             auto ig = this->iGroup_[groupID];

                             IGrp::staticContrib(sched, group, this->nWGMax_, this->nGMaxz_,
                                                 simStep, ig);
                         });

    // Define Static Contributions to SGrp Array.
    groupLoop(curGroups,
              [this](const Group& /* group */, const std::size_t groupID) -> void
              {
                  auto sw = this->sGroup_[groupID];
                  SGrp::staticContrib(sw);
              });

    // Define Dynamic Contributions to XGrp Array.
    groupLoop(curGroups, [&sumState, this]
              (const Group& group, const std::size_t groupID) -> void
                         {
                             auto xg = this->xGroup_[groupID];

                             XGrp::dynamicContrib(this->restart_group_keys, this->restart_field_keys,
                                                  this->groupKeyToIndex, this->fieldKeyToIndex, group,
                                                  sumState, xg);
                         });

    // Define Static Contributions to ZGrp Array.
    groupLoop(curGroups, [this, &inteHead]
              (const Group& group, const std::size_t /* groupID */) -> void
                         {
                             std::size_t group_index = group.insert_index() - 1;
                             if (group.name() == "FIELD")
                                 group_index = ngmaxz(inteHead) - 1;
                             auto zg = this->zGroup_[ group_index ];

                             ZGrp::staticContrib(group, zg);
                         });
}
