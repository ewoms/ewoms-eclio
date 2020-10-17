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
#include <optional>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

#ifndef GROUPTREE2
#define GROUPTREE2

namespace Ewoms {

class GTNode {
public:
    GTNode(const Group& group, std::size_t level, const Ewoms::optional<std::string>& parent_name);

    void add_group(const GTNode& child_group);
    void add_well(const Well& well);

    const std::vector<Well>& wells() const;
    const std::vector<GTNode>& groups() const;
    const std::string& name() const;
    const std::string& parent_name() const;

    const Group& group() const;
    std::size_t level() const;
    std::vector<const GTNode*> all_nodes() const;
private:
    const Group m_group;
    std::size_t m_level;
    Ewoms::optional<std::string> m_parent_name;
    /*
      Class T with a stl container <T> - supposedly undefined behavior before
      C++17 - but it compiles without warnings.
    */
    std::vector<GTNode> m_child_groups;
    std::vector<Well> m_wells;
};

}
#endif

