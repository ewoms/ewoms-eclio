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

#include <algorithm>
#include <stdexcept>

#include <ewoms/eclio/parser/eclipsestate/schedule/network/extnetwork.hh>

namespace Ewoms {
namespace Network {

ExtNetwork ExtNetwork::serializeObject() {
    ExtNetwork object;
    return object;
}

bool ExtNetwork::active() const {
    return !this->m_branches.empty() && !this->m_nodes.empty();
}

bool ExtNetwork::operator==(const ExtNetwork&) const {
    return true;
}

bool ExtNetwork::has_node(const std::string& name) const {
    return (this->m_nodes.count(name) > 0);
}

const Node& ExtNetwork::node(const std::string& name) const {
    const auto node_iter = this->m_nodes.find( name );
    if (node_iter == this->m_nodes.end())
        throw std::out_of_range("No such node: " + name);

    return node_iter->second;
}

const Node& ExtNetwork::root() const {
    if (this->m_nodes.empty())
        throw std::invalid_argument("No root defined for empty network");

    auto node_ptr = &(this->m_nodes.begin()->second);
    while (true) {
        auto next_branch = this->uptree_branch(node_ptr->name());
        if (!next_branch)
            break;

        node_ptr = &(this->node( next_branch->uptree_node() ));
    }

    return *node_ptr;
}

void ExtNetwork::add_branch(Branch branch)
{
    this->m_branches.push_back( std::move(branch) );
}

std::optional<Branch> ExtNetwork::uptree_branch(const std::string& node) const {
    if (!this->has_node(node))
        throw std::out_of_range("No such node: " + node);

    std::vector<Branch> branches;
    std::copy_if(this->m_branches.begin(), this->m_branches.end(), std::back_inserter(branches), [&node](const Branch& b) { return b.downtree_node() == node; });
    if (branches.empty())
        return {};

    if (branches.size() == 1)
        return std::move(branches[0]);

    throw std::logic_error("Bug - more than upstree branch for node: " + node);
}

std::vector<Branch> ExtNetwork::downtree_branches(const std::string& node) const {
    if (!this->has_node(node))
        throw std::out_of_range("No such node: " + node);

    std::vector<Branch> branches;
    std::copy_if(this->m_branches.begin(), this->m_branches.end(), std::back_inserter(branches), [&node](const Branch& b) { return b.uptree_node() == node; });
    return branches;
}

/*
  The validation of the network structure is very weak. The current validation
  goes as follows:

   1. A branch is defined with and uptree and downtree node; the node names used
      in the Branch definition is totally unchecked.

   2. When a node is added we check that the name of the node corresponds to a
      node name referred to in one of the previous branch definitions.

  The algorithm feels quite illogical, but from the documentation it seems to be
  the only possibility.
*/

void ExtNetwork::add_node(Node node)
{
    std::string name = node.name();
    auto branch = std::find_if(this->m_branches.begin(), this->m_branches.end(),
                               [&name](const Branch& b) { return b.uptree_node() == name || b.downtree_node() == name;});

    if (branch == this->m_branches.end())
        throw std::invalid_argument("Node: " + name + " is not referenced by any branch and would be dangling.");

    if (branch->downtree_node() == name) {
        if (node.as_choke() && branch->vfp_table().has_value())
            throw std::invalid_argument("Node: " + name + " should serve as a choke => upstream branch can not have VFP table");
    }

    this->m_nodes.insert({ name, std::move(node) });
}

}
}
