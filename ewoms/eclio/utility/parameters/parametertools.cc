//===========================================================================
//
// File: ParameterTools.cpp
//
// Created: Tue Jun  2 19:03:09 2009
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

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <ewoms/eclio/utility/parameters/parametertools.hh>
#include <ewoms/eclio/utility/parameters/parameterstrings.hh>

namespace Ewoms {
	std::pair<std::string, std::string> splitParam(const std::string& name)
        {
	    int pos = name.find(ID_delimiter_path);
	    if (pos == int(std::string::npos)) {
		return std::make_pair(name, "");
	    } else {
		return std::make_pair(name.substr(0, pos),
                                      name.substr(pos + ID_delimiter_path.size()));
	    }
	}
} // namespace Ewoms
