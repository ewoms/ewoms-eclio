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

#ifndef EQLDIMS_H
#define EQLDIMS_H

/*
  The Eqldims class is a small utility class designed to hold on to
  the values from the EQLDIMS keyword.
*/

#include <ewoms/eclio/parser/parserkeywords/e.hh>

namespace Ewoms {
    class Eqldims {
    public:

        Eqldims() :
            m_ntequl( ParserKeywords::EQLDIMS::NTEQUL::defaultValue ),
            m_depth_nodes_p( ParserKeywords::EQLDIMS::DEPTH_NODES_P::defaultValue ),
            m_depth_nodes_tab( ParserKeywords::EQLDIMS::DEPTH_NODES_TAB::defaultValue ),
            m_nttrvd(  ParserKeywords::EQLDIMS::NTTRVD::defaultValue ),
            m_nstrvd(  ParserKeywords::EQLDIMS::NSTRVD::defaultValue )
        { }

        Eqldims( size_t ntequl , size_t depth_nodes_p , size_t depth_nodes_tab , size_t nttrvd , size_t nstrvd) :
            m_ntequl( ntequl ),
            m_depth_nodes_p( depth_nodes_p ),
            m_depth_nodes_tab( depth_nodes_tab ),
            m_nttrvd( nttrvd ),
            m_nstrvd( nstrvd )
        { }

        size_t getNumEquilRegions() const
        {
            return m_ntequl;
        }

        size_t getNumDepthNodesP() const
        {
            return m_depth_nodes_p;
        }

        size_t getNumDepthNodesTable() const
        {
            return m_depth_nodes_tab;
        }

        size_t getNumTracerTables() const
        {
            return m_nttrvd;
        }

        size_t getNumDepthNodesTracer() const
        {
            return m_nstrvd;
        }

        bool operator==(const Eqldims& data) const
        {
            return this->getNumEquilRegions() == data.getNumEquilRegions() &&
                   this->getNumDepthNodesP() == data.getNumDepthNodesP() &&
                   this->getNumDepthNodesTable() == data.getNumDepthNodesTable() &&
                   this->getNumTracerTables() == data.getNumTracerTables() &&
                   this->getNumDepthNodesTracer() == data.getNumDepthNodesTracer();
        }
    private:
        size_t m_ntequl , m_depth_nodes_p , m_depth_nodes_tab , m_nttrvd , m_nstrvd;

    };
}

#endif