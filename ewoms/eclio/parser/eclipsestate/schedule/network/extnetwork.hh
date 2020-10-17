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
#ifndef EXT_NETWORK_H
#define EXT_NETWORK_H

#include <map>
#include <string>
#include <vector>

#include <ewoms/common/optional.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/network/branch.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/network/node.hh>

namespace Ewoms {
namespace Network {

class ExtNetwork {
public:
    ExtNetwork() = default;
    bool active() const;
    bool has_node(const std::string& name) const;
    void add_node(Node node);
    void add_branch(Branch branch);
    void drop_branch(const std::string& uptree_node, const std::string& downtree_node);
    const Node& node(const std::string& name) const;
    const Node& root() const;
    std::vector<Branch> downtree_branches(const std::string& node) const;
    Ewoms::optional<Branch> uptree_branch(const std::string& node) const;
    std::vector<std::string> node_names() const;

    bool operator==(const ExtNetwork& other) const;
    static ExtNetwork serializeObject();

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.vector(m_branches);
        serializer.map(m_nodes);
    }

private:
    std::vector<Branch> m_branches;
    std::map<std::string, Node> m_nodes;
};

}
}
#endif
