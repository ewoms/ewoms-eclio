//===========================================================================
//
// File: ParameterTools.hpp
//
// Created: Tue Jun  2 19:02:19 2009
//
// Author(s): B�rd Skaflestad     <bard.skaflestad@sintef.no>
//            Atgeirr F Rasmussen <atgeirr@sintef.no>
//
// $Date$
//
// $Revision$
//
//===========================================================================

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

#ifndef EWOMS_PARAMETERTOOLS_H
#define EWOMS_PARAMETERTOOLS_H

#include <string>
#include <utility>

namespace Ewoms {
	std::pair<std::string, std::string> splitParam(const std::string& name);
} // namespace Ewoms

#endif // EWOMS_PARAMETERTOOLS_H
